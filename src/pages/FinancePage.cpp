#include "FinancePage.h"

#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

FinacePage::FinacePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnection();
}

void FinacePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("记账本", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    summaryLabel = new QLabel("本月收入：0 元    本月支出：0 元    结余：0 元", this);

    addButton = new QPushButton("添加记录", this);
    deleteButton = new QPushButton("删除记录", this);
    exportButton = new QPushButton("导出 CSV", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    recordTable = new QTableWidget(this);
    recordTable->setColumnCount(5);

    QStringList headers;
    headers << "类型" << "金额" << "分类" << "日期" << "备注";
    recordTable->setHorizontalHeaderLabels(headers);

    recordTable->horizontalHeader()->setStretchLastSection(true);
    recordTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    recordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recordTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(recordTable);
}

void FinacePage::setupConnection()
{
    // 添加按钮连接逻辑
}