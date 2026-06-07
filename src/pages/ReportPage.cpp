/**
 * @file ReportPage.cpp
 * @brief 报表页面实现文件
 * 
 * 所属分层：Pages（界面层）
 * 功能：提供高级查询功能，展示每日流水收支和账户状态快照两种报表
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "ReportPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>

#include "services/billmanager.h"
#include "services/calculateservice.h"
#include "data/DatabaseManager.h"

/**
 * @brief 构造函数
 * 
 * 初始化报表页面，创建 UI 布局并加载字典数据
 * 
 * @param parent 父窗口指针，用于 Qt 对象树管理
 */
ReportPage::ReportPage(QWidget *parent) : QWidget(parent)
{
    setupUI();      // 创建并配置 UI 组件
    loadDictData(); // 加载下拉框的字典数据
}

/**
 * @brief 设置 UI 布局
 * 
 * 创建高级查询面板和报表展示区域（Tab 控件）
 * 查询面板包含日期范围选择、账户/分类筛选和查询按钮
 * 报表展示区包含两个表格视图：每日流水收支和账户状态快照
 */
void ReportPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ==========================================
    // 1. 高级查询面板
    // ==========================================
    QGroupBox *groupBox = new QGroupBox("🔍 高级查询条件", this);
    groupBox->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #CCC; border-radius: 5px; margin-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; }");
    
    QHBoxLayout *queryLayout = new QHBoxLayout(groupBox);
    
    // 日期范围选择表单
    QFormLayout *form1 = new QFormLayout();
    dateStart = new QDateEdit(QDate::currentDate().addDays(-30), this); // 默认起始日期为30天前
    dateEnd = new QDateEdit(QDate::currentDate(), this);                // 默认结束日期为今天
    dateStart->setCalendarPopup(true);  // 启用日历弹出选择
    dateEnd->setCalendarPopup(true);    // 启用日历弹出选择
    form1->addRow("起始日期:", dateStart);
    form1->addRow("终止日期:", dateEnd);

    // 账户和分类筛选表单
    QFormLayout *form2 = new QFormLayout();
    comboSubject = new QComboBox(this);  // 账户选择下拉框
    comboCategory = new QComboBox(this); // 分类选择下拉框
    form2->addRow("限定账户:", comboSubject);
    form2->addRow("限定分类:", comboCategory);

    // 查询按钮
    btnQuery = new QPushButton("⚡ 执行查询", this);
    btnQuery->setStyleSheet("background-color: #27AE60; color: white; font-weight: bold; padding: 10px; border-radius: 5px;");
    
    queryLayout->addLayout(form1);
    queryLayout->addLayout(form2);
    queryLayout->addWidget(btnQuery);

    // ==========================================
    // 2. 报表展示区 (Tabs)
    // ==========================================
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("QTabBar::tab { padding: 8px 20px; font-weight: bold; }");

    // 表1：每日流水收支
    tableDailySummary = new QTableView(this);
    modelDaily = new QStandardItemModel(0, 4, this); // 4列：日期、总收入、总支出、净流水
    modelDaily->setHorizontalHeaderLabels({"日期", "当日总收入", "当日总支出", "当日净流水"});
    tableDailySummary->setModel(modelDaily);
    tableDailySummary->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
    tabWidget->addTab(tableDailySummary, "📅 每日流水收支");

    // 表2：账户分期状态 (快照)
    tableSnapshot = new QTableView(this);
    modelSnapshot = new QStandardItemModel(0, 3, this); // 3列：日期、变动账户、变动后余额
    modelSnapshot->setHorizontalHeaderLabels({"日期", "变动账户", "变动后余额"});
    tableSnapshot->setModel(modelSnapshot);
    tableSnapshot->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
    tabWidget->addTab(tableSnapshot, "💳 账户状态快照");

    // 统一样式：启用交替行颜色
    tableDailySummary->setAlternatingRowColors(true);
    tableSnapshot->setAlternatingRowColors(true);

    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(tabWidget);

    // 连接查询按钮的点击信号到执行查询槽函数
    connect(btnQuery, &QPushButton::clicked, this, &ReportPage::executeQuery);
}

/**
 * @brief 加载字典数据到下拉框
 * 
 * 从数据库加载账户和分类字典数据，填充到对应的 QComboBox
 * 每个下拉框的第一个选项为"全部"（值为0）
 */
void ReportPage::loadDictData()
{
    // 加载账户字典
    comboSubject->addItem("全部账户", 0); // 添加"全部"选项，值为0表示不筛选
    auto subjects = DatabaseManager::loadDict("dict_subject");
    for (auto it = subjects.begin(); it != subjects.end(); ++it) {
        comboSubject->addItem(it.value(), it.key()); // 添加具体账户选项
    }

    // 加载分类字典
    comboCategory->addItem("全部分类", 0); // 添加"全部"选项，值为0表示不筛选
    auto categories = DatabaseManager::loadDict("dict_category");
    for (auto it = categories.begin(); it != categories.end(); ++it) {
        comboCategory->addItem(it.value(), it.key()); // 添加具体分类选项
    }
}

/**
 * @brief 执行查询并更新报表数据
 * 
 * 根据用户选择的查询条件（日期范围、账户、分类）过滤账单数据，
 * 然后调用 CalculateService 生成两种报表：
 * 1. 每日流水收支：按日期汇总收入和支出
 * 2. 账户状态快照：记录每次交易后的账户余额变化
 * 
 * @note 如果查询失败，直接返回不更新报表
 */
void ReportPage::executeQuery()
{
    // 1. 获取全量数据并进行条件过滤
    auto result = BillManager::getAllBills();
    if (!result.isSuccess()) return; // 获取数据失败，直接返回
    
    QList<Bill> allBills = result.data();
    QList<Bill> filteredBills;

    // 获取查询条件
    QDate startD = dateStart->date();   // 起始日期
    QDate endD = dateEnd->date();       // 终止日期
    quint16 targetSub = comboSubject->currentData().toInt(); // 目标账户ID，0表示全部
    quint16 targetCat = comboCategory->currentData().toInt(); // 目标分类ID，0表示全部

    // 遍历所有账单，根据条件过滤
    for (const Bill& b : std::as_const(allBills)) {
        QDate bDate = QDate::fromJulianDay(b.dayNumber()); // 将儒略日转换为日期
        if (bDate < startD || bDate > endD) continue; // 时间过滤
        if (targetSub != 0 && b.subjectId() != targetSub) continue; // 账户过滤
        if (targetCat != 0 && b.categoryId() != targetCat) continue; // 分类过滤
        
        filteredBills.append(b); // 符合条件的账单加入过滤列表
    }

    // ==========================================
    // 引擎 1：生成【每日流水收支】
    // 调用底层 CalculateService::summaryPerDay
    // ==========================================
    modelDaily->removeRows(0, modelDaily->rowCount()); // 清空现有数据
    auto dailyResult = CalculateService::summaryPerDay(startD, endD, filteredBills);
    if (dailyResult.isSuccess()) {
        auto list = dailyResult.data();
        for (const auto& item : list) {
            // 如果某天没有流水，跳过展示
            if (item.inflow() == 0 && item.outflow() == 0) continue;

            QList<QStandardItem*> row;
            row << new QStandardItem(item.start().toString("yyyy-MM-dd")); // 日期
            row << new QStandardItem(QString::number(item.inflow()/100.0, 'f', 2));   // 总收入
            row << new QStandardItem(QString::number(item.outflow()/100.0, 'f', 2));  // 总支出
            row << new QStandardItem(QString::number(item.netFlow()/100.0, 'f', 2));  // 净流水
            for(auto i : row) i->setTextAlignment(Qt::AlignCenter); // 居中对齐
            modelDaily->appendRow(row); // 添加到表格模型
        }
    }

    // ==========================================
    // 引擎 2：生成【账户状态快照】
    // 调用底层 CalculateService::scanPerDay
    // ==========================================
    modelSnapshot->removeRows(0, modelSnapshot->rowCount()); // 清空现有数据
    SnapshotItem initSnap; // 假设期初余额为 0
    auto snapResult = CalculateService::scanPerDay(initSnap, filteredBills);
    auto subDict = DatabaseManager::loadDict("dict_subject"); // 加载账户字典用于显示账户名称

    if (snapResult.isSuccess()) {
        auto list = snapResult.data();
        for (const auto& item : list) {
            QDate sDate = QDate::fromJulianDay(item.dayNumber); // 将儒略日转换为日期
            
            // 展开 Map，展示每个变动的账户
            for (auto it = item.states.begin(); it != item.states.end(); ++it) {
                QList<QStandardItem*> row;
                row << new QStandardItem(sDate.toString("yyyy-MM-dd")); // 日期
                row << new QStandardItem(subDict.value(it.key(), "未知账户")); // 账户名称
                row << new QStandardItem(QString::number(it.value().balance().view(), 'f', 2)); // 变动后余额
                for(auto i : row) i->setTextAlignment(Qt::AlignCenter); // 居中对齐
                modelSnapshot->appendRow(row); // 添加到表格模型
            }
        }
    }
}