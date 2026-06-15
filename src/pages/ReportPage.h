/**
 * @file ReportPage.h
 * @brief 报表页面头文件
 *
 * 所属分层：Pages（界面层）
 * 功能：提供报表查询与展示的用户界面，包括日期范围、科目、类别筛选，
 *       以及每日收支表和账户快照表的展示。
 * 依赖关系：依赖 Models 层（bill.h），使用 Qt Widgets 框架。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>
#include "models/bill.h" // 引入账单模型，用于数据展示

// 前置声明，减少头文件依赖
class QDateEdit;
class QComboBox;
class QPushButton;
class QTabWidget;
class QTableView;
class QStandardItemModel;
class QLabel;

/**
 * @brief 报表页面主窗口部件
 *
 * 设计目的：为用户提供一个集中查询和浏览各类财务报表的界面。
 * 职责：
 * 1. 提供日期范围、科目、类别等查询条件的输入控件。
 * 2. 提供“查询”按钮，触发报表数据加载。
 * 3. 使用 QTabWidget 组织不同的报表视图（如每日收支、账户快照）。
 * 4. 管理表格视图及其背后的数据模型。
 */
class ReportPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件，默认为 nullptr
     *
     * 初始化页面，调用 setupUI() 创建界面，调用 loadDictData() 加载下拉框数据。
     */
    explicit ReportPage(QWidget *parent = nullptr);

public slots:
    /**
     * @brief 执行查询操作的私有槽函数
     *
     * 当用户点击“查询”按钮时触发。
     * 内部逻辑：
     * 1. 从 UI 控件中获取用户输入的查询条件（日期、科目、类别）。
     * 2. 调用 Services 层接口获取报表数据。
     * 3. 将获取到的数据填充到 modelDaily 和 modelSnapshot 中，更新表格视图。
     */
    void executeQuery();

private:
    /**
     * @brief 初始化并布局所有 UI 控件
     *
     * 创建日期选择器、下拉框、按钮、标签页、表格视图等控件，
     * 并设置它们的布局和初始状态。
     */
    void setupUI();

    /**
     * @brief 加载下拉框的字典数据
     *
     * 从 Services 或 Data 层获取科目和类别的字典数据，
     * 并填充到 comboSubject 和 comboCategory 下拉框中。
     */
    void loadDictData();

    // --- UI 控件成员 ---
    QDateEdit *dateStart;          /**< 起始日期选择器 */
    QDateEdit *dateEnd;            /**< 结束日期选择器 */
    QComboBox *comboSubject;       /**< 科目筛选下拉框 */
    QComboBox *comboCategory;      /**< 类别筛选下拉框 */
    QPushButton *btnQuery;         /**< 查询按钮 */

    QTabWidget *tabWidget;         /**< 报表标签页容器 */
    QTableView *tableDailySummary; /**< 每日收支表视图 */
    QTableView *tableSnapshot;     /**< 账户快照表视图 */

    QStandardItemModel *modelDaily;    /**< 每日收支表的数据模型 */
    QStandardItemModel *modelSnapshot; /**< 账户快照表的数据模型 */
};

#endif // REPORTPAGE_H