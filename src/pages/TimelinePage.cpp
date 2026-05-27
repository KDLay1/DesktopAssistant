#include "TimelinePage.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QDate>

TimelinePage::TimelinePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

void TimelinePage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("生活轨迹生成器", this);

    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    descriptionLabel = new QLabel(
        "根据记账记录、课程任务和学习计时记录，生成某一天的生活时间线。",
        this
        );

    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());

    generateButton = new QPushButton("生成时间线", this);
    exportButton = new QPushButton("导出 Markdown", this);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(dateEdit);
    topLayout->addWidget(generateButton);
    topLayout->addWidget(exportButton);
    topLayout->addStretch();

    timelineTextEdit = new QTextEdit(this);

    timelineTextEdit->setPlaceholderText("点击“生成时间线”后，这里会显示每日生活轨迹。");

    timelineTextEdit->setPlainText(
        "示例：\n\n"
        "08:30 - 10:00 学习 C++ Qt\n"
        "12:10 添加记账记录：午餐 18 元\n"
        "23:00 今日总结：学习 3 小时，完成 2 个任务"
        );

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(timelineTextEdit);
}

void TimelinePage::setupConnections()
{
    // 后面由生活轨迹模块负责人继续添加生成和导出逻辑
}