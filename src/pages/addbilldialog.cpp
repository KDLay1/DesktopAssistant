#include "addbilldialog.h"
#include "../app/Theme.h"

#include "core/DatabaseManager.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QSet>
#include <QVBoxLayout>

#include "models/CategoryID.h"

namespace {

int nextAvailablePrimary(const QMap<quint16, QString> &categories, bool isIncome, int preferredStart)
{
    QSet<int> usedPrimaries;
    for (auto it = categories.begin(); it != categories.end(); ++it) {
        const CategoryID category(it.key());
        if (category.isIncome() == isIncome) {
            usedPrimaries.insert(category.primary());
        }
    }

    for (int primary = qMax(0, preferredStart); primary < CategoryID::PrimaryCategoryCapacity; ++primary) {
        if (!usedPrimaries.contains(primary)) {
            return primary;
        }
    }

    return -1;
}

}

AddBillDialog::AddBillDialog(SmartFillService *fillService, QWidget *parent)
    : QDialog(parent),
      m_fillService(fillService),
      m_nextCounterpartId(1000),
      m_nextSubjectId(0xFF10),
      m_nextExpenseCategoryPrimary(10),
      m_nextIncomeCategoryPrimary(10)
{
    setWindowTitle("新增账单");
    setMinimumWidth(380);
    setStyleSheet(Theme::dialogStyle());

    setupUI();
    loadInitialData();

    connect(counterpartComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddBillDialog::onCounterpartChanged);
    connect(counterpartComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &AddBillDialog::onCounterpartActivated);
    connect(subjectComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &AddBillDialog::onSubjectActivated);
    connect(categoryComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &AddBillDialog::onCategoryActivated);
}

void AddBillDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();

    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("支出", 0);
    typeComboBox->addItem("收入", 1);

    amountSpinBox = new QDoubleSpinBox(this);
    amountSpinBox->setRange(0.01, 9999999.99);
    amountSpinBox->setDecimals(2);
    amountSpinBox->setSuffix(" 元");

    counterpartComboBox = new QComboBox(this);
    subjectComboBox = new QComboBox(this);
    categoryComboBox = new QComboBox(this);

    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);

    remarksLineEdit = new QLineEdit(this);
    remarksLineEdit->setPlaceholderText("可选备注");

    smartFillHintLabel = new QLabel(this);
    smartFillHintLabel->setStyleSheet(Theme::smartHintStyle());

    formLayout->addRow("类型", typeComboBox);
    formLayout->addRow("金额", amountSpinBox);
    formLayout->addRow("日期", dateEdit);
    formLayout->addRow("对手方", counterpartComboBox);
    formLayout->addRow("", smartFillHintLabel);
    formLayout->addRow("账户", subjectComboBox);
    formLayout->addRow("分类", categoryComboBox);
    formLayout->addRow("备注", remarksLineEdit);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddBillDialog::onAcceptClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
}

void AddBillDialog::loadInitialData()
{
    counterpartComboBox->clear();
    counterpartComboBox->addItem("-- 请选择 --", 0);
    const auto counterparts = DatabaseManager::loadDict("dict_counterpart");
    for (auto it = counterparts.begin(); it != counterparts.end(); ++it) {
        counterpartComboBox->addItem(it.value(), it.key());
        if (it.key() >= m_nextCounterpartId) {
            m_nextCounterpartId = it.key() + 1;
        }
    }
    counterpartComboBox->addItem("+ 新增对手方...", -1);

    subjectComboBox->clear();
    subjectComboBox->addItem("-- 请选择 --", 0);
    const auto subjects = DatabaseManager::loadDict("dict_subject");
    for (auto it = subjects.begin(); it != subjects.end(); ++it) {
        subjectComboBox->addItem(it.value(), it.key());
        if (it.key() >= m_nextSubjectId) {
            m_nextSubjectId = it.key() + 1;
        }
    }
    subjectComboBox->addItem("+ 新增账户...", -1);

    categoryComboBox->clear();
    categoryComboBox->addItem("-- 请选择 --", 0);
    const auto categories = DatabaseManager::loadDict("dict_category");
    for (auto it = categories.begin(); it != categories.end(); ++it) {
        categoryComboBox->addItem(it.value(), it.key());
    }
    m_nextIncomeCategoryPrimary = nextAvailablePrimary(categories, true, m_nextIncomeCategoryPrimary);
    m_nextExpenseCategoryPrimary = nextAvailablePrimary(categories, false, m_nextExpenseCategoryPrimary);
    categoryComboBox->addItem("+ 新增分类...", -1);
}

void AddBillDialog::onCounterpartActivated(int index)
{
    if (counterpartComboBox->itemData(index).toInt() != -1) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(this, "新增对手方", "名称：", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || text.isEmpty()) {
        counterpartComboBox->setCurrentIndex(0);
        return;
    }

    const quint16 newId = m_nextCounterpartId++;
    DatabaseManager::saveDictItem("dict_counterpart", newId, text);
    const int insertPos = counterpartComboBox->count() - 1;
    counterpartComboBox->insertItem(insertPos, text, newId);
    counterpartComboBox->setCurrentIndex(insertPos);
}

void AddBillDialog::onSubjectActivated(int index)
{
    if (subjectComboBox->itemData(index).toInt() != -1) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(this, "新增账户", "名称：", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || text.isEmpty()) {
        subjectComboBox->setCurrentIndex(0);
        return;
    }

    const quint16 newId = m_nextSubjectId++;
    DatabaseManager::saveDictItem("dict_subject", newId, text);
    const int insertPos = subjectComboBox->count() - 1;
    subjectComboBox->insertItem(insertPos, text, newId);
    subjectComboBox->setCurrentIndex(insertPos);
}

void AddBillDialog::onCategoryActivated(int index)
{
    if (categoryComboBox->itemData(index).toInt() != -1) {
        return;
    }

    bool ok = false;
    const QString text = QInputDialog::getText(this, "新增分类", "名称：", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || text.isEmpty()) {
        categoryComboBox->setCurrentIndex(0);
        return;
    }

    const bool isIncome = typeComboBox->currentIndex() == 1;
    int &nextPrimary = isIncome ? m_nextIncomeCategoryPrimary : m_nextExpenseCategoryPrimary;
    if (nextPrimary < 0) {
        QMessageBox::warning(this, "提示", "当前类型已没有可用的新分类编号。");
        categoryComboBox->setCurrentIndex(0);
        return;
    }

    const quint16 newId = isIncome
        ? CategoryID(true, nextPrimary, false, 1).id()
        : CategoryID(false, nextPrimary, false, 1).id();
    DatabaseManager::saveDictItem("dict_category", newId, text);

    const int insertPos = categoryComboBox->count() - 1;
    categoryComboBox->insertItem(insertPos, text, newId);
    categoryComboBox->setCurrentIndex(insertPos);

    nextPrimary = nextAvailablePrimary(DatabaseManager::loadDict("dict_category"), isIncome, nextPrimary + 1);
}

void AddBillDialog::onAcceptClicked()
{
    if (amountSpinBox->value() <= 0) {
        QMessageBox::warning(this, "提示", "金额必须大于 0。");
        return;
    }

    const int counterpartId = counterpartComboBox->currentData().toInt();
    const int subjectId = subjectComboBox->currentData().toInt();
    const int categoryId = categoryComboBox->currentData().toInt();
    if (counterpartId <= 0 || subjectId <= 0 || categoryId <= 0) {
        QMessageBox::warning(this, "提示", "请选择对手方、账户和分类。");
        return;
    }

    accept();
}

void AddBillDialog::onCounterpartChanged(int index)
{
    smartFillHintLabel->clear();

    const quint16 counterpartId = counterpartComboBox->itemData(index).value<quint16>();
    if (counterpartId == 0 || counterpartId == static_cast<quint16>(-1) || m_fillService == nullptr) {
        return;
    }

    auto result = m_fillService->getSuggestion(counterpartId);
    if (!result.isSuccess() || !result.data().hasSuggestion()) {
        return;
    }

    const FillSuggestion suggestion = result.data();
    const int subjectIndex = subjectComboBox->findData(suggestion.suggestedSubject);
    if (subjectIndex >= 0) {
        subjectComboBox->setCurrentIndex(subjectIndex);
    }

    const int categoryIndex = categoryComboBox->findData(suggestion.suggestedCategoryId);
    if (categoryIndex >= 0) {
        categoryComboBox->setCurrentIndex(categoryIndex);
    }

    smartFillHintLabel->setText(QString("智能填充置信度：%1").arg(suggestion.confidenceLevel()));
}

Bill AddBillDialog::getBill() const
{
    const int dayNumber = dateEdit->date().toJulianDay();
    const quint16 categoryId = categoryComboBox->currentData().value<quint16>();
    const quint16 subjectId = subjectComboBox->currentData().value<quint16>();
    const quint16 counterpartId = counterpartComboBox->currentData().value<quint16>();
    const MoneyRecord amount(amountSpinBox->value());
    const QString remarks = remarksLineEdit->text().trimmed();

    return Bill(dayNumber, false, categoryId, subjectId, counterpartId, amount, remarks);
}
