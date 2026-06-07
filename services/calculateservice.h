/**
 * @file calculateservice.h
 * @brief 服务层 - 核心财务计算服务
 *
 * 本文件定义了 CalculateService 类，提供账单汇总、按时间维度生成财务摘要、
 * 以及计算账户余额历史快照等核心业务逻辑。
 *
 * 所属分层: Services (服务层)
 * 依赖关系:
 *   - Base 层: base_operationresult.h, base_validator.h, date_extension.h
 *   - Models 层: bill.h, eventsummary.h, financialsummary.h, AccountBalance.h
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef SERVICES_CALCULATESERVICE_H
#define SERVICES_CALCULATESERVICE_H

#include <QList>
#include <QDate>
#include <QMap>
#include <functional>

#include "base/base_operationresult.h"
#include "base/base_validator.h"
#include "base/date_extension.h"
#include "models/bill.h"
#include "models/eventsummary.h"
#include "models/financialsummary.h"
#include "models/AccountBalance.h"

/**
 * @brief 账户余额快照项
 *
 * 表示在某个特定时间点（由 dayNumber 标识）所有账户的余额状态。
 * 用于追踪账户余额随时间的变化轨迹。
 */
struct SnapshotItem {
    int dayNumber; /**< 从基准日期（如项目起始日）开始的天数，用于在时间轴上定位此快照 */
    QMap<quint16, AccountBalance> states; /**< 账户ID到余额的映射，记录该时刻所有账户的余额 */

    /**
     * @brief 构造函数
     * @param day 快照对应的天数，默认为0
     * @param s 账户余额状态映射，默认为空映射
     */
    SnapshotItem(int day = 0, const QMap<quint16, AccountBalance>& s = QMap<quint16, AccountBalance>())
        : dayNumber(day), states(s) {}
};

/**
 * @brief 财务计算服务类
 *
 * 提供一组静态方法，用于执行各种财务计算。
 * 所有公开方法均返回 OperationResult，以统一处理成功结果或错误信息。
 * 该类为工具类，不包含成员变量，所有方法均为静态。
 */
class CalculateService {
public:
    // ==========================================
    // 账单收支汇总 (EventSummary)
    // ==========================================

    /**
     * @brief 汇总账单列表的收支情况
     *
     * 遍历给定的账单列表，计算总收入、总支出和净收入，并封装到 EventSummary 中返回。
     *
     * @param events 待汇总的账单列表
     * @return OperationResult<EventSummary> 包含收支汇总结果，或错误信息
     */
    static OperationResult<EventSummary> summary(const QList<Bill>& events);

    // ==========================================
    // 按时间维度汇总 (FinancialSummary)
    // ==========================================

    /**
     * @brief 按天汇总财务摘要
     * @param start 汇总起始日期
     * @param end 汇总结束日期
     * @param events 待汇总的账单列表
     * @return OperationResult<QList<FinancialSummary>> 按天分组的财务摘要列表
     */
    static OperationResult<QList<FinancialSummary>> summaryPerDay(
        const QDate& start, const QDate& end, const QList<Bill>& events);

    /**
     * @brief 按周汇总财务摘要
     * @param start 汇总起始日期
     * @param end 汇总结束日期
     * @param events 待汇总的账单列表
     * @param firstDayOfWeek 每周的第一天，默认为周日
     * @return OperationResult<QList<FinancialSummary>> 按周分组的财务摘要列表
     */
    static OperationResult<QList<FinancialSummary>> summaryPerWeek(
        const QDate& start, const QDate& end, const QList<Bill>& events, Qt::DayOfWeek firstDayOfWeek = Qt::Sunday);

    /**
     * @brief 按月汇总财务摘要
     * @param start 汇总起始日期
     * @param end 汇总结束日期
     * @param events 待汇总的账单列表
     * @return OperationResult<QList<FinancialSummary>> 按月分组的财务摘要列表
     */
    static OperationResult<QList<FinancialSummary>> summaryPerMonth(
        const QDate& start, const QDate& end, const QList<Bill>& events);

    /**
     * @brief 按年汇总财务摘要
     * @param start 汇总起始日期
     * @param end 汇总结束日期
     * @param events 待汇总的账单列表
     * @return OperationResult<QList<FinancialSummary>> 按年分组的财务摘要列表
     */
    static OperationResult<QList<FinancialSummary>> summaryPerYear(
        const QDate& start, const QDate& end, const QList<Bill>& events);

    /**
     * @brief 通用周期汇总核心逻辑（高阶函数）
     *
     * 通过传入两个回调函数，将“如何划分周期”的逻辑抽象出来，实现灵活的周期汇总。
     * 内部逻辑：计算总周期数 -> 遍历每个周期 -> 筛选该周期内的账单 -> 调用 summary() 汇总 -> 收集结果。
     *
     * @param start 汇总起始日期
     * @param end 汇总结束日期
     * @param events 待汇总的账单列表
     * @param getPeriodNumberBetweenStartAndEnd 函数对象，用于计算两个日期之间有多少个周期
     * @param getPeriodStartBoundary 函数对象，用于根据周期序号计算该周期的起始日期
     * @return OperationResult<QList<FinancialSummary>> 按周期分组的财务摘要列表
     */
    static OperationResult<QList<FinancialSummary>> summaryPerPeriod(
        const QDate& start, const QDate& end, const QList<Bill>& events,
        std::function<int(const QDate&, const QDate&)> getPeriodNumberBetweenStartAndEnd,
        std::function<QDate(const QDate&, int)> getPeriodStartBoundary);

    // ==========================================
    // 账户余额快照计算 (SnapshotItem)
    // ==========================================

    /**
     * @brief 聚合并计算最终快照
     *
     * 从初始快照开始，遍历所有账单事件，将每个事件的影响应用到快照中对应的账户余额上。
     *
     * @param init 初始账户余额快照
     * @param events 待应用的账单事件列表
     * @return OperationResult<SnapshotItem> 应用所有事件后的最终快照
     */
    static OperationResult<SnapshotItem> aggregate(const SnapshotItem& init, const QList<Bill>& events);

    /**
     * @brief 逐笔事件扫描快照
     *
     * 从初始快照开始，逐个应用账单事件。每应用一个事件，就生成一个新的快照并添加到结果列表中。
     * 返回的快照列表长度 = 事件数量 + 1（包含初始快照）。
     *
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @return OperationResult<QList<SnapshotItem>> 每个事件发生后的快照列表
     */
    static OperationResult<QList<SnapshotItem>> scanPerEvent(const SnapshotItem& init, const QList<Bill>& events);

    /**
     * @brief 按天扫描快照
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @return OperationResult<QList<SnapshotItem>> 每天结束时的快照列表
     */
    static OperationResult<QList<SnapshotItem>> scanPerDay(const SnapshotItem& init, const QList<Bill>& events);

    /**
     * @brief 按周扫描快照
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @param firstDayOfWeek 每周的第一天，默认为周日
     * @return OperationResult<QList<SnapshotItem>> 每周结束时的快照列表
     */
    static OperationResult<QList<SnapshotItem>> scanPerWeek(const SnapshotItem& init, const QList<Bill>& events, Qt::DayOfWeek firstDayOfWeek = Qt::Sunday);

    /**
     * @brief 按月扫描快照
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @return OperationResult<QList<SnapshotItem>> 每月结束时的快照列表
     */
    static OperationResult<QList<SnapshotItem>> scanPerMonth(const SnapshotItem& init, const QList<Bill>& events);

    /**
     * @brief 按年扫描快照
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @return OperationResult<QList<SnapshotItem>> 每年结束时的快照列表
     */
    static OperationResult<QList<SnapshotItem>> scanPerYear(const SnapshotItem& init, const QList<Bill>& events);

private:
    /**
     * @brief 内部通用时间扫描器
     *
     * 遍历所有事件，根据 intervalFunc 判断当前事件属于哪个周期。
     * 如果事件进入了新的周期，则记录上一个周期的最终快照。
     *
     * @note 此函数直接返回 QList<SnapshotItem>，而非 OperationResult。
     *       调用者需确保输入数据有效，或在使用前进行校验。
     *
     * @param intervalFunc 函数对象，输入天数，返回该天数所属的周期序号
     * @param init 初始账户余额快照
     * @param events 待扫描的账单事件列表
     * @return QList<SnapshotItem> 每个周期结束时的快照列表
     */
    static QList<SnapshotItem> scanPerTime(
        std::function<int(int)> intervalFunc, const SnapshotItem& init, const QList<Bill>& events);
};

#endif // SERVICES_CALCULATESERVICE_H