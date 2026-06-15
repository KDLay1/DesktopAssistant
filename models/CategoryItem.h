/**
 * @file CategoryItem.h
 * @brief 分类条目数据模型（Models 层）
 *
 * 本文件定义了 CategoryItem 类，用于表示记账系统中的分类条目。
 * 作为值对象（Value Object），提供分类的唯一标识符和名称。
 *
 * @author tys
 * @date 2026-06-08
 *
 * @note 分层位置：Models 层
 * @note 依赖关系：仅依赖 QtBase（QString, QtGlobal）
 * @note 被依赖关系：Data 层（数据访问）、Services 层（业务逻辑）
 */

#ifndef MODELS_CATEGORYITEM_H
#define MODELS_CATEGORYITEM_H

#include <QString>
#include <QtGlobal>

/**
 * @brief 分类条目类
 *
 * 表示一个记账分类，包含唯一标识符和名称。
 * 作为不可变值对象使用，提供相等性比较操作。
 *
 * @note 分类ID使用 quint16 类型，范围 0-65535
 * @note 分类名称使用 QString，支持 Unicode 字符
 */
class CategoryItem {
public:
    /**
     * @brief 构造函数
     *
     * 创建一个分类条目对象，提供默认值。
     * 当 categoryId 为 0 时，表示“未分类”或“默认分类”。
     *
     * @param categoryId 分类唯一标识符（默认值 0）
     * @param categoryName 分类名称（默认值 空字符串）
     *
     * @note 未对参数进行校验，调用方需确保 categoryId 在有效范围内
     * @note 若需严格校验，建议在 Data 层或 Service 层使用 Validator
     */
    CategoryItem(quint16 categoryId = 0, const QString& categoryName = QString())
        : m_categoryId(categoryId), m_categoryName(categoryName) {}

    /**
     * @brief 获取分类ID
     *
     * @return quint16 分类的唯一标识符
     */
    quint16 categoryId() const { return m_categoryId; }

    /**
     * @brief 获取分类名称
     *
     * @return QString 分类的名称
     */
    QString categoryName() const { return m_categoryName; }

    /**
     * @brief 相等性比较运算符
     *
     * 比较两个分类条目是否完全相同（ID 和名称都相等）。
     *
     * @param other 待比较的另一个分类条目
     * @return bool 相等返回 true，否则返回 false
     *
     * @note 这是 Record 默认的字段比对逻辑
     */
    bool operator==(const CategoryItem& other) const {
        return m_categoryId == other.m_categoryId && m_categoryName == other.m_categoryName;
    }

    /**
     * @brief 不等性比较运算符
     *
     * 基于 operator== 的结果取反。
     *
     * @param other 待比较的另一个分类条目
     * @return bool 不相等返回 true，否则返回 false
     */
    bool operator!=(const CategoryItem& other) const {
        return !(*this == other);
    }

private:
    quint16 m_categoryId;   /**< 分类唯一标识符，0 表示未分类 */
    QString m_categoryName; /**< 分类名称，支持 Unicode 字符 */
};

#endif // MODELS_CATEGORYITEM_H