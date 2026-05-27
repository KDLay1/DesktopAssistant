#include "DashboardPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void DashboardPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 35, 40, 35);
    mainLayout->setSpacing(24);

    titleLabel = new QLabel("欢迎使用 LifeMate 桌面生活助手", this);
    subtitleLabel = new QLabel("管理你的记账、课程DDL、学习计时和生活轨迹", this);

    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QFont subtitleFont;
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);

    QGridLayout *cardLayout = new QGridLayout;
    cardLayout->setSpacing(20);

    QFrame *taskCard = createInfoCard(
        "今日任务",
        "0 个",
        "今日待完成课程任务"
        );

    QFrame *studyCard = createInfoCard(
        "今日学习",
        "0 分钟",
        "今日累计学习时长"
        );

    QFrame *expenseCard = createInfoCard(
        "本月支出",
        "0 元",
        "本月记账支出统计"
        );

    QFrame *timelineCard = createInfoCard(
        "生活轨迹",
        "暂无记录",
        "今日生活轨迹尚未生成"
        );

    cardLayout->addWidget(taskCard, 0, 0);
    cardLayout->addWidget(studyCard, 0, 1);
    cardLayout->addWidget(expenseCard, 1, 0);
    cardLayout->addWidget(timelineCard, 1, 1);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addLayout(cardLayout);
    mainLayout->addStretch();
}

QFrame* DashboardPage::createInfoCard(const QString &title,
                                      const QString &value,
                                      const QString &description)
{
    QFrame *card = new QFrame(this);
    card->setObjectName("infoCard");
    card->setMinimumHeight(130);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    QLabel *titleLabel = new QLabel(title, card);
    QLabel *valueLabel = new QLabel(value, card);
    QLabel *descLabel = new QLabel(description, card);

    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QFont valueFont;
    valueFont.setPointSize(22);
    valueFont.setBold(true);
    valueLabel->setFont(valueFont);

    QFont descFont;
    descFont.setPointSize(10);
    descLabel->setFont(descFont);

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(descLabel);
    layout->addStretch();

    return card;
}