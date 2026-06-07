/**
 * @file smartfillservice.h
 * @brief 智能填充服务头文件
 *
 * 本文件定义了智能填充服务（SmartFillService）及其相关的数据传输对象（FillSuggestion）。
 * 该服务属于服务层（Services），负责根据历史账单数据学习用户习惯，
 * 并为特定对手方提供智能填充建议，以提高记账效率。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef SERVICES_SMARTFILLSERVICE_H
#define SERVICES_SMARTFILLSERVICE_H

#include <QMap>
#include <QList>
#include <QString>
#include <QDate>
#include "base/base_operationresult.h"
#include "models/bill.h"

// ==========================================
// 填充建议实体 (DTO)
// ==========================================

/**
 * @brief 填充建议数据传输对象
 *
 * 该结构体封装了针对某个对手方的智能填充建议，包括建议的主体账户、分类以及置信度。
 * 它是 SmartFillService 的输出结果，用于向用户界面提供填充候选。
 */
struct FillSuggestion {
    quint16 counterpart = 0;         /**< 对手方ID（输入条件） */
    quint16 suggestedSubject = 0;    /**< 建议的主体账户ID（输出结果） */
    quint16 suggestedCategoryId = 0; /**< 建议的分类ID（输出结果） */
    int confidence = 0;              /**< 置信度，基于历史命中次数（频次） */

    /**
     * @brief 判断是否包含有效建议
     * @return true 如果主体账户和分类ID均不为0，表示有有效建议；否则返回 false。
     */
    bool hasSuggestion() const {
        return suggestedSubject != 0 && suggestedCategoryId != 0;
    }

    /**
     * @brief 获取置信度级别评估
     * @return 返回一个描述性字符串，如 "高频"、"中频"、"低频" 或 "首次"。
     */
    QString confidenceLevel() const {
        if (confidence > 10) return "高频";
        if (confidence > 5)  return "中频";
        if (confidence > 1)  return "低频";
        return "首次";
    }

    /**
     * @brief 空建议工厂方法
     * @return 返回一个所有字段均为默认值的空 FillSuggestion 实例。
     */
    static FillSuggestion empty() {
        return FillSuggestion{};
    }
};

// ==========================================
// 智能填充服务核心类
// ==========================================

/**
 * @brief 智能填充服务核心类
 *
 * 该类负责维护一个基于对手方的记忆库，通过分析历史账单数据，
 * 学习用户在不同对手方下的记账习惯（常用主体账户和分类）。
 * 当用户再次与同一对手方交易时，服务可以自动提供填充建议。
 * 该类使用 LRU（最近最少使用）策略来管理记忆库的大小，防止内存无限增长。
 */
class SmartFillService {
public:
    /**
     * @brief 构造函数
     * @param maxMemoryItems 最大记忆条目数量，默认为150。当记忆条目超过此数量时，将淘汰最不常用的条目。
     */
    explicit SmartFillService(int maxMemoryItems = 150);

    /**
     * @brief 根据对手方ID获取填充建议
     *
     * 核心查询方法。根据传入的对手方ID在记忆库中查找历史记录，
     * 如果找到，则根据历史频数统计生成一个填充建议。
     *
     * @param counterpartId 对手方ID
     * @return OperationResult<FillSuggestion> 包含填充建议的操作结果。
     *         如果未找到相关记忆，则返回一个空的 FillSuggestion。
     * @note 根据项目规范，入参 counterpartId 应通过 Validator 进行校验。
     */
    OperationResult<FillSuggestion> getSuggestion(quint16 counterpartId);

    /**
     * @brief 批量学习历史账单，从中提取频数统计
     *
     * 遍历传入的账单列表，对每个账单的对手方进行记忆学习。
     * 学习过程包括：获取或创建该对手方的记忆条目，然后更新该条目中
     * 主体账户和分类的频数统计。
     *
     * @param events 一组历史账单对象
     * @return OperationResult<void> 表示操作成功或失败的操作结果。
     * @note 根据项目规范，入参 events 及其内部元素应通过 Validator 进行校验。
     * @note 未处理 Bill 中 counterpartId 为0（未知对手方）的情况，调用方应确保数据有效性。
     */
    OperationResult<void> learnFromEventsBatch(const QList<Bill>& events);

    /**
     * @brief 清空所有记忆数据
     */
    void clear();

    /**
     * @brief 获取当前记忆条目数量
     * @return int 当前记忆库中的条目数。
     */
    int memoryCount() const;

private:
    /**
     * @brief 内部记忆结构体
     *
     * 用于存储针对一个特定对手方的历史统计信息。
     * 包括该对手方使用过的所有主体账户和分类及其频次，以及创建日期和使用次数。
     */
    struct FieldMemory {
        quint16 fieldValue = 0;                     /**< 对手方ID */
        QMap<quint16, int> subjectFrequency;        /**< 主体账户频数统计，Key: 主体账户ID, Value: 使用次数 */
        QMap<quint16, int> categoryFrequency;       /**< 分类频数统计，Key: 分类ID, Value: 使用次数 */
        QDate createDate;                           /**< 记忆条目的创建日期 */
        int usageCount = 0;                         /**< 该记忆条目被命中的总次数，用于LRU淘汰 */
    };

    QMap<quint16, FieldMemory> m_memoryDict; /**< 记忆字典，Key: 对手方ID, Value: 对应的记忆结构体 */
    int m_maxMemoryItems;                    /**< 最大记忆条目数，用于控制内存并触发LRU淘汰 */

    /**
     * @brief 获取或创建记忆条目（包含 LRU 淘汰逻辑）
     *
     * 根据对手方ID查找记忆库。如果存在，则增加其使用次数并返回引用。
     * 如果不存在，则创建一个新的记忆条目。在创建新条目前，会检查当前条目数
     * 是否已达到上限，如果是，则淘汰使用次数最少的条目。
     *
     * @param counterpart 对手方ID
     * @param date 当前日期，用于新条目的创建日期
     * @return FieldMemory& 返回对应对手方的记忆结构体引用
     */
    FieldMemory& getOrCreateMemory(quint16 counterpart, const QDate& date);

    /**
     * @brief 从记忆结构体中计算并生成填充建议
     *
     * 分析给定的记忆结构体，从其 subjectFrequency 和 categoryFrequency 中
     * 找出频次最高的主体账户和分类，并以此生成一个 FillSuggestion 对象。
     * 置信度设置为该最高频次。
     *
     * @param memory 一个有效的记忆结构体引用
     * @return FillSuggestion 生成的填充建议
     */
    FillSuggestion createSuggestionFromMemory(const FieldMemory& memory) const;
};

#endif // SERVICES_SMARTFILLSERVICE_H