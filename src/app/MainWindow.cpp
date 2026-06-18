#include "MainWindow.h"

#include "../pages/CoursePage.h"
#include "../pages/DashboardPage.h"
#include "../pages/FinancePage.h"
#include "../pages/PomodoroStatsPage.h"
#include "../pages/ReportPage.h"
#include "../pages/TimelinePage.h"
#include "../pages/TimerPage.h"
#include "Theme.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      navList(nullptr),
      stackedWidget(nullptr),
      dashboardPage(nullptr),
      pomodoroStatsPage(nullptr),
      timelinePage(nullptr)
{
    setupUi();
    setupPages();
    setupConnections();
    setupStyle();

    navList->setCurrentRow(0);
}

void MainWindow::setupUi()
{
    setWindowTitle("LifeMate 桌面生活助手");
    resize(1000, 700);

    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    navList = new QListWidget(this);
    navList->setObjectName("navList");
    navList->setFocusPolicy(Qt::NoFocus);
    navList->addItem("首页");
    navList->addItem("记账本");
    navList->addItem("财务报表");
    navList->addItem("课程DDL");
    navList->addItem("学习计时");
    navList->addItem("计时统计");
    navList->addItem("生活轨迹");
    navList->setFixedWidth(180);

    stackedWidget = new QStackedWidget(this);

    mainLayout->addWidget(navList);
    mainLayout->addWidget(stackedWidget, 1);
    setCentralWidget(central);
}

void MainWindow::setupPages()
{
    dashboardPage = new DashboardPage(this);
    auto *financePage = new FinancePage(this);
    auto *reportPage = new ReportPage(this);
    auto *coursePage = new CoursePage(this);
    auto *timerPage = new TimerPage(this);
    pomodoroStatsPage = new PomodoroStatsPage(this);
    timelinePage = new TimelinePage(this);

    stackedWidget->addWidget(dashboardPage);
    stackedWidget->addWidget(financePage);
    stackedWidget->addWidget(reportPage);
    stackedWidget->addWidget(coursePage);
    stackedWidget->addWidget(timerPage);
    stackedWidget->addWidget(pomodoroStatsPage);
    stackedWidget->addWidget(timelinePage);
}

void MainWindow::setupConnections()
{
    connect(navList, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(navList, &QListWidget::currentRowChanged, this, [this](int row) {
        QWidget *page = stackedWidget->widget(row);
        if (page == dashboardPage) {
            dashboardPage->refreshData();
        }
        if (page == pomodoroStatsPage) {
            pomodoroStatsPage->refreshData();
        }
        if (page == timelinePage) {
            timelinePage->refreshData();
        }
    });
}

void MainWindow::setupStyle()
{
    qApp->setStyleSheet(Theme::appStyleSheet());
}
