/**
 * @file InsideAccountItem.h
 * @brief 内部账户数据模型定义
 *
 * 所属分层：Models（数据模型层）
 * 功能：表示内部账户的完整数据，继承自 AccountItem，扩展账户状态字段
 * 依赖：models/AccountItem.h（基类）、models/AccountStatusOptions.h（状态枚举）
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_INSIDEACCOUNTITEM_H
#define MODELS_INSIDEACCOUNTITEM_H

#include "models/AccountItem.h"
#include "models/AccountStatusOptions.h"
#include <QString>
#include <QtGlobal>

/**
 * @brief 内部账户数据模型类
 *
 * 继承自基类 AccountItem，用于表示内部账户的完整数据。
 * 在基类（账户ID、名称）基础上，增加了账户状态字段。
 * 提供完整的比较运算符，支持 Record 继承体系下的值比较。
 */
class InsideAccountItem : public AccountItem {
public:
    /**
     * @brief 构造函数，带有默认参数
     *
     * 初始化内部账户的所有字段，包括基类字段和派生类字段。
     * 基类构造函数负责初始化账户ID和名称，本构造函数初始化账户状态。
     *
     * @param accountId 账户ID（默认0，表示未指定）
     * @param accountName 账户名称（默认空字符串）
     * @param status 账户状态（默认 Active）
     */
    InsideAccountItem(quint16 accountId = 0, 
                      const QString& accountName = QString(), 
                      AccountStatusOptions status = AccountStatusOptions::Active)
        : AccountItem(accountId, accountName), m_status(status) {}

    /**
     * @brief 获取账户状态
     *
     * 返回当前内部账户的状态枚举值。
     *
     * @return AccountStatusOptions 当前账户状态
     */
    AccountStatusOptions status() const { return m_status; }

    /**
     * @brief 相等比较运算符
     *
     * 先调用基类 AccountItem 的 operator== 比较基类字段（账户ID、名称），
     * 再比较派生类字段（账户状态）。所有字段相等时返回 true。
     *
     * @param other 待比较的另一个 InsideAccountItem 对象
     * @return bool 相等返回 true，否则返回 false
     */
    bool operator==(const InsideAccountItem& other) const {
        // 先调用基类 AccountItem 的 operator==，再比较派生类字段
        return AccountItem::operator==(other) && m_status == other.m_status;
    }

    /**
     * @brief 不等比较运算符
     *
     * 基于 operator== 的结果取反实现。
     *
     * @param other 待比较的另一个 InsideAccountItem 对象
     * @return bool 不相等返回 true，否则返回 false
     */
    bool operator!=(const InsideAccountItem& other) const {
        return !(*this == other);
    }

private:
    AccountStatusOptions m_status; /**< 账户状态（Active/Inactive 等） */
};

#endif // MODELS_INSIDEACCOUNTITEM_H