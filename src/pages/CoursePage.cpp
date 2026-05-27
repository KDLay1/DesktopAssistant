#include "CoursePage.h"

#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

CoursePage::CoursePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnection();
}

void CoursePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("课程安排", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    summaryLabel = new QLabel("剩余任务:0件 今日还有0节课", this);

    addclassButton = new QPushButton("添加课程", this);
    addtodoButton = new QPushButton("添加任务", this);
    deleteButton = new QPushButton("删除课程/任务", this);
    exportButton = new QPushButton("导出 CSV", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addclassButton);
    buttonLayout->addWidget(addtodoButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    taskTable = new QTableWidget(this);
    taskTable->setColumnCount(5);

    QStringList headers;
    headers << "课程" << "任务" << "截止时间" << "优先级" << "状态";
    taskTable->setHorizontalHeaderLabels(headers);

    taskTable->horizontalHeader()->setStretchLastSection(true);
    taskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(taskTable);
}

void CoursePage::setupConnection()
{
    // 添加按钮连接逻辑
}