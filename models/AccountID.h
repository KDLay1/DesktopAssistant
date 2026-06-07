/**
 * @file AccountID.h
 * @brief 账户ID强类型封装
 *
 * 所属分层：Models（模型层）
 * 功能描述：提供账户ID的强类型封装，区分内部账户（0xFF00-0xFFFF）和外部账户（0x0000-0xFEFF）
 *           内部账户用于系统内置账户（如现金、银行等），外部账户用于用户自定义账户
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_ACCOUNTID_H
#define MODELS_ACCOUNTID_H

#include <QtGlobal> // 使用 quint16 类型

/**
 * @brief 账户ID强类型类
 *
 * 设计目的：防止账户ID被随意赋值或混淆，提供类型安全的账户标识
 * 职责：封装 quint16 类型的账户ID，提供内部/外部账户判断、比较操作
 *
 * 取值范围：
 * - 外部账户：0x0000 - 0xFEFF（共 0xFF00 个）
 * - 内部账户：0xFF00 - 0xFFFF（共 256 个）
 */
class AccountID {
public:
    /** @name 外部账户范围常量 */
    ///@{
    static constexpr quint16 MaxOutsideAccountId = 0xFEFF;  ///< 外部账户ID最大值
    static constexpr quint16 MinOutsideAccountId = 0x0000;  ///< 外部账户ID最小值
    static constexpr int OutsideAccountIdCapacity = 0xFF00; ///< 外部账户ID容量（0x0000-0xFEFF 共 0xFF00 个）
    ///@}

    /** @name 内部账户范围常量 */
    ///@{
    static constexpr quint16 MaxInsideAccountId = 0xFFFF;   ///< 内部账户ID最大值
    static constexpr quint16 MinInsideAccountId = 0xFF00;   ///< 内部账户ID最小值
    static constexpr int InsideAccountIdCapacity = 0x100;   ///< 内部账户ID容量（0xFF00-0xFFFF 共 256 个）
    ///@}

    /**
     * @brief 构造函数
     * @param id 账户ID值，默认为0（外部账户）
     *
     * @note 不进行参数校验，由调用方确保传入合法ID值
     */
    AccountID(quint16 id = 0) : m_id(id) {}

    /**
     * @brief 隐式转换为 quint16
     * @return 原始账户ID值
     *
     * @note 允许隐式转换以便与原始类型兼容，但需注意类型安全
     */
    operator quint16() const { return m_id; }

    /**
     * @brief 相等比较运算符
     * @param other 另一个账户ID
     * @return true 如果两个ID相等，否则 false
     */
    bool operator==(const AccountID& other) const { return m_id == other.m_id; }

    /**
     * @brief 不等比较运算符
     * @param other 另一个账户ID
     * @return true 如果两个ID不相等，否则 false
     */
    bool operator!=(const AccountID& other) const { return m_id != other.m_id; }

    /** @name 静态判断方法（基于原始值） */
    ///@{
    /**
     * @brief 判断是否为内部账户格式
     * @param accountId 原始账户ID值
     * @return true 如果 accountId >= 0xFF00，否则 false
     */
    static bool isInsideFormat(quint16 accountId) { return accountId >= 0xFF00; }

    /**
     * @brief 判断是否为外部账户格式
     * @param accountId 原始账户ID值
     * @return true 如果 accountId <= 0xFEFF，否则 false
     */
    static bool isOutsideFormat(quint16 accountId) { return accountId <= 0xFEFF; }
    ///@}

    /** @name 实例判断方法（基于当前对象） */
    ///@{
    /**
     * @brief 判断当前账户是否为内部账户
     * @return true 如果 m_id >= 0xFF00，否则 false
     */
    bool isInside() const { return m_id >= 0xFF00; }

    /**
     * @brief 判断当前账户是否为外部账户
     * @return true 如果 m_id <= 0xFEFF，否则 false
     */
    bool isOutside() const { return m_id <= 0xFEFF; }
    ///@}

private:
    quint16 m_id; ///< 账户ID原始值，取值范围 0x0000-0xFFFF
};

#endif // MODELS_ACCOUNTID_H