/**
 * @file bill.h
 * @brief 交易账单模型类
 *
 * 所属分层：Models（数据模型层）
 * 依赖关系：依赖 Qt 基础库、CategoryID、MoneyRecord
 * 被依赖：Data 层（数据访问）、Services 层（业务逻辑）
 *
 * 本文件定义了 Bill 类，用于表示一笔交易账单，
 * 包含交易的基本信息（日期、类型、金额、账户等）和业务规则判断方法。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_BILL_H
#define MODELS_BILL_H

#include <QString>
#include <QtGlobal>
#include "models/CategoryID.h"
#include "models/MoneyRecord.h"

/**
 * @brief 交易账单模型类
 *
 * 表示一笔交易账单，包含交易日期、类型、金额、账户等核心信息。
 * 提供静态和实例方法用于判断交易类型（出账、入账、支出、收入等）。
 * 金额以分为单位存储（通过 MoneyRecord 类型），避免浮点数精度问题。
 *
 * @note 所有金额操作均使用 MoneyRecord 类型，禁止直接使用 double
 * @note ID 类型（categoryId, subjectId, counterpartId）后续应改为强类型（如 CategoryID）
 */
class Bill {
public:
    /**
     * @brief 构造函数
     *
     * 初始化交易账单的所有字段，提供默认值以便创建空对象。
     *
     * @param dayNumber 交易日期编号（默认 0）
     * @param isRefund 是否为退/还款事件（默认 false）
     * @param categoryId 交易类型ID（默认 0，关联 CategoryID）
     * @param subjectId 主体账户ID（默认 0）
     * @param counterpartId 对手方账户ID（默认 0）
     * @param amount 交易金额（默认 MoneyRecord(0)，以分为单位）
     * @param remarks 交易附言（默认空字符串）
     * @param id 数据库自增主键（默认 -1，表示新记录）
     *
     * @note 调用方应在构造前使用 Validator 校验参数合法性
     */
    Bill(int dayNumber = 0,
         bool isRefund = false,
         quint16 categoryId = 0,
         quint16 subjectId = 0,
         quint16 counterpartId = 0,
         MoneyRecord amount = MoneyRecord(0),
         const QString& remarks = QString(),
         int id = -1);

    // ==========================================
    // 字段 Getters
    // ==========================================

    /** @brief 获取交易日期编号 */
    int dayNumber() const { return m_dayNumber; }

    /** @brief 获取是否为退/还款事件 */
    bool isRefund() const { return m_isRefund; }

    /** @brief 获取交易类型ID（关联 CategoryID） */
    quint16 categoryId() const { return m_categoryId; }

    /** @brief 获取主体账户ID */
    quint16 subjectId() const { return m_subjectId; }

    /** @brief 获取对手方账户ID */
    quint16 counterpartId() const { return m_counterpartId; }

    /** @brief 获取交易金额（以分为单位） */
    MoneyRecord amount() const { return m_amount; }

    /** @brief 获取交易附言 */
    QString remarks() const { return m_remarks; }

    /** @brief 获取数据库自增主键 */
    int id() const { return m_id; }

    /**
     * @brief 设置数据库自增主键
     *
     * @param id 主键值（通常由数据库生成后设置）
     * @note 此方法仅在数据持久化后调用，不应在业务逻辑中修改
     */
    void setId(int id) { m_id = id; }

    // ==========================================
    // 静态判断方法 (无状态依赖的通用规则判断)
    // ==========================================

    /**
     * @brief 判断是否为出账事件（静态方法）
     *
     * 基于交易类型和退款标志，判断交易是否为出账（资金流出）。
     *
     * @param isRefund 是否为退/还款事件
     * @param categoryId 交易类型ID
     * @return true 表示出账事件，false 表示非出账事件
     *
     * @note 此方法不依赖实例状态，可用于批量判断
     */
    static bool isOutflowEvent(bool isRefund, quint16 categoryId);

    /**
     * @brief 判断是否为入账事件（静态方法）
     *
     * 基于交易类型和退款标志，判断交易是否为入账（资金流入）。
     *
     * @param isRefund 是否为退/还款事件
     * @param categoryId 交易类型ID
     * @return true 表示入账事件，false 表示非入账事件
     *
     * @note 此方法不依赖实例状态，可用于批量判断
     */
    static bool isInflowEvent(bool isRefund, quint16 categoryId);

    /**
     * @brief 判断是否为支出事件（静态方法）
     *
     * 不考虑后续事件退款的情况，仅基于当前交易类型判断是否为支出。
     *
     * @param isRefund 是否为退/还款事件
     * @param categoryId 交易类型ID
     * @return true 表示支出事件，false 表示非支出事件
     *
     * @note 此方法不依赖实例状态，可用于批量判断
     */
    static bool isExtendEvent(bool isRefund, quint16 categoryId);

    /**
     * @brief 判断是否为收入事件（静态方法）
     *
     * 例如内部转入、退款不是收入事件。
     *
     * @param isRefund 是否为退/还款事件
     * @param categoryId 交易类型ID
     * @return true 表示收入事件，false 表示非收入事件
     *
     * @note 此方法不依赖实例状态，可用于批量判断
     */
    static bool isIncomeEvent(bool isRefund, quint16 categoryId);

    // ==========================================
    // 实例判断方法 (基于当前账单数据的判断)
    // ==========================================

    /**
     * @brief 判断是否为原支出事件的退款事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示是原支出事件的退款，false 表示不是
     */
    bool isExtendRefund() const;

    /**
     * @brief 判断是否为原仅出账事件的冲正或撤销事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示是冲正或撤销事件，false 表示不是
     */
    bool isOutflowReversal() const;

    /**
     * @brief 判断是否为出账事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示出账事件，false 表示非出账事件
     */
    bool isOutflow() const;

    /**
     * @brief 判断是否为入账事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示入账事件，false 表示非入账事件
     */
    bool isInflow() const;

    /**
     * @brief 判断是否为支出事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示支出事件，false 表示非支出事件
     */
    bool isExtend() const;

    /**
     * @brief 判断是否为收入事件
     *
     * 基于当前实例的 isRefund 和 categoryId 判断。
     *
     * @return true 表示收入事件，false 表示非收入事件
     */
    bool isIncome() const;

    // ==========================================
    // 操作符重载
    // ==========================================

    /**
     * @brief 相等比较运算符
     *
     * 比较两个 Bill 对象的所有字段是否相等。
     *
     * @param other 另一个 Bill 对象
     * @return true 表示两个对象完全相等，false 表示不相等
     */
    bool operator==(const Bill& other) const;

    /**
     * @brief 不等比较运算符
     *
     * 比较两个 Bill 对象是否不相等。
     *
     * @param other 另一个 Bill 对象
     * @return true 表示两个对象不相等，false 表示相等
     */
    bool operator!=(const Bill& other) const;

private:
    int m_dayNumber;           /**< 交易日期编号（如 20231001 表示 2023年10月1日） */
    bool m_isRefund;           /**< 是否为退/还款事件（true 表示退款，false 表示正常交易） */
    quint16 m_categoryId;      /**< 交易类型ID（关联 CategoryID 枚举） */
    quint16 m_subjectId;       /**< 主体账户ID（资金流出方或流入方） */
    quint16 m_counterpartId;   /**< 对手方账户ID（交易对方） */
    MoneyRecord m_amount;      /**< 交易金额（以分为单位，避免浮点数精度问题） */
    QString m_remarks;         /**< 交易附言（可选，用于备注交易信息） */
    int m_id;                  /**< 数据库自增主键（-1 表示新记录，未持久化） */
};

#endif // MODELS_BILL_H