/**
 * @file addbilldialog.cpp
 * @brief 新增账单记录对话框的实现文件
 * 
 * 所属分层：Pages（界面层）
 * 功能：提供新增账单记录的UI界面，支持智能填充、动态添加字典项
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "addbilldialog.h"
#include "data/DatabaseManager.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QVariant>
#include <QInputDialog> 
#include <QDebug>

#include "models/CategoryID.h"

/**
 * @brief 构造函数：初始化对话框
 * 
 * @param fillService 智能填充服务指针，用于根据对手方自动填充其他字段
 * @param parent 父窗口指针
 * 
 * @note 初始化自增ID计数器，确保新添加的字典项ID不冲突
 */
AddBillDialog::AddBillDialog(SmartFillService* fillService, QWidget *parent)
    : QDialog(parent), m_fillService(fillService),
      m_nextCounterpartId(1000),             // 对手方从 1000 开始分配
      m_nextSubjectId(0xFF10),               // 内部账户从 0xFF10 开始分配
      m_nextExpenseCategoryPrimary(10),      // 支出主分类从 10 开始分配
      m_nextIncomeCategoryPrimary(10)        // 收入主分类从 10 开始分配
{
    setWindowTitle("新增账单记录");
    setMinimumWidth(380); 

    setupUI();          // 创建UI布局
    loadInitialData();  // 加载初始数据到下拉框

    // 智能填充触发器：当对手方选择改变时，自动填充账户和分类
    connect(counterpartComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddBillDialog::onCounterpartChanged);

    // 动态添加新条目触发器 (使用 activated 避免死循环)
    connect(counterpartComboBox, QOverload<int>::of(&QComboBox::activated), this, &AddBillDialog::onCounterpartActivated);
    connect(subjectComboBox, QOverload<int>::of(&QComboBox::activated), this, &AddBillDialog::onSubjectActivated);
    connect(categoryComboBox, QOverload<int>::of(&QComboBox::activated), this, &AddBillDialog::onCategoryActivated);
}

/**
 * @brief 设置UI布局
 * 
 * 创建表单布局，包含所有输入控件：
 * - 收支类型（下拉框）
 * - 交易金额（双精度输入框，显示为元）
 * - 交易日期（日期选择器）
 * - 对手方（下拉框，支持动态添加）
 * - 智能填充提示标签
 * - 主体账户（下拉框，支持动态添加）
 * - 交易分类（下拉框，支持动态添加）
 * - 备注（单行文本输入框）
 * - 确定/取消按钮
 */
void AddBillDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // 收支类型下拉框：0=支出，1=收入
    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("支出", 0);
    typeComboBox->addItem("收入", 1);

    // 金额输入框：范围0.01~9999999.99，保留2位小数
    amountSpinBox = new QDoubleSpinBox(this);
    amountSpinBox->setRange(0.01, 9999999.99);
    amountSpinBox->setDecimals(2);
    amountSpinBox->setSuffix(" 元");

    // 下拉框控件
    counterpartComboBox = new QComboBox(this);
    subjectComboBox = new QComboBox(this);
    categoryComboBox = new QComboBox(this);

    // 日期选择器：默认当前日期，支持日历弹出
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);

    // 备注输入框：最多50字
    remarksLineEdit = new QLineEdit(this);
    remarksLineEdit->setPlaceholderText("选填（最多50字）");

    // 智能填充提示标签：绿色粗体显示
    smartFillHintLabel = new QLabel(this);
    smartFillHintLabel->setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold;");
    smartFillHintLabel->setFixedHeight(20);

    // 添加表单行
    formLayout->addRow("收支类型：", typeComboBox);
    formLayout->addRow("交易金额：", amountSpinBox);
    formLayout->addRow("交易日期：", dateEdit);
    formLayout->addRow("对 手 方：", counterpartComboBox);
    formLayout->addRow("", smartFillHintLabel); 
    formLayout->addRow("主体账户：", subjectComboBox);
    formLayout->addRow("交易分类：", categoryComboBox);
    formLayout->addRow("备    注：", remarksLineEdit);

    // 按钮框：确定和取消
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddBillDialog::onAcceptClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
}

/**
 * @brief 加载初始数据到下拉框
 * 
 * 从数据库加载字典数据（对手方、账户、分类），填充到对应的下拉框中。
 * 每个下拉框包含：
 * - 默认选项 "-- 请选择 --"（ID=0）
 * - 已存在的字典项
 * - 添加新条目选项（ID=-1）
 * 
 * @note 同时更新自增ID计数器，确保新添加的ID不重复
 */
void AddBillDialog::loadInitialData()
{
    // 加载对手方字典
    counterpartComboBox->addItem("-- 请选择 --", 0);
    auto counterparts = DatabaseManager::loadDict("dict_counterpart");
    for (auto it = counterparts.begin(); it != counterparts.end(); ++it) {
        counterpartComboBox->addItem(it.value(), it.key());
        if (it.key() >= m_nextCounterpartId) m_nextCounterpartId = it.key() + 1; // 刷新自增计数器
    }
    counterpartComboBox->addItem("➕ 添加新对手方...", -1); 

    // 加载账户字典
    subjectComboBox->addItem("-- 请选择 --", 0);
    auto subjects = DatabaseManager::loadDict("dict_subject");
    for (auto it = subjects.begin(); it != subjects.end(); ++it) {
        subjectComboBox->addItem(it.value(), it.key());
        if (it.key() >= m_nextSubjectId) m_nextSubjectId = it.key() + 1;
    }
    subjectComboBox->addItem("➕ 添加新账户...", -1); 

    // 加载分类字典
    categoryComboBox->addItem("-- 请选择 --", 0);
    auto categories = DatabaseManager::loadDict("dict_category");
    for (auto it = categories.begin(); it != categories.end(); ++it) {
        categoryComboBox->addItem(it.value(), it.key());
        // 简单提取主分类ID用于自增
        int primary = CategoryID(it.key()).primary();
        if (CategoryID(it.key()).isInflow() && primary >= m_nextIncomeCategoryPrimary) m_nextIncomeCategoryPrimary = primary + 1;
        if (CategoryID(it.key()).isOutflow() && primary >= m_nextExpenseCategoryPrimary) m_nextExpenseCategoryPrimary = primary + 1;
    }
    categoryComboBox->addItem("➕ 添加新分类...", -1); 
}

/**
 * @brief 处理对手方下拉框的"添加新对手方"选项
 * 
 * @param index 当前选中的索引
 * 
 * @note 当用户选择"添加新对手方..."（ID=-1）时触发
 *       弹出输入对话框，获取名称后保存到数据库并插入到下拉框
 */
void AddBillDialog::onCounterpartActivated(int index) {
    if (counterpartComboBox->itemData(index).toInt() == -1) {
        bool ok;
        QString text = QInputDialog::getText(this, "新增对手方", "请输入名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.trimmed().isEmpty()) {
            quint16 newId = m_nextCounterpartId++;
            DatabaseManager::saveDictItem("dict_counterpart", newId, text); // 【核心】存入数据库
            int pos = counterpartComboBox->count() - 1;
            counterpartComboBox->insertItem(pos, text, newId);
            counterpartComboBox->setCurrentIndex(pos);
        } else counterpartComboBox->setCurrentIndex(0);
    }
}

/**
 * @brief 处理账户下拉框的"添加新账户"选项
 * 
 * @param index 当前选中的索引
 * 
 * @note 当用户选择"添加新账户..."（ID=-1）时触发
 *       弹出输入对话框，获取名称后保存到数据库并插入到下拉框
 */
void AddBillDialog::onSubjectActivated(int index) {
    if (subjectComboBox->itemData(index).toInt() == -1) {
        bool ok;
        QString text = QInputDialog::getText(this, "新增账户", "请输入名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.trimmed().isEmpty()) {
            quint16 newId = m_nextSubjectId++;
            DatabaseManager::saveDictItem("dict_subject", newId, text); // 【核心】存入数据库
            int pos = subjectComboBox->count() - 1;
            subjectComboBox->insertItem(pos, text, newId);
            subjectComboBox->setCurrentIndex(pos);
        } else subjectComboBox->setCurrentIndex(0);
    }
}

/**
 * @brief 处理分类下拉框的"添加新分类"选项
 * 
 * @param index 当前选中的索引
 * 
 * @note 当用户选择"添加新分类..."（ID=-1）时触发
 *       根据当前收支类型生成对应的分类ID（收入或支出）
 *       弹出输入对话框，获取名称后保存到数据库并插入到下拉框
 */
void AddBillDialog::onCategoryActivated(int index) {
    if (categoryComboBox->itemData(index).toInt() == -1) {
        bool ok;
        QString text = QInputDialog::getText(this, "新增分类", "请输入名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.trimmed().isEmpty()) {
            // 根据收支类型生成分类ID：收入使用收入主分类，支出使用支出主分类
            quint16 newId = (typeComboBox->currentIndex() == 1) 
                            ? CategoryID(true, m_nextIncomeCategoryPrimary++, false, 1).id()
                            : CategoryID(false, m_nextExpenseCategoryPrimary++, false, 1).id();
            DatabaseManager::saveDictItem("dict_category", newId, text); // 【核心】存入数据库
            int pos = categoryComboBox->count() - 1;
            categoryComboBox->insertItem(pos, text, newId);
            categoryComboBox->setCurrentIndex(pos);
        } else categoryComboBox->setCurrentIndex(0);
    }
}

/**
 * @brief 处理确定按钮点击事件
 * 
 * 验证输入有效性：
 * 1. 金额必须大于0
 * 2. 对手方、账户、分类必须选择有效条目（不能是默认选项或添加选项）
 * 
 * @note 验证通过后调用accept()关闭对话框
 */
void AddBillDialog::onAcceptClicked()
{
    // 验证金额必须大于0
    if (amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "提示", "金额必须大于0！");
        return;
    }
    
    // 强制验证下拉框是否选中有效条目 (不能是 "--请选择--" 也不能是 "➕ 添加")
    int counterId = counterpartComboBox->currentData().toInt();
    int subId = subjectComboBox->currentData().toInt();
    int catId = categoryComboBox->currentData().toInt();

    if (counterId <= 0 || counterId == -1 || 
        subId <= 0 || subId == -1 || 
        catId <= 0 || catId == -1) {
        QMessageBox::warning(this, "提示", "请完整并正确选择对手方、主体账户和交易分类！");
        return;
    }
    
    accept(); // 关闭对话框并返回Accepted
}

/**
 * @brief 处理对手方选择变化事件（智能填充）
 * 
 * @param index 当前选中的索引
 * 
 * @note 当用户切换对手方时，根据智能填充服务获取建议：
 *       - 自动填充主体账户和交易分类
 *       - 显示置信度提示
 *       如果对手方无效或服务不可用，则清除提示
 */
void AddBillDialog::onCounterpartChanged(int index)
{
    smartFillHintLabel->clear();

    quint16 counterpartId = counterpartComboBox->itemData(index).value<quint16>();
    if (counterpartId <= 0 || counterpartId == static_cast<quint16>(-1) || !m_fillService) return;

    auto result = m_fillService->getSuggestion(counterpartId);

    if (result.isSuccess() && result.data().hasSuggestion()) {
        FillSuggestion suggestion = result.data();

        // 自动填充主体账户
        int subIdx = subjectComboBox->findData(suggestion.suggestedSubject);
        if (subIdx >= 0) subjectComboBox->setCurrentIndex(subIdx);

        // 自动填充交易分类
        int catIdx = categoryComboBox->findData(suggestion.suggestedCategoryId);
        if (catIdx >= 0) categoryComboBox->setCurrentIndex(catIdx);

        // 显示智能填充提示
        smartFillHintLabel->setText(QString("✨ 已根据习惯智能填充 (置信度: %1)")
                                        .arg(suggestion.confidenceLevel()));
    }
}

// ==========================================
// 组装最终账单实体
// ==========================================

/**
 * @brief 获取组装好的账单实体
 * 
 * @return Bill 包含所有输入信息的账单对象
 * 
 * @note 此函数在对话框接受后调用，用于获取用户输入的数据
 *       金额从UI的元转换为MoneyRecord内部存储的分
 */
Bill AddBillDialog::getBill() const
{
    bool isRefund = (typeComboBox->currentIndex() == 1); // 1=收入

    int dayNumber = dateEdit->date().toJulianDay(); // 转换为儒略日
    quint16 categoryId = categoryComboBox->currentData().value<quint16>();
    quint16 subjectId = subjectComboBox->currentData().value<quint16>();
    quint16 counterpartId = counterpartComboBox->currentData().value<quint16>();
    MoneyRecord amount(amountSpinBox->value()); // 注意：此处存在浮点精度问题，建议使用qRound(value*100)
    QString remarks = remarksLineEdit->text();

    return Bill(dayNumber, isRefund, categoryId, subjectId, counterpartId, amount, remarks);
}
