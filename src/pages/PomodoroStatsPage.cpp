#include "PomodoroStatsPage.h"
#include "../data/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

PomodoroStatsPage::PomodoroStatsPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    refreshData();
}

void PomodoroStatsPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 统计标签
    totalCountLabel = new QLabel("总专注次数：0", this);
    totalDurationLabel = new QLabel("总专注时长：0 分钟", this);

    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    totalCountLabel->setFont(font);
    totalDurationLabel->setFont(font);

    // 刷新按钮
    refreshButton = new QPushButton("刷新数据", this);
    connect(refreshButton, &QPushButton::clicked, this, &PomodoroStatsPage::refreshData);

    // 表格
    recordsTable = new QTableWidget(this);
    recordsTable->setColumnCount(3);
    QStringList headers;
    headers << "任务名称" << "时长(分钟)" << "日期";
    recordsTable->setHorizontalHeaderLabels(headers);
    recordsTable->horizontalHeader()->setStretchLastSection(true);
    recordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(totalCountLabel);
    mainLayout->addWidget(totalDurationLabel);
    mainLayout->addWidget(refreshButton);
    mainLayout->addWidget(recordsTable);
}

void PomodoroStatsPage::refreshData() {
    auto records = DatabaseManager::instance().getPomodoroRecords();

    // 更新统计
    int totalCount = records.size();
    int totalDuration = 0;
    totalCountLabel->setText(QString("总专注次数：%1").arg(totalCount));

    // 更新表格
    recordsTable->setRowCount(totalCount);
    for (int i = 0; i < totalCount; ++i) {
        auto &record = records[i];
        totalDuration += record["duration_minutes"].toInt();

        recordsTable->setItem(i, 0, new QTableWidgetItem(record["task_name"].toString()));
        recordsTable->setItem(i, 1, new QTableWidgetItem(QString::number(record["duration_minutes"].toInt())));
        recordsTable->setItem(i, 2, new QTableWidgetItem(record["record_date"].toString()));
    }

    totalDurationLabel->setText(QString("总专注时长：%1 分钟").arg(totalDuration));
}