#include "DashboardPage.h"
#include "../app/Theme.h"
#include "../core/CourseRepository.h"
#include "../core/DatabaseManager.h"
#include "../core/TimelineRepository.h"

#include <QDate>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QVBoxLayout>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent),
      taskValueLabel(nullptr),
      studyValueLabel(nullptr),
      expenseValueLabel(nullptr),
      timelineValueLabel(nullptr)
{
    setupUI();
    refreshData();
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
    subtitleLabel->setStyleSheet(Theme::subtleTextStyle());

    QGridLayout *cardLayout = new QGridLayout;
    cardLayout->setSpacing(20);

    QFrame *taskCard = createInfoCard(
        "今日任务",
        "0 个",
        "今日待完成课程任务",
        taskValueLabel
        );

    QFrame *studyCard = createInfoCard(
        "今日学习",
        "0 分钟",
        "今日累计学习时长",
        studyValueLabel
        );

    QFrame *expenseCard = createInfoCard(
        "本月支出",
        "0 元",
        "本月记账支出统计",
        expenseValueLabel
        );

    QFrame *timelineCard = createInfoCard(
        "生活轨迹",
        "暂无记录",
        "今日课程、DDL和生活事件",
        timelineValueLabel
        );

    taskCard->setProperty("cardTone", "rose");
    studyCard->setProperty("cardTone", "lavender");
    expenseCard->setProperty("cardTone", "peach");
    timelineCard->setProperty("cardTone", "mint");

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
                                      const QString &description,
                                      QLabel *&valueLabel)
{
    QFrame *card = new QFrame(this);
    card->setObjectName("infoCard");
    card->setMinimumHeight(130);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(8);

    QLabel *titleLabel = new QLabel(title, card);
    valueLabel = new QLabel(value, card);
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
    descLabel->setStyleSheet(Theme::subtleTextStyle());

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(descLabel);
    layout->addStretch();

    return card;
}

void DashboardPage::refreshData()
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QString today = QDate::currentDate().toString("yyyy-MM-dd");

    // 课程卡片的数据由课程仓库统计
    int taskCount = CourseRepository::unfinishedTaskCount(QDate::currentDate());
    taskValueLabel->setText(QString("%1 个").arg(taskCount));

    QSqlQuery studyQuery(db);
    studyQuery.prepare(
        "SELECT COALESCE(SUM(duration_minutes), 0) "
        "FROM pomodoro_records WHERE record_date = :date");
    studyQuery.bindValue(":date", today);
    if (studyQuery.exec() && studyQuery.next()) {
        studyValueLabel->setText(QString("%1 分钟").arg(studyQuery.value(0).toInt()));
    } else {
        studyValueLabel->setText("--");
    }

    QDate monthStart(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    QDate monthEnd = monthStart.addMonths(1).addDays(-1);
    qint64 expenseCents = 0;
    const QList<Bill> bills = DatabaseManager::getAllBills();
    for (const Bill &bill : bills) {
        QDate billDate = QDate::fromJulianDay(bill.dayNumber());
        if (billDate >= monthStart && billDate <= monthEnd && bill.isOutflow()) {
            expenseCents += static_cast<int>(bill.amount());
        }
    }
    expenseValueLabel->setText(
        QString("%1 元").arg(QString::number(expenseCents / 100.0, 'f', 2)));

    // 时间线卡片和时间线页面使用同一份汇总结果
    int timelineCount = TimelineRepository::recordsForDate(QDate::currentDate()).size();
    timelineValueLabel->setText(timelineCount == 0
        ? "暂无记录"
        : QString("%1 条").arg(timelineCount));
}
