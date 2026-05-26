#include "MainWindow.h"

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupPages();
    setupConnections();

    navList->setCurrentRow(0);
}

void MainWindow::setupUi()
{
    //设置窗口结构
    setWindowTitle("LifeMate 桌面生活助手");
    resize(1000, 700);

    QWidget *central = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    navList = new QListWidget(this);
    navList->addItem("首页");
    navList->addItem("记账本");
    navList->addItem("课程DDL");
    navList->addItem("学习计时");
    navList->addItem("生活轨迹");
    navList->setFixedWidth(180);

    stackedWidget = new QStackedWidget(this);

    mainLayout->addWidget(navList);
    mainLayout->addWidget(stackedWidget, 1);

    setCentralWidget(central);
}

void MainWindow::setupPages()
{
    QLabel *homePage = new QLabel("首页", this);
    QLabel *financePage = new QLabel("记账本页面", this);
    QLabel *coursePage = new QLabel("课程DDL页面", this);
    QLabel *timerPage = new QLabel("学习计时页面", this);
    QLabel *timelinePage = new QLabel("生活轨迹页面", this);

    homePage->setAlignment(Qt::AlignCenter);
    financePage->setAlignment(Qt::AlignCenter);
    coursePage->setAlignment(Qt::AlignCenter);
    timerPage->setAlignment(Qt::AlignCenter);
    timelinePage->setAlignment(Qt::AlignCenter);

    stackedWidget->addWidget(homePage);
    stackedWidget->addWidget(financePage);
    stackedWidget->addWidget(coursePage);
    stackedWidget->addWidget(timerPage);
    stackedWidget->addWidget(timelinePage);
}

void MainWindow::setupConnections()
{
    connect(navList, &QListWidget::currentRowChanged,
            stackedWidget, &QStackedWidget::setCurrentIndex);
}