/**
 * @file categorymanager.cpp
 * @brief 分类管理器实现文件
 *
 * 本文件属于 Models 层，负责管理分类数据的构建过程。
 * 支持添加一级/二级支出/收入分类，并返回构建结果。
 * 依赖 CategoryID、CategoryItem、SubCategoryDef 等类型。
 *
 * @author tys
 * @date 2026-06-08
 */

#include "models/categorymanager.h"

/**
 * @brief 构造函数，初始化成功标志为 true
 */
CategoryManager::CategoryManager() 
    : m_isSuccess(true) 
{
}

/**
 * @brief 获取构建过程是否成功
 *
 * @return true 表示所有操作均成功，false 表示至少有一个操作失败
 */
bool CategoryManager::isSuccess() const {
    return m_isSuccess;
}

/**
 * @brief 添加一级支出分类
 *
 * 校验分类数量不超过容量限制，并为每个分类名称创建 CategoryItem。
 * 注意：当前未使用 Validator 进行参数校验，违反项目规范。
 *
 * @param categories 一级支出分类名称列表
 */
void CategoryManager::addOutflowPrimaryCategories(const QStringList& categories) {
    // 数量校验逻辑：检查是否超过一级分类容量
    m_isSuccess &= (categories.size() <= CategoryID::PrimaryCategoryCapacity);
    
    for (int i = 0; i < categories.size(); ++i) {
        m_result.append(CategoryItem(
            CategoryID(false, i),   // false 表示支出类型，i 为一级索引
            categories[i]
        ));
    }
}

/**
 * @brief 添加一级收入分类
 *
 * 校验分类数量不超过容量限制，并为每个分类名称创建 CategoryItem。
 * 注意：当前未使用 Validator 进行参数校验，违反项目规范。
 *
 * @param categories 一级收入分类名称列表
 */
void CategoryManager::addInflowPrimaryCategories(const QStringList& categories) {
    // 数量校验逻辑：检查是否超过一级分类容量
    m_isSuccess &= (categories.size() <= CategoryID::PrimaryCategoryCapacity);
    
    for (int i = 0; i < categories.size(); ++i) {
        m_result.append(CategoryItem(
            CategoryID(true, i),    // true 表示收入类型，i 为一级索引
            categories[i]
        ));
    }
}

/**
 * @brief 添加二级支出分类
 *
 * 校验分类数量不超过容量限制，并为每个子分类定义创建 CategoryItem。
 * 注意：当前未使用 Validator 进行参数校验，违反项目规范。
 *
 * @param primaryIndex 所属一级分类的索引
 * @param categories 二级支出子分类定义列表
 */
void CategoryManager::addOutflowSecondaryCategories(int primaryIndex, const QList<SubCategoryDef>& categories) {
    // 数量校验逻辑：检查是否超过二级分类容量
    m_isSuccess &= (categories.size() <= CategoryID::SecondaryCategoryCapacity);
    
    for (int i = 0; i < categories.size(); ++i) {
        m_result.append(CategoryItem(
            CategoryID(false, primaryIndex, !categories[i].flag, i + 1),  // false 表示支出类型，子分类标志取反
            categories[i].name
        ));
    }
}

/**
 * @brief 添加二级收入分类
 *
 * 校验分类数量不超过容量限制，并为每个子分类定义创建 CategoryItem。
 * 注意：当前未使用 Validator 进行参数校验，违反项目规范。
 * 注意：数量校验使用 < 而非 <=，与其他函数不一致，可能为笔误。
 *
 * @param primaryIndex 所属一级分类的索引
 * @param categories 二级收入子分类定义列表
 */
void CategoryManager::addInflowSecondaryCategories(int primaryIndex, const QList<SubCategoryDef>& categories) {
    // 数量校验逻辑：检查是否超过二级分类容量（注意：此处使用 < 而非 <=）
    m_isSuccess &= (categories.size() < CategoryID::SecondaryCategoryCapacity);
    
    for (int i = 0; i < categories.size(); ++i) {
        m_result.append(CategoryItem(
            CategoryID(true, primaryIndex, !categories[i].flag, i + 1),   // true 表示收入类型，子分类标志取反
            categories[i].name
        ));
    }
}

/**
 * @brief 获取构建的分类项列表
 *
 * @return QList<CategoryItem> 包含所有已添加的分类项
 */
QList<CategoryItem> CategoryManager::getResult() const {
    return m_result;
}