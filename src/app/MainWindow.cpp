#include "MainWindow.h"

#include "../pages/CoursePage.h"
#include "../pages/DashboardPage.h"
#include "../pages/FinancePage.h"
#include "../pages/PomodoroStatsPage.h"
#include "../pages/ReportPage.h"
#include "../pages/TimelinePage.h"
#include "../pages/TimerPage.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>

namespace {

constexpr int kStatsPageIndex = 5;

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      navList(nullptr),
      stackedWidget(nullptr),
      pomodoroStatsPage(nullptr)
{
    setupUi();
    setupPages();
    setupConnections();
    setupStyle();

    navList->setCurrentRow(0);
}

void MainWindow::setupUi()
{
    setWindowTitle("LifeMate");
    resize(1000, 700);

    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    navList = new QListWidget(this);
    navList->setObjectName("navList");
    navList->setFocusPolicy(Qt::NoFocus);
    navList->addItem("Dashboard");
    navList->addItem("Finance");
    navList->addItem("Reports");
    navList->addItem("Courses");
    navList->addItem("Timer");
    navList->addItem("Pomodoro Stats");
    navList->addItem("Timeline");
    navList->setFixedWidth(180);

    stackedWidget = new QStackedWidget(this);

    mainLayout->addWidget(navList);
    mainLayout->addWidget(stackedWidget, 1);
    setCentralWidget(central);
}

void MainWindow::setupPages()
{
    auto *dashboardPage = new DashboardPage(this);
    auto *financePage = new FinancePage(this);
    auto *reportPage = new ReportPage(this);
    auto *coursePage = new CoursePage(this);
    auto *timerPage = new TimerPage(this);
    pomodoroStatsPage = new PomodoroStatsPage(this);
    auto *timelinePage = new TimelinePage(this);

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
        if (row == kStatsPageIndex && pomodoroStatsPage != nullptr) {
            pomodoroStatsPage->refreshData();
        }
    });
}

void MainWindow::setupStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f6fa;
        }

        QListWidget#navList {
            background-color: #2f3640;
            color: #f5f6fa;
            border: none;
            font-size: 16px;
            padding-top: 12px;
        }

        QListWidget#navList::item {
            height: 44px;
            padding-left: 18px;
            border-radius: 8px;
            margin: 4px 8px;
        }

        QListWidget#navList::item:hover {
            background-color: #353b48;
        }

        QListWidget#navList::item:selected {
            background-color: #40739e;
            color: white;
            font-weight: bold;
            border: none;
        }

        QLabel {
            color: #2f3640;
        }

        QPushButton {
            background-color: #40739e;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 14px;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #487eb0;
        }

        QPushButton:pressed {
            background-color: #273c75;
        }

        QTableWidget {
            background-color: white;
            border: 1px solid #dcdde1;
            gridline-color: #dcdde1;
            font-size: 14px;
        }

        QHeaderView::section {
            background-color: #f1f2f6;
            color: #2f3640;
            padding: 6px;
            border: none;
            font-weight: bold;
        }

        QLineEdit, QTextEdit, QDateEdit {
            background-color: white;
            border: 1px solid #dcdde1;
            border-radius: 6px;
            padding: 6px;
            font-size: 14px;
        }

        QFrame#infoCard {
            background-color: white;
            border: 1px solid #dcdde1;
            border-radius: 12px;
        }

        QFrame#infoCard:hover {
            border: 1px solid #40739e;
        }
    )");
}
