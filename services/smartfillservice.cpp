/**
 * @file smartfillservice.cpp
 * @brief 智能填充服务实现文件
 * 
 * 所属分层：Services（业务服务层）
 * 功能：通过记忆用户历史操作模式，为特定对手方提供自动填充建议
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "services/smartfillservice.h"
#include "base/base_validator.h"

/**
 * @brief 构造函数
 * @param maxMemoryItems 最大记忆条目数，超过此数量将触发淘汰机制
 */
SmartFillService::SmartFillService(int maxMemoryItems)
    : m_maxMemoryItems(maxMemoryItems)
{
}

// ==========================================
// 获取填充建议
// ==========================================
/**
 * @brief 根据对手方ID获取智能填充建议
 * 
 * 首先校验参数有效性，然后查询记忆字典。
 * 如果找到历史记忆，则基于频数统计生成建议；
 * 否则返回空建议（所有字段为0）。
 * 
 * @param counterpartId 对手方ID（必须大于0）
 * @return OperationResult<FillSuggestion> 包含填充建议的操作结果
 *         - 成功：包含建议对象（可能为空建议）
 *         - 失败：包含错误信息（参数校验失败时）
 */
OperationResult<FillSuggestion> SmartFillService::getSuggestion(quint16 counterpartId) {
    // 1. 入参校验：使用Validator确保ID有效
    Validator<quint16> validator;
    validator.addRule([](quint16 id) { 
        return id > 0; 
    }, "无法获取建议：对手方ID无效");

    ValidationResult valRes = validator.validate(counterpartId);
    if (!valRes.isValid()) {
        return OperationResult<FillSuggestion>::failure(valRes.errorMessage());
    }

    // 2. 查找记忆字典：如果存在历史记录，则生成建议
    if (m_memoryDict.contains(counterpartId)) {
        return OperationResult<FillSuggestion>::success(
            createSuggestionFromMemory(m_memoryDict[counterpartId])
        );
    }

    // 未找到记忆，返回空建议（所有字段为0）
    return OperationResult<FillSuggestion>::success(FillSuggestion::empty());
}

// ==========================================
// 批量学习历史账单
// ==========================================
/**
 * @brief 批量学习历史账单，更新记忆字典
 * 
 * 遍历账单列表，过滤无效记录（对手方、科目、分类ID为0），
 * 为每个有效记录创建或更新对应的记忆条目。
 * 如果传入的账单日期更早，则更新记忆的起始日期。
 * 
 * @param events 历史账单列表
 * @return OperationResult<void> 操作结果（始终成功）
 */
OperationResult<void> SmartFillService::learnFromEventsBatch(const QList<Bill>& events) {
    if (events.isEmpty()) {
        return OperationResult<void>::success();
    }

    for (const Bill& record : events) {
        quint16 counterpart = record.counterpartId();
        quint16 subject = record.subjectId();
        quint16 categoryId = record.categoryId();

        // 过滤无效记录：任何关键ID为0则跳过
        if (counterpart == 0 || subject == 0 || categoryId == 0) {
            continue; 
        }

        QDate recordDate = QDate::fromJulianDay(record.dayNumber());
        FieldMemory& memory = getOrCreateMemory(counterpart, recordDate);

        // 如果传入了更早的账单，更新记忆的起始日期
        if (recordDate < memory.createDate) {
            memory.createDate = recordDate;
        }

        // 频数累加：记录该对手方下各科目和分类的出现次数
        memory.subjectFrequency[subject]++;
        memory.categoryFrequency[categoryId]++;
        memory.usageCount++;
    }

    return OperationResult<void>::success();
}

// ==========================================
// 内部机制：获取或创建记忆条目（附带淘汰算法）
// ==========================================
/**
 * @brief 获取或创建指定对手方的记忆条目
 * 
 * 如果记忆已存在，直接返回引用。
 * 如果达到容量上限，触发淘汰机制：优先淘汰使用频率最低且最早创建的条目。
 * 淘汰后创建新条目并初始化。
 * 
 * @param counterpart 对手方ID
 * @param date 当前账单日期（用于初始化新条目的创建日期）
 * @return FieldMemory& 记忆条目的引用
 * 
 * @note 淘汰算法使用双重优先级：使用频率（低优先淘汰）> 创建日期（早优先淘汰）
 */
SmartFillService::FieldMemory& SmartFillService::getOrCreateMemory(quint16 counterpart, const QDate& date) {
    if (m_memoryDict.contains(counterpart)) {
        return m_memoryDict[counterpart];
    }

    // 如果达到容量上限，触发淘汰机制 (优先淘汰使用频率最低且最早创建的)
    if (m_memoryDict.size() >= m_maxMemoryItems) {
        quint16 keyToRemove = 0;
        int minUsage = -1;
        QDate earliestDate = QDate(9999,12,31);

        for (auto it = m_memoryDict.constBegin(); it != m_memoryDict.constEnd(); ++it) {
            const FieldMemory& m = it.value();
            if (minUsage == -1 || m.usageCount < minUsage || 
               (m.usageCount == minUsage && m.createDate < earliestDate)) {
                minUsage = m.usageCount;
                earliestDate = m.createDate;
                keyToRemove = it.key();
            }
        }
        
        if (keyToRemove != 0) {
            m_memoryDict.remove(keyToRemove);
        }
    }

    // 初始化新记忆：设置字段值、创建日期，使用次数初始为0
    FieldMemory newMemory;
    newMemory.fieldValue = counterpart;
    newMemory.createDate = date;
    newMemory.usageCount = 0;
    
    m_memoryDict.insert(counterpart, newMemory);
    return m_memoryDict[counterpart];
}

// ==========================================
// 内部机制：从频数统计中推导出最佳建议
// ==========================================
/**
 * @brief 从记忆条目的频数统计中推导出最佳填充建议
 * 
 * 分别找出出现频率最高的科目和分类，
 * 将历史录入次数作为置信度。
 * 
 * @param memory 记忆条目（包含频数统计）
 * @return FillSuggestion 填充建议对象
 *         - counterpart: 对手方ID
 *         - suggestedSubject: 最高频科目ID（0表示无记录）
 *         - suggestedCategoryId: 最高频分类ID（0表示无记录）
 *         - confidence: 历史录入次数（作为置信度指标）
 */
FillSuggestion SmartFillService::createSuggestionFromMemory(const FieldMemory& memory) const {
    // 找出最高频的主体账户
    quint16 bestSubject = 0;
    int maxSubjFreq = 0;
    for (auto it = memory.subjectFrequency.constBegin(); it != memory.subjectFrequency.constEnd(); ++it) {
        if (it.value() > maxSubjFreq) {
            maxSubjFreq = it.value();
            bestSubject = it.key();
        }
    }

    // 找出最高频的分类
    quint16 bestCategory = 0;
    int maxCatFreq = 0;
    for (auto it = memory.categoryFrequency.constBegin(); it != memory.categoryFrequency.constEnd(); ++it) {
        if (it.value() > maxCatFreq) {
            maxCatFreq = it.value();
            bestCategory = it.key();
        }
    }

    FillSuggestion suggestion;
    suggestion.counterpart = memory.fieldValue;
    suggestion.suggestedSubject = bestSubject;
    suggestion.suggestedCategoryId = bestCategory;
    suggestion.confidence = memory.usageCount; // 将历史录入次数作为置信度

    return suggestion;
}

// ==========================================
// 维护方法
// ==========================================
/**
 * @brief 清空所有记忆数据
 * 
 * 重置服务状态，删除所有已学习的记忆条目。
 */
void SmartFillService::clear() {
    m_memoryDict.clear();
}

/**
 * @brief 获取当前记忆条目数量
 * @return int 记忆字典中的条目数
 */
int SmartFillService::memoryCount() const {
    return m_memoryDict.size();
}