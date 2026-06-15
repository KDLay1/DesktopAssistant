/**
 * @file FinancePage.h
 * @brief 财务管理页面头文件
 *
 * 所属分层：Pages（页面层）
 * 功能：提供账单的增删改查、导入导出、搜索过滤及智能填充功能
 * 依赖：Models 层（Bill）、Services 层（SmartFillService）
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef FINANCEPAGE_H
#define FINANCEPAGE_H

#include <QWidget>
#include <QList>
#include <memory> // 【关键补充】引入智能指针，用于管理 SmartFillService 生命周期
#include "models/bill.h"          // 账单数据模型
#include "services/smartfillservice.h" // 【关键补充】引入智能填充服务，用于学习和推荐

// 前向声明 Qt 控件类，减少头文件依赖
class QLabel;
class QPushButton;
class QLineEdit;
class QTableView;
class QStandardItemModel;
class QSortFilterProxyModel;

/**
 * @brief 财务管理页面类
 *
 * 设计目的：作为系统的主页面之一，提供完整的账单管理功能。
 * 职责：
 *   - 展示账单列表（通过 QTableView 和代理模型实现搜索过滤）
 *   - 提供添加、删除、导入、导出账单的操作入口
 *   - 集成智能填充服务，学习用户输入习惯并推荐
 *
 * @note 金额使用 int（分）存储，禁止使用 double
 * @note 所有业务操作应通过 Services 层完成，并返回 OperationResult
 */
class FinancePage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针，默认为 nullptr
     *
     * 初始化界面布局和信号连接
     */
    explicit FinancePage(QWidget *parent = nullptr);

private slots:
    /**
     * @brief 添加按钮点击槽函数
     *
     * 触发添加账单流程，可能调用 SmartFillService 进行智能推荐
     */
    void onAddButtonClicked();

    /**
     * @brief 删除按钮点击槽函数
     *
     * 删除当前选中的账单记录
     */
    void onDeleteButtonClicked();

    /**
     * @brief 导出按钮点击槽函数
     *
     * 将当前账单数据导出到文件（如 CSV 或 Excel）
     */
    void onExportButtonClicked();

    /**
     * @brief 导入按钮点击槽函数
     *
     * 从文件导入账单数据并刷新页面
     */
    void onImportButtonClicked();

    /**
     * @brief 搜索框文本变化槽函数
     * @param text 用户输入的搜索关键字
     *
     * 更新代理模型的过滤条件，实现实时搜索
     */
    void onSearchTextChanged(const QString &text);

private:
    /**
     * @brief 初始化界面布局和控件
     *
     * 创建所有 UI 控件（标签、按钮、输入框、表格等）并设置布局
     */
    void setupUI();

    /**
     * @brief 连接信号与槽
     *
     * 将按钮点击、搜索框变化等信号连接到对应的槽函数
     */
    void setupConnections();

    /**
     * @brief 刷新页面数据
     *
     * 从数据源重新加载账单数据到 m_sourceModel 和 m_currentBills
     */
    void refreshData();

    /**
     * @brief 【关键补充】更新智能填充服务的记忆
     *
     * 将当前账单数据传递给 m_fillService，用于学习用户输入习惯
     * 应在数据变更后调用（如添加、删除、导入后）
     */
    void updateSmartFillMemory();

    // --- UI 控件 ---
    QLabel *titleLabel;          ///< 页面标题标签
    QLabel *summaryLabel;        ///< 汇总信息标签（如总金额）
    QLabel *lblStatusDate;       ///< 状态栏日期标签
    QLabel *lblStatusTotal;      ///< 状态栏总金额标签
    QLineEdit *searchLineEdit;   ///< 搜索输入框

    QPushButton *addButton;      ///< 添加账单按钮
    QPushButton *deleteButton;   ///< 删除账单按钮
    QPushButton *exportButton;   ///< 导出账单按钮

    QTableView *recordTableView;             ///< 账单记录表格视图
    QStandardItemModel *m_sourceModel;       ///< 真实数据模型，存储所有账单数据
    QSortFilterProxyModel *m_proxyModel;     ///< 代理筛选模型，用于搜索过滤
    QPushButton *importButton;               ///< 导入账单按钮
    QList<Bill> m_currentBills;              ///< 当前页面加载的账单列表（可能已过滤）

    // 【关键补充】使用智能指针统一管理智能推荐服务，避免手动内存管理
    std::unique_ptr<SmartFillService> m_fillService; ///< 智能填充服务，用于学习和推荐
};

#endif // FINANCEPAGE_H