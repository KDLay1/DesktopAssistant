/**
 * @file calculateservice.cpp
 * @brief 计算服务实现文件
 *
 * 所属分层：Services（业务服务层）
 * 功能：提供账单收支汇总、周期汇总、账户余额快照等财务计算功能
 * 依赖：services/accountservice.h（同层服务）、models/ 中的模型类、Base 层的 Validator/OperationResult
 *
 * @author tys
 * @date 2026-06-08
 */

#include "services/calculateservice.h"
#include "services/accountservice.h" // 用于应用账单对余额的更改
#include <QVector>

// ==========================================
// 辅助校验函数
// ==========================================

/**
 * @brief 校验日期范围的有效性
 *
 * 检查开始日期和结束日期是否有效，以及开始日期是否不晚于结束日期。
 *
 * @param start 开始日期
 * @param end   结束日期
 * @return ValidationResult 校验结果，包含是否有效及错误信息
 */
static ValidationResult validateDates(const QDate& start, const QDate& end) {
    Validator<void> validator;
    validator.addRule([&]() { return start.isValid() && end.isValid(); }, "日期格式无效")
             .addRule([&]() { return start <= end; }, "开始日期不能晚于结束日期");
    return validator.validate();
}

// ==========================================
// 账单收支汇总实现
// ==========================================

/**
 * @brief 对账单列表进行收支汇总
 *
 * 根据账单类型（支出、支出冲正、收入、扩展退款、扩展、收入）分别汇总金额。
 * 注意：MoneyRecord 只重载了 operator+，没有重载 operator+=，因此使用显式加法。
 *
 * @param events 待汇总的账单列表
 * @return OperationResult<EventSummary> 包含汇总结果的操作结果
 *         - 成功：返回 EventSummary 对象，包含各类收支汇总金额
 *         - 失败：当前实现始终成功
 */
OperationResult<EventSummary> CalculateService::summary(const QList<Bill>& events) {
    MoneyRecord outflowWithReversal(0), outflowReversal(0), inflow(0);
    MoneyRecord extendRefund(0), extendWithRefund(0), income(0);

    for (const Bill& e : events) {
        // 注意：MoneyRecord 只重载了 operator+，没有重载 operator+=
        if (e.isOutflow()) outflowWithReversal = outflowWithReversal + e.amount();
        if (e.isOutflowReversal()) outflowReversal = outflowReversal + e.amount();
        if (e.isInflow()) inflow = inflow + e.amount();
        if (e.isExtendRefund()) extendRefund = extendRefund + e.amount();
        if (e.isExtend()) extendWithRefund = extendWithRefund + e.amount();
        if (e.isIncome()) income = income + e.amount();
    }

    return OperationResult<EventSummary>::success(
        EventSummary(outflowWithReversal, outflowReversal, inflow, extendRefund, extendWithRefund, income)
    );
}

// ==========================================
// 周期汇总实现
// ==========================================

/**
 * @brief 按指定周期对账单进行汇总
 *
 * 将账单按周期分桶，对每个桶内的账单进行汇总，并构建每个周期的起止日期边界。
 * 支持自定义周期划分逻辑（通过回调函数实现）。
 *
 * @param start 汇总起始日期
 * @param end   汇总结束日期
 * @param events 待汇总的账单列表
 * @param getPeriodNumberBetweenStartAndEnd 计算两个日期之间周期数的回调函数
 *        - 参数1: 起始日期
 *        - 参数2: 结束日期
 *        - 返回值: 周期数（从0开始）
 * @param getPeriodStartBoundary 获取指定周期起始日期的回调函数
 *        - 参数1: 起始日期
 *        - 参数2: 周期索引
 *        - 返回值: 该周期的起始日期
 * @return OperationResult<QList<FinancialSummary>> 包含周期汇总列表的操作结果
 *         - 成功：返回 FinancialSummary 列表，每个元素包含周期起止日期和汇总数据
 *         - 失败：日期校验不通过时返回错误信息
 */
OperationResult<QList<FinancialSummary>> CalculateService::summaryPerPeriod(
    const QDate& start, const QDate& end, const QList<Bill>& events,
    std::function<int(const QDate&, const QDate&)> getPeriodNumberBetweenStartAndEnd,
    std::function<QDate(const QDate&, int)> getPeriodStartBoundary) 
{
    // 校验日期范围有效性
    auto valRes = validateDates(start, end);
    if (!valRes.isValid()) return OperationResult<QList<FinancialSummary>>::failure(valRes.errorMessage());

    // 计算周期总数并初始化分桶数组
    int length = getPeriodNumberBetweenStartAndEnd(start, end) + 1;
    QVector<QList<Bill>> buckets(length);

    // 将账单分桶装入对应的周期
    for (const Bill& e : events) {
        QDate eventDate = QDate::fromJulianDay(e.dayNumber());
        int index = getPeriodNumberBetweenStartAndEnd(start, eventDate);
        if (index >= 0 && index < length) {
            buckets[index].append(e);
        }
    }

    // 对每个桶进行汇总
    QVector<EventSummary> summaries(length);
    for (int i = 0; i < length; ++i) {
        summaries[i] = summary(buckets[i]).data();
    }

    // 计算每个周期的起止日期边界
    QVector<QDate> boundaries(length + 1);
    boundaries[0] = start;
    boundaries[length] = end;
    for (int i = 1; i < length; i++) {
        boundaries[i] = getPeriodStartBoundary(start, i);
    }

    // 构建最终结果列表
    QList<FinancialSummary> res;
    if (length == 1) {
        // 只有一个周期时，直接使用起始和结束日期
        res.append(FinancialSummary(boundaries[0], boundaries[length], summaries[length - 1]));
        return OperationResult<QList<FinancialSummary>>::success(res);
    }

    // 第一个周期：从起始日期到第二个周期起始日期的前一天
    res.append(FinancialSummary(boundaries[0], boundaries[1].addDays(-1), summaries[0]));
    // 中间周期：从当前周期起始日期到下一个周期起始日期的前一天
    for (int i = 1; i < length - 1; i++) {
        res.append(FinancialSummary(boundaries[i], boundaries[i + 1].addDays(-1), summaries[i]));
    }
    // 最后一个周期：从最后一个周期起始日期到结束日期
    res.append(FinancialSummary(boundaries[length - 1], boundaries[length], summaries[length - 1]));

    return OperationResult<QList<FinancialSummary>>::success(res);
}

/**
 * @brief 按天对账单进行汇总
 *
 * 调用 summaryPerPeriod，传入按天划分的回调函数。
 *
 * @param start 汇总起始日期
 * @param end   汇总结束日期
 * @param events 待汇总的账单列表
 * @return OperationResult<QList<FinancialSummary>> 包含按天汇总列表的操作结果
 */
OperationResult<QList<FinancialSummary>> CalculateService::summaryPerDay(const QDate& start, const QDate& end, const QList<Bill>& events) {
    return summaryPerPeriod(start, end, events,
        [](const QDate& s, const QDate& e) { return s.daysTo(e); },
        [](const QDate& s, int index) { return s.addDays(index); });
}

/**
 * @brief 按周对账单进行汇总
 *
 * 调用 summaryPerPeriod，传入按周划分的回调函数。
 * 支持自定义每周的第一天。
 *
 * @param start 汇总起始日期
 * @param end   汇总结束日期
 * @param events 待汇总的账单列表
 * @param firstDayOfWeek 每周的第一天（默认为周一）
 * @return OperationResult<QList<FinancialSummary>> 包含按周汇总列表的操作结果
 */
OperationResult<QList<FinancialSummary>> CalculateService::summaryPerWeek(const QDate& start, const QDate& end, const QList<Bill>& events, Qt::DayOfWeek firstDayOfWeek) {
    return summaryPerPeriod(start, end, events,
        [=](const QDate& s, const QDate& e) { return DateExtension::getWeekNumber(e, firstDayOfWeek) - DateExtension::getWeekNumber(s, firstDayOfWeek); },
        [=](const QDate& s, int index) { return DateExtension::getFirstDayOfTheSameWeek(s, firstDayOfWeek).addDays(7 * index); });
}

/**
 * @brief 按月对账单进行汇总
 *
 * 调用 summaryPerPeriod，传入按月划分的回调函数。
 *
 * @param start 汇总起始日期
 * @param end   汇总结束日期
 * @param events 待汇总的账单列表
 * @return OperationResult<QList<FinancialSummary>> 包含按月汇总列表的操作结果
 */
OperationResult<QList<FinancialSummary>> CalculateService::summaryPerMonth(const QDate& start, const QDate& end, const QList<Bill>& events) {
    return summaryPerPeriod(start, end, events,
        [](const QDate& s, const QDate& e) { return (e.year() - s.year()) * 12 + (e.month() - s.month()); },
        [](const QDate& s, int index) { return DateExtension::getFirstDayOfTheSameMonth(s).addMonths(index); });
}

/**
 * @brief 按年对账单进行汇总
 *
 * 调用 summaryPerPeriod，传入按年划分的回调函数。
 *
 * @param start 汇总起始日期
 * @param end   汇总结束日期
 * @param events 待汇总的账单列表
 * @return OperationResult<QList<FinancialSummary>> 包含按年汇总列表的操作结果
 */
OperationResult<QList<FinancialSummary>> CalculateService::summaryPerYear(const QDate& start, const QDate& end, const QList<Bill>& events) {
    return summaryPerPeriod(start, end, events,
        [](const QDate& s, const QDate& e) { return e.year() - s.year(); },
        [](const QDate& s, int index) { return DateExtension::getFirstDayOfTheSameYear(s).addYears(index); });
}

// ==========================================
// 账户余额快照实现
// ==========================================

/**
 * @brief 对账单列表进行聚合，生成最终余额快照
 *
 * 从初始快照状态开始，逐条应用账单，更新对应账户的余额。
 * 如果账单中的账户ID在初始快照中不存在，则自动创建余额为0的账户。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return OperationResult<SnapshotItem> 包含最终余额快照的操作结果
 *         - 成功：返回包含最后账单日期和所有账户余额的快照
 *         - 失败：当前实现始终成功
 */
OperationResult<SnapshotItem> CalculateService::aggregate(const SnapshotItem& init, const QList<Bill>& events) {
    // 复制初始状态
    QMap<quint16, AccountBalance> res = init.states;
    for (const auto& e : events) {
        // 如果账户不存在，则创建余额为0的账户
        if (!res.contains(e.subjectId())) res.insert(e.subjectId(), AccountBalance(0));
        // 应用账单对余额的更改
        AccountService::apply(res[e.subjectId()], e);
    }
    // 计算最后账单日期（如果没有账单则使用初始快照的日期）
    int lastDay = events.isEmpty() ? init.dayNumber : events.last().dayNumber();
    return OperationResult<SnapshotItem>::success(SnapshotItem(lastDay, res));
}

/**
 * @brief 逐条应用账单，生成每次应用后的余额快照
 *
 * 从初始快照状态开始，每应用一条账单就记录一次当前所有账户的余额快照。
 * 如果账单中的账户ID在初始快照中不存在，则自动创建余额为0的账户。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return OperationResult<QList<SnapshotItem>> 包含每次应用账单后的快照列表的操作结果
 *         - 成功：返回快照列表，每个元素对应一条账单应用后的状态
 *         - 失败：当前实现始终成功（空账单列表返回空列表）
 */
OperationResult<QList<SnapshotItem>> CalculateService::scanPerEvent(const SnapshotItem& init, const QList<Bill>& events) {
    QList<SnapshotItem> resultList;
    if (events.isEmpty()) return OperationResult<QList<SnapshotItem>>::success(resultList);

    // 复制初始状态
    QMap<quint16, AccountBalance> res = init.states;
    for (const auto& e : events) {
        // 如果账户不存在，则创建余额为0的账户
        if (!res.contains(e.subjectId())) res.insert(e.subjectId(), AccountBalance(0));
        // 应用账单对余额的更改
        AccountService::apply(res[e.subjectId()], e);
        // 记录当前快照
        resultList.append(SnapshotItem(e.dayNumber(), res));
    }
    return OperationResult<QList<SnapshotItem>>::success(resultList);
}

/**
 * @brief 内部时间扫描引擎
 *
 * 按时间区间变化记录余额快照。当账单日期跨越不同的时间区间时，记录区间结束时的快照。
 * 初始快照始终被记录，最后一条账单后的快照也被记录。
 *
 * @param intervalFunc 计算日期所属时间区间编号的回调函数
 *        - 参数: 儒略日数
 *        - 返回值: 时间区间编号
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return QList<SnapshotItem> 包含按时间区间变化的快照列表
 *         - 始终包含初始快照
 *         - 每次时间区间变化时记录前一个区间结束时的快照
 *         - 最后包含所有账单应用后的最终快照
 */
QList<SnapshotItem> CalculateService::scanPerTime(std::function<int(int)> intervalFunc, const SnapshotItem& init, const QList<Bill>& events) {
    QList<SnapshotItem> resultList;
    // 记录初始快照
    resultList.append(SnapshotItem(init.dayNumber, init.states));

    if (events.isEmpty()) return resultList;

    // 初始化：获取第一条账单的时间区间编号
    int last = intervalFunc(events.first().dayNumber());
    int currentDateNum = 0;
    QMap<quint16, AccountBalance> res = init.states;

    for (const auto& e : events) {
        currentDateNum = e.dayNumber();
        int current = intervalFunc(currentDateNum);
        
        // 如果时间区间发生变化，记录前一个区间结束时的快照
        if (current != last) {
            resultList.append(SnapshotItem(currentDateNum - 1, res));
        }

        // 如果账户不存在，则创建余额为0的账户
        if (!res.contains(e.subjectId())) res.insert(e.subjectId(), AccountBalance(0));
        // 应用账单对余额的更改
        AccountService::apply(res[e.subjectId()], e);
        last = current;
    }
    // 记录所有账单应用后的最终快照
    resultList.append(SnapshotItem(currentDateNum, res));
    return resultList;
}

/**
 * @brief 按天扫描余额快照
 *
 * 调用 scanPerTime，传入按天划分的回调函数。
 * 每天记录一次余额快照。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return OperationResult<QList<SnapshotItem>> 包含按天扫描的快照列表的操作结果
 */
OperationResult<QList<SnapshotItem>> CalculateService::scanPerDay(const SnapshotItem& init, const QList<Bill>& events) {
    return OperationResult<QList<SnapshotItem>>::success(scanPerTime([](int d) { return d; }, init, events));
}

/**
 * @brief 按周扫描余额快照
 *
 * 调用 scanPerTime，传入按周划分的回调函数。
 * 每周记录一次余额快照。支持自定义每周的第一天。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @param firstDayOfWeek 每周的第一天（默认为周一）
 * @return OperationResult<QList<SnapshotItem>> 包含按周扫描的快照列表的操作结果
 */
OperationResult<QList<SnapshotItem>> CalculateService::scanPerWeek(const SnapshotItem& init, const QList<Bill>& events, Qt::DayOfWeek firstDayOfWeek) {
    return OperationResult<QList<SnapshotItem>>::success(scanPerTime([=](int d) { 
        return DateExtension::getWeekNumber(QDate::fromJulianDay(d), firstDayOfWeek); 
    }, init, events));
}

/**
 * @brief 按月扫描余额快照
 *
 * 调用 scanPerTime，传入按月划分的回调函数。
 * 每月记录一次余额快照。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return OperationResult<QList<SnapshotItem>> 包含按月扫描的快照列表的操作结果
 */
OperationResult<QList<SnapshotItem>> CalculateService::scanPerMonth(const SnapshotItem& init, const QList<Bill>& events) {
    return OperationResult<QList<SnapshotItem>>::success(scanPerTime([](int d) { 
        return QDate::fromJulianDay(d).month(); 
    }, init, events));
}

/**
 * @brief 按年扫描余额快照
 *
 * 调用 scanPerTime，传入按年划分的回调函数。
 * 每年记录一次余额快照。
 *
 * @param init   初始余额快照
 * @param events 待应用的账单列表
 * @return OperationResult<QList<SnapshotItem>> 包含按年扫描的快照列表的操作结果
 */
OperationResult<QList<SnapshotItem>> CalculateService::scanPerYear(const SnapshotItem& init, const QList<Bill>& events) {
    return OperationResult<QList<SnapshotItem>>::success(scanPerTime([](int d) { 
        return QDate::fromJulianDay(d).year(); 
    }, init, events));
}