/**
 * @file AccountBalance.h
 * @brief 账户余额数据模型
 *
 * 所属分层：Models（数据模型层）
 * 依赖关系：依赖 models/MoneyRecord.h（同层依赖）
 * 功能描述：封装账户余额数据，提供与 MoneyRecord 的互操作接口
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_ACCOUNTBALANCE_H
#define MODELS_ACCOUNTBALANCE_H

#include "models/MoneyRecord.h"

/**
 * @class AccountBalance
 * @brief 账户余额类
 *
 * 设计目的：以 MoneyRecord 为基础，提供账户余额的封装与比较功能。
 * 职责说明：
 * - 存储账户余额（以分为单位，通过 MoneyRecord 实现）
 * - 提供与 MoneyRecord 的构造转换
 * - 提供余额比较操作
 *
 * @note 金额存储遵循项目规范，内部使用 int（分）存储，禁止直接使用 double
 */
class AccountBalance {
public:
    /**
     * @brief 默认构造函数，使用 MoneyRecord 构造余额
     * @param balance MoneyRecord 对象，默认值为 0（即 0 分）
     *
     * 内部逻辑：直接初始化成员变量 m_balance
     */
    AccountBalance(MoneyRecord balance = MoneyRecord(0))
        : m_balance(balance) {}

    /**
     * @brief 基于 double 的构造函数
     * @param viewBalance 以元为单位的金额（double 类型）
     *
     * 内部逻辑：通过 MoneyRecord(double) 构造函数转换为以分为单位的存储
     * @note 此构造函数会调用 MoneyRecord 的内部校验逻辑
     */
    AccountBalance(double viewBalance)
        : m_balance(MoneyRecord(viewBalance)) {}

    /**
     * @brief 获取余额值
     * @return MoneyRecord 对象，表示当前余额
     *
     * 内部逻辑：返回 m_balance 的拷贝副本
     */
    MoneyRecord balance() const { return m_balance; }

    /**
     * @brief 相等比较运算符
     * @param other 另一个 AccountBalance 对象
     * @return bool 如果两个余额相等返回 true，否则返回 false
     *
     * 内部逻辑：比较两个对象的 m_balance 成员是否相等
     * 调用关系：调用 MoneyRecord::operator==
     */
    bool operator==(const AccountBalance& other) const {
        return m_balance == other.m_balance;
    }

    /**
     * @brief 不等比较运算符
     * @param other 另一个 AccountBalance 对象
     * @return bool 如果两个余额不相等返回 true，否则返回 false
     *
     * 内部逻辑：取反 operator== 的结果
     * 调用关系：调用 operator==
     */
    bool operator!=(const AccountBalance& other) const {
        return !(*this == other);
    }

private:
    MoneyRecord m_balance; /**< 余额存储，以分为单位 */
};

#endif // MODELS_ACCOUNTBALANCE_H