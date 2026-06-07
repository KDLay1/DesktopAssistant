/**
 * @file FinancePage.cpp
 * @brief 记账本主页面实现文件
 *
 * 所属分层：Pages（界面层）
 * 功能：提供记账本的主要用户界面，包括账单列表展示、搜索、添加、删除、导入/导出CSV等功能。
 *       依赖 Services 层进行业务逻辑处理，依赖 Data 层加载字典数据。
 *
 * @author tys
 * @date 2026-06-08
 */

#include "financepage.h"
#include "data/DatabaseManager.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <QDate>
#include <QRegularExpression>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "addbilldialog.h"
#include "services/billmanager.h"
#include "services/calculateservice.h"
#include "services/smartfillservice.h" // 【新增】包含 SmartFillService 头文件
#include "models/eventsummary.h"
#include "models/CategoryID.h"

/**
 * @brief 构造函数
 *
 * 初始化智能填充服务，并设置UI、连接信号槽、加载初始数据。
 * @param parent 父窗口部件
 */
FinancePage::FinancePage(QWidget *parent)
    : QWidget(parent), m_fillService(std::make_unique<SmartFillService>())
{
    setupUI();
    setupConnections();
    refreshData();
}

/**
 * @brief 设置用户界面
 *
 * 创建并布局所有UI控件，包括标题、汇总标签、搜索框、操作按钮、账单表格和状态栏。
 * 应用统一的样式表以美化界面。
 */
void FinancePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. 顶部标题与汇总
    titleLabel = new QLabel("记账本", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    
    summaryLabel = new QLabel("本月收入：0 元    本月支出：0 元    结余：0 元", this);
    summaryLabel->setStyleSheet("color: #555555; font-size: 14px; margin-bottom: 10px;");

    // 2. 搜索框
    QHBoxLayout *searchLayout = new QHBoxLayout;
    QLabel *searchIcon = new QLabel("🔍 搜索:", this);
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("输入备注、对手方或金额进行筛选...");
    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(searchLineEdit);

    // 3. 按钮操作区域
    addButton = new QPushButton("添加记录", this);
    deleteButton = new QPushButton("删除记录", this);
    importButton = new QPushButton("导入 CSV", this); 
    exportButton = new QPushButton("导出 CSV", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(importButton); 
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    // 4. 表格配置
    recordTableView = new QTableView(this);
    m_sourceModel = new QStandardItemModel(0, 7, this); // 【改成 7 列】
    QStringList headers;
    headers << "类型" << "对手方" << "账户" << "金额" << "分类" << "日期" << "备注"; // 【加上“账户”】
    m_sourceModel->setHorizontalHeaderLabels(headers);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_sourceModel);
    m_proxyModel->setFilterKeyColumn(-1); // 对所有列进行搜索
    m_proxyModel->setFilterRegularExpression(QRegularExpression("", QRegularExpression::CaseInsensitiveOption));
    recordTableView->setModel(m_proxyModel);

    // 样式表
    recordTableView->setAlternatingRowColors(true); // 交替行颜色
    recordTableView->setShowGrid(false);            // 隐藏网格线
    recordTableView->setFocusPolicy(Qt::NoFocus);   // 禁用焦点
    recordTableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选择整行
    recordTableView->setSelectionMode(QAbstractItemView::SingleSelection); // 单选
    recordTableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    recordTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 根据内容调整列宽
    recordTableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch); // 备注列自动拉伸

    this->setStyleSheet(R"(
        QTableView { background-color: #FFFFFF; alternate-background-color: #F8F9FA; border: 1px solid #E0E0E0; border-radius: 8px; selection-background-color: #E3F2FD; selection-color: #1565C0; font-size: 13px; }
        QHeaderView::section { background-color: #F1F3F5; color: #495057; padding: 8px; border: none; border-bottom: 2px solid #DEE2E6; font-weight: bold; font-size: 14px; }
        QPushButton { background-color: #3B82F6; color: white; border-radius: 6px; padding: 6px 16px; font-weight: bold; }
        QPushButton:hover { background-color: #2563EB; }
        QPushButton:pressed { background-color: #1D4ED8; }
        QLineEdit { padding: 6px; border: 1px solid #CED4DA; border-radius: 6px; }
    )");
 
    // 5. 底部状态栏
    QHBoxLayout *statusBarLayout = new QHBoxLayout;
    statusBarLayout->setContentsMargins(5, 5, 5, 0);
    
    lblStatusDate = new QLabel(QString("当前日期: %1").arg(QDate::currentDate().toString("yyyy-MM-dd")), this);
    QLabel *lblDbPath = new QLabel("当前数据库: lifemate.db", this);
    lblStatusTotal = new QLabel("总行数: 0", this);

    QString statusStyle = "color: #555; font-size: 12px; font-weight: bold;";
    lblStatusDate->setStyleSheet(statusStyle);
    lblDbPath->setStyleSheet(statusStyle);
    lblStatusTotal->setStyleSheet(statusStyle);

    statusBarLayout->addWidget(lblStatusDate);
    statusBarLayout->addWidget(new QLabel("|"));
    statusBarLayout->addWidget(lblDbPath);
    statusBarLayout->addWidget(new QLabel("|"));
    statusBarLayout->addWidget(lblStatusTotal);
    statusBarLayout->addStretch();

    // 将各布局添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addLayout(searchLayout); 
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(recordTableView);
    mainLayout->addLayout(statusBarLayout);
}

/**
 * @brief 连接信号与槽
 *
 * 将按钮点击、搜索框文本变化等信号连接到对应的处理槽函数。
 */
void FinancePage::setupConnections()
{
    connect(addButton, &QPushButton::clicked, this, &FinancePage::onAddButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &FinancePage::onDeleteButtonClicked);
    connect(importButton, &QPushButton::clicked, this, &FinancePage::onImportButtonClicked);
    connect(exportButton, &QPushButton::clicked, this, &FinancePage::onExportButtonClicked);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &FinancePage::onSearchTextChanged);
}

/**
 * @brief 刷新页面数据
 *
 * 从 BillManager 获取所有账单，加载字典数据，更新表格模型、汇总信息和状态栏。
 * 这是页面数据同步的核心方法。
 */
void FinancePage::refreshData()
{
    // 1. 获取所有账单
    auto result = BillManager::getAllBills();
    if (!result.isSuccess()) {
        QMessageBox::critical(this, "数据加载失败", result.errorMessage());
        return;
    }

    m_currentBills = result.data();

    // 2. 加载字典数据（对手方、分类、账户）
    auto counterpartMap = DatabaseManager::loadDict("dict_counterpart");
    auto categoryMap = DatabaseManager::loadDict("dict_category");
    auto subMap = DatabaseManager::loadDict("dict_subject"); // 确保前面加载了字典

    // 3. 清空表格模型
    m_sourceModel->removeRows(0, m_sourceModel->rowCount());

    // 4. 遍历账单，填充表格行
    for (const Bill& bill : std::as_const(m_currentBills)) {
        QList<QStandardItem*> rowItems;

        // 类型列
        QString typeStr = bill.isOutflow() ? "支出" : (bill.isInflow() ? "收入" : "其他");
        auto *typeItem = new QStandardItem(typeStr);
        typeItem->setData(bill.id(), Qt::UserRole); // 在 UserRole 中存储账单ID，用于删除操作
        typeItem->setForeground(bill.isOutflow() ? QColor("#E53935") : QColor("#43A047")); 
        rowItems << typeItem;

        // 对手方列
        QString cpName = counterpartMap.value(bill.counterpartId(), "未知对手方");
        rowItems << new QStandardItem(cpName);

        // 账户列
        QString subName = subMap.value(bill.subjectId(), "未知账户");
        rowItems << new QStandardItem(subName);

        // 金额列（注意：bill.amount().view() 返回的是 int（分），需要转换为元显示）
        // 【潜在问题】view() 返回 int，'f', 2 会将其视为整数并添加 .00，应除以100.0
        rowItems << new QStandardItem(QString::number(bill.amount().view() , 'f', 2));

        // 分类列
        QString catName = categoryMap.value(bill.categoryId(), "未知分类");
        rowItems << new QStandardItem(catName);

        // 日期列
        QDate date = QDate::fromJulianDay(bill.dayNumber());
        rowItems << new QStandardItem(date.toString("yyyy-MM-dd"));

        // 备注列
        rowItems << new QStandardItem(bill.remarks());

        // 设置所有项居中对齐
        for (auto item : rowItems) item->setTextAlignment(Qt::AlignCenter);

        m_sourceModel->appendRow(rowItems);
    }

    // 5. 更新汇总信息
    auto summaryResult = CalculateService::summary(m_currentBills);
    if (summaryResult.isSuccess()) {
        EventSummary summary = summaryResult.data();
        // 【潜在问题】summary.inflow().view() 和 summary.outflow().view() 返回 int（分），应转换为元
        double inflowYuan = summary.inflow().view() ;
        double outflowYuan = summary.outflow().view() ;
        double balanceYuan = inflowYuan - outflowYuan;
        summaryLabel->setText(QString("总收入：%1 元    总支出：%2 元    结余：%3 元")
                              .arg(inflowYuan, 0, 'f', 2)
                              .arg(outflowYuan, 0, 'f', 2)
                              .arg(balanceYuan, 0, 'f', 2));
    }

    // 6. 更新智能填充服务的内存
    updateSmartFillMemory();

    // 7. 更新状态栏总行数
    lblStatusTotal->setText(QString("总行数: %1 行").arg(m_currentBills.size()));
}

/**
 * @brief 更新智能填充服务的内存
 *
 * 将当前所有账单数据传递给 SmartFillService，使其学习并建立填充记忆。
 */
void FinancePage::updateSmartFillMemory()
{
    m_fillService->clear();
    m_fillService->learnFromEventsBatch(m_currentBills);
}

/**
 * @brief 搜索框文本变化时的槽函数
 *
 * 根据输入的文本更新代理模型的正则表达式过滤器，实现实时搜索。
 * @param text 用户输入的搜索文本
 */
void FinancePage::onSearchTextChanged(const QString &text)
{
    QRegularExpression regExp(text, QRegularExpression::CaseInsensitiveOption);
    m_proxyModel->setFilterRegularExpression(regExp);
}

/**
 * @brief 删除按钮点击的槽函数
 *
 * 获取当前选中的账单，确认后调用 BillManager 删除，并刷新页面。
 */
void FinancePage::onDeleteButtonClicked()
{
    QModelIndex proxyIndex = recordTableView->currentIndex();
    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "提示", "请先在表格中选择要删除的记录！");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定删除选中的账单吗？") == QMessageBox::No) return;

    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    int realBillId = m_sourceModel->item(sourceIndex.row(), 0)->data(Qt::UserRole).toInt();

    auto result = BillManager::deleteBill(realBillId);
    if (result.isSuccess()) {
        QMessageBox::information(this, "成功", "删除记录成功！");
        refreshData(); 
    } else {
        QMessageBox::critical(this, "失败", result.errorMessage());
    }
}

void FinancePage::onAddButtonClicked()
{
    AddBillDialog dialog(m_fillService.get(), this);
    if (dialog.exec() == QDialog::Accepted) {
        Bill newBill = dialog.getBill();
        auto result = BillManager::addBill(newBill);
        if (!result.isSuccess()) QMessageBox::warning(this, "失败", result.errorMessage());
        else refreshData(); 
    }
}

// ==========================================
// CSV 解析引擎与转义
// ==========================================
static QString escapeCSV(const QString& text) {
    QString res = text;
    res.replace("\"", "\"\""); 
    if (res.contains(',') || res.contains('"') || res.contains('\n')) {
        res = "\"" + res + "\"";
    }
    return res;
}

static QStringList parseCsvLine(const QString &line) {
    QStringList fields;
    QString currentField;
    bool inQuotes = false;
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        if (c == '\"') {
            if (inQuotes && i + 1 < line.length() && line[i+1] == '\"') {
                currentField.append('\"'); 
                ++i;
            } else {
                inQuotes = !inQuotes; 
            }
        } else if (c == ',' && !inQuotes) {
            fields.append(currentField);
            currentField.clear();
        } else {
            currentField.append(c);
        }
    }
    fields.append(currentField);
    return fields;
}

// ==========================================
// 导出功能
// ==========================================
void FinancePage::onExportButtonClicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "导出 CSV", "我的记账本_账单导出.csv", "CSV 文件 (*.csv)");
    if (filePath.isEmpty()) return;

    auto result = BillManager::getAllBills();
    if (!result.isSuccess()) return;
    QList<Bill> billList = result.data();

    auto cpMap = DatabaseManager::loadDict("dict_counterpart");
    auto catMap = DatabaseManager::loadDict("dict_category");
    auto subMap = DatabaseManager::loadDict("dict_subject"); 

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "失败", "文件被占用！");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    out << QChar(0xFEFF); 
    out << "类型,对手方,账户,金额,分类,日期,备注\n";

    for (const Bill& bill : std::as_const(billList)) {
        QString typeStr = bill.isOutflow() ? "支出" : (bill.isInflow() ? "收入" : "其他");
        QString cpName = cpMap.value(bill.counterpartId(), "未知对手方");
        QString subName = subMap.value(bill.subjectId(), "未知账户"); 
        QString amtStr = QString::number(bill.amount().view(), 'f', 2);
        QString catName = catMap.value(bill.categoryId(), "未知分类");
        QString dateStr = QDate::fromJulianDay(bill.dayNumber()).toString("yyyy-MM-dd");
        
        out << escapeCSV(typeStr) << "," << escapeCSV(cpName) << ","
            << escapeCSV(subName) << "," << escapeCSV(amtStr) << ","
            << escapeCSV(catName) << "," << escapeCSV(dateStr) << ","
            << escapeCSV(bill.remarks()) << "\n";
    }
    file.close();
    QMessageBox::information(this, "成功", QString("成功导出 %1 条账单！").arg(billList.size()));
}

// ==========================================
// 导入功能 (含自动字典扩容)
// ==========================================
void FinancePage::onImportButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "导入 CSV", "", "CSV 文件 (*.csv)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "失败", "无法打开文件！");
        return;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    // 加载现有字典映射
    auto cpMap = DatabaseManager::loadDict("dict_counterpart");
    auto catMap = DatabaseManager::loadDict("dict_category");
    auto subMap = DatabaseManager::loadDict("dict_subject");
    
    // 构建反向映射（名称→ID），用于快速查找
    QMap<QString, quint16> revCp, revCat, revSub;
    for (auto it = cpMap.begin(); it != cpMap.end(); ++it) revCp[it.value()] = it.key();
    for (auto it = catMap.begin(); it != catMap.end(); ++it) revCat[it.value()] = it.key();
    for (auto it = subMap.begin(); it != subMap.end(); ++it) revSub[it.value()] = it.key();

    // 字典扩容起始ID（硬编码，存在冲突风险）
    quint16 nextCpId = 2000;      // 对手方ID起始值
    quint16 nextSubId = 0xFF50;   // 账户ID起始值
    int nextIncPrimary = 20;      // 收入分类主分类ID起始值
    int nextExpPrimary = 20;      // 支出分类主分类ID起始值

    int successCount = 0; // 成功导入的账单数
    bool isFirstLine = true; // 是否为首行（标题行）

    // 逐行读取CSV文件
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (isFirstLine) { isFirstLine = false; continue; } // 跳过标题行

        // 解析CSV行
        QStringList fields = parseCsvLine(line);
        if (fields.size() < 7) continue; // 字段不足则跳过

        // 提取各字段并去除首尾空格
        QString typeStr = fields[0].trimmed(); // 类型（收入/支出）
        QString cpName  = fields[1].trimmed(); // 对手方名称
        QString subName = fields[2].trimmed(); // 账户名称
        QString amtStr  = fields[3].trimmed(); // 金额（元）
        QString catName = fields[4].trimmed(); // 分类名称
        QString dateStr = fields[5].trimmed(); // 日期字符串
        QString remarks = fields[6].trimmed(); // 备注

        // 自动扩容：如果对手方不存在，则创建新记录
        if (!revCp.contains(cpName)) {
            DatabaseManager::saveDictItem("dict_counterpart", nextCpId, cpName);
            revCp[cpName] = nextCpId++;
        }
        // 自动扩容：如果账户不存在，则创建新记录
        if (!revSub.contains(subName)) {
            DatabaseManager::saveDictItem("dict_subject", nextSubId, subName);
            revSub[subName] = nextSubId++;
        }
        // 判断是否为收入类型
        bool isIncome = (typeStr == "收入");
        // 自动扩容：如果分类不存在，则创建新记录
        if (!revCat.contains(catName)) {
            // 根据收入/支出类型生成分类ID
            quint16 newCatId = isIncome ? CategoryID(true, nextIncPrimary++, false, 1).id() 
                                        : CategoryID(false, nextExpPrimary++, false, 1).id();
            DatabaseManager::saveDictItem("dict_category", newCatId, catName);
            revCat[catName] = newCatId;
        }

        // --- 容错日期解析引擎 (防 Excel 篡改格式) ---
        // 尝试多种日期格式解析
        QDate recordDate = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (!recordDate.isValid()) recordDate = QDate::fromString(dateStr, "yyyy/M/d");
        if (!recordDate.isValid()) recordDate = QDate::fromString(dateStr, "yyyy-M-d");
        if (!recordDate.isValid()) recordDate = QDate::fromString(dateStr, "yyyy/MM/dd");
        
        // 终极兜底：如果日期彻底损坏，为了防止出现公元前4714年，使用当天日期
        if (!recordDate.isValid()) {
            recordDate = QDate::currentDate();
        }
        
        int dayNumber = recordDate.toJulianDay(); // 转换为儒略日
        // ---------------------------------------------
        
        // 金额从元转换为分（注意：toDouble()违反int存储约束）
        MoneyRecord amount(amtStr.toDouble());
        
        // 创建账单对象并添加
        Bill newBill(dayNumber, false, revCat[catName], revSub[subName], revCp[cpName], amount, remarks);
        if (BillManager::addBill(newBill).isSuccess()) {
            successCount++; // 添加成功则计数
        }
    }

    file.close(); // 关闭文件
    refreshData(); // 刷新页面数据
    // 显示导入完成消息
    QMessageBox::information(this, "完成", QString("成功导入并合并了 %1 条账单！").arg(successCount));
}