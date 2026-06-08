#include "TimerPage.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QAbstractItemView>

TimerPage::TimerPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

void TimerPage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("学习计时", this);

    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    timeLabel = new QLabel("00:00:00", this);

    QFont timeFont;
    timeFont.setPointSize(32);
    timeFont.setBold(true);
    timeLabel->setFont(timeFont);
    timeLabel->setAlignment(Qt::AlignCenter);

    summaryLabel = new QLabel("今日学习时长：0 分钟", this);

    taskNameEdit = new QLineEdit(this);
    taskNameEdit->setPlaceholderText("请输入学习内容");

    startButton = new QPushButton("开始", this);
    pauseButton = new QPushButton("暂停", this);
    finishButton = new QPushButton("结束并保存", this);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(taskNameEdit);
    controlLayout->addWidget(startButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(finishButton);

    sessionTable = new QTableWidget(this);
    sessionTable->setColumnCount(4);

    QStringList headers;
    headers << "学习内容" << "开始时间" << "结束时间" << "持续时长";
    sessionTable->setHorizontalHeaderLabels(headers);

    sessionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    sessionTable->horizontalHeader()->setStretchLastSection(true);
    sessionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sessionTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(timeLabel);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(sessionTable);
}

void TimerPage::setupConnections()
{
    // 后面由学习计时模块负责人继续添加计时逻辑
}