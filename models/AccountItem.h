/**
 * @file AccountItem.h
 * @brief 账户数据模型定义
 *
 * 所属分层：Models（数据模型层）
 * 功能描述：定义账户的基本信息结构，用于在系统各层间传递账户数据
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_ACCOUNTITEM_H
#define MODELS_ACCOUNTITEM_H

#include <QString>
#include <QtGlobal>

/**
 * @brief 账户数据模型类
 *
 * 表示一个账户的基本信息，包含账户ID和账户名称。
 * 作为纯数据对象（POCO），不包含业务逻辑，仅提供数据存储和访问功能。
 * 该对象在 Models 层定义，可被 Data、Services、Pages 层使用。
 */
class AccountItem {
public:
    /**
     * @brief 构造函数，提供默认值
     *
     * 创建一个账户项实例，支持默认参数以方便创建空对象。
     * 注意：账户ID使用 quint16 类型，但项目规范要求使用强类型 AccountID，
     * 后续版本应迁移至强类型以增强类型安全。
     *
     * @param accountId 账户ID，默认值为0（表示无效ID）
     * @param accountName 账户名称，默认值为空字符串
     */
    AccountItem(quint16 accountId = 0, const QString& accountName = QString())
        : m_accountId(accountId), m_accountName(accountName) {}

    /**
     * @brief 获取账户ID
     *
     * @return quint16 当前账户的唯一标识符
     */
    quint16 accountId() const { return m_accountId; }

    /**
     * @brief 获取账户名称
     *
     * @return QString 当前账户的名称
     */
    QString accountName() const { return m_accountName; }

    /**
     * @brief 相等比较运算符
     *
     * 比较两个账户项的所有字段是否完全相等。
     * 用于集合查找、去重等场景。
     *
     * @param other 待比较的另一个账户项
     * @return bool 如果所有字段相等返回true，否则返回false
     */
    bool operator==(const AccountItem& other) const {
        return m_accountId == other.m_accountId && m_accountName == other.m_accountName;
    }

    /**
     * @brief 不等比较运算符
     *
     * 基于 operator== 实现，取反返回结果。
     *
     * @param other 待比较的另一个账户项
     * @return bool 如果任意字段不相等返回true，否则返回false
     */
    bool operator!=(const AccountItem& other) const {
        return !(*this == other);
    }

private:
    quint16 m_accountId;    /**< 账户ID，使用 quint16 存储（待迁移至强类型 AccountID） */
    QString m_accountName;  /**< 账户名称 */
};

#endif // MODELS_ACCOUNTITEM_H