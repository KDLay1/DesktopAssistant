/**
 * @file categorymanager.h
 * @brief 分类管理器头文件
 *
 * 本文件定义了 SubCategoryDef 结构体和 CategoryManager 类，
 * 用于管理分类的批量添加操作，属于 Models 层。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef SERVICES_CATEGORYMANAGER_H
#define SERVICES_CATEGORYMANAGER_H

#include <QList>
#include <QStringList>
#include <QString>
#include "models/CategoryItem.h"
#include "models/CategoryID.h"

/**
 * @brief 二级分类定义结构体
 *
 * 用于定义二级分类的配置信息，包含名称和标志位。
 * 标志位的含义根据分类类型（支出/收入）而不同。
 */
struct SubCategoryDef {
    QString name; ///< 分类名称
    bool flag;    ///< 标志位：对于支出代表 isExtend（是否可扩展），对于收入代表 isIncome（是否为收入类型）
};

/**
 * @brief 分类管理器类
 *
 * 负责管理分类的批量添加操作，包括一级分类和二级分类的添加，
 * 并生成最终的分类列表。该类属于 Models 层，供上层服务调用。
 *
 * @note 当前实现未使用 Validator 进行参数校验，也未使用 OperationResult 作为返回值，
 *       后续版本需要按照项目规范进行重构。
 */
class CategoryManager {
public:
    /**
     * @brief 构造函数
     *
     * 初始化分类管理器，设置初始状态为失败。
     */
    CategoryManager();

    /**
     * @brief 获取操作是否成功
     *
     * @return true 操作成功，false 操作失败
     */
    bool isSuccess() const;

    /**
     * @brief 批量添加支出一级分类
     *
     * 将传入的分类名称列表作为支出一级分类添加到管理器中。
     *
     * @param categories 支出一级分类名称列表
     *
     * @note 当前实现未对参数进行有效性校验，也未处理重复分类的情况。
     *       后续版本应使用 Validator 进行参数校验。
     */
    void addOutflowPrimaryCategories(const QStringList& categories);

    /**
     * @brief 批量添加收入一级分类
     *
     * 将传入的分类名称列表作为收入一级分类添加到管理器中。
     *
     * @param categories 收入一级分类名称列表
     *
     * @note 当前实现未对参数进行有效性校验，也未处理重复分类的情况。
     *       后续版本应使用 Validator 进行参数校验。
     */
    void addInflowPrimaryCategories(const QStringList& categories);

    /**
     * @brief 批量添加支出二级分类
     *
     * 根据一级分类的索引，将传入的二级分类定义列表添加到对应的支出一级分类下。
     *
     * @param primaryIndex 一级分类的索引（从0开始）
     * @param categories 二级分类定义列表
     *
     * @note 当前实现未校验索引是否越界，也未校验 SubCategoryDef 的有效性。
     *       后续版本应使用强类型 CategoryID 替代 int 索引。
     */
    void addOutflowSecondaryCategories(int primaryIndex, const QList<SubCategoryDef>& categories);

    /**
     * @brief 批量添加收入二级分类
     *
     * 根据一级分类的索引，将传入的二级分类定义列表添加到对应的收入一级分类下。
     *
     * @param primaryIndex 一级分类的索引（从0开始）
     * @param categories 二级分类定义列表
     *
     * @note 当前实现未校验索引是否越界，也未校验 SubCategoryDef 的有效性。
     *       后续版本应使用强类型 CategoryID 替代 int 索引。
     */
    void addInflowSecondaryCategories(int primaryIndex, const QList<SubCategoryDef>& categories);

    /**
     * @brief 获取生成的分类列表
     *
     * 返回经过所有添加操作后生成的完整分类列表。
     *
     * @return QList<CategoryItem> 分类列表
     */
    QList<CategoryItem> getResult() const;

private:
    QList<CategoryItem> m_result; ///< 存储生成的分类列表
    bool m_isSuccess;             ///< 操作是否成功的标志
};

#endif // SERVICES_CATEGORYMANAGER_H