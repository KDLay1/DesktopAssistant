/**
 * @file MainWindow.cpp
 * @brief 主窗口实现文件
 *
 * 所属分层：App 层（应用层）
 * 功能：创建主窗口界面，管理导航栏和页面切换，处理全局事件
 * 依赖：Pages 层（FinancePage, ReportPage, TimerPage, DesktopPet）
 *        Qt 框架（QWidget, QListWidget, QStackedWidget 等）
 *
 * @author tys
 * @date 2026-06-08
 */

#include "MainWindow.h"
#include "../pages/FinancePage.h"
#include "../pages/ReportPage.h"
#include "../pages/TimerPage.h"   
#include "../pages/DesktopPet.h"
#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "../pages/PomodoroStatsPage.h" 

/**
 * @brief 构造函数：初始化主窗口
 *
 * 依次调用界面创建、页面初始化、信号连接和样式应用，
 * 最后默认选中导航栏的「记账本」页面。
 *
 * @param parent 父窗口指针，默认为 nullptr
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();          // 创建界面布局
    setupPages();       // 初始化各功能页面
    setupConnections(); // 连接信号与槽
    setupStyle();       // 应用全局样式

    // 默认选中导航栏的「记账本」（索引为1）
    navList->setCurrentRow(1);
    // 【新增】强制同步堆叠容器索引，修复初始化不同步问题
    stackedWidget->setCurrentIndex(navList->currentRow());
}

/**
 * @brief 创建主窗口界面布局
 *
 * 设置窗口标题、大小，创建左侧导航列表和右侧页面堆叠容器，
 * 并组装到水平布局中。
 */
void MainWindow::setupUi()
{
    setWindowTitle("DesktopAssistant - 智能记账系统");
    resize(1100, 750);

    // 创建中央部件和水平布局
    QWidget *central = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建导航列表（左侧菜单）
    navList = new QListWidget(this);
    navList->setObjectName("navList");
    navList->setFocusPolicy(Qt::NoFocus);

    // 【新增】关键修复：开启条目可选中、可点击（Qt经典漏配项）
    navList->setSelectionMode(QAbstractItemView::SingleSelection);

    // 导航列表添加菜单项
    navList->addItem("🏠  首页");          // 索引 0
    navList->addItem("📓  记账本");        // 索引 1
    navList->addItem("📊  高级报表");      // 索引 2
    navList->addItem("⏱️ 番茄时钟");       // 索引 3
    navList->addItem("🍅 番茄统计");       // 索引 4
    navList->setFixedWidth(180); // 固定导航栏宽度

    // 创建页面堆叠容器（右侧内容区）
    stackedWidget = new QStackedWidget(this);

    // 组装布局：左侧导航 + 右侧内容
    mainLayout->addWidget(navList);
    mainLayout->addWidget(stackedWidget, 1);
    setCentralWidget(central);
}

/**
 * @brief 初始化各功能页面
 *
 * 创建首页占位页面、记账本页面、报表页面和番茄时钟页面，
 * 并按导航索引顺序添加到堆叠容器中。
 */
void MainWindow::setupPages()
{
    // 0. 首页 (占位页面)
    dummyHomePage = new QWidget(this);
    QVBoxLayout *dummyLayout = new QVBoxLayout(dummyHomePage);
    QLabel *lblComingSoon = new QLabel("首页功能开发中...\n敬请期待！", dummyHomePage);
    lblComingSoon->setAlignment(Qt::AlignCenter);
    lblComingSoon->setStyleSheet("font-size: 24px; color: #7f8fa6; font-weight: bold;");
    dummyLayout->addWidget(lblComingSoon);
    
    // 1. 记账本页
    financePage = new FinancePage(this);
    // 2. 高级报表页
    reportPage  = new ReportPage(this);
    // 3. 番茄时钟页
    timerPage   = new TimerPage(this);
    pomodoroStatsPage = new PomodoroStatsPage(this);
    
    // 严格和导航条目顺序一一对应（索引0~3）
    stackedWidget->addWidget(dummyHomePage); // 索引0
    stackedWidget->addWidget(financePage);   // 索引1
    stackedWidget->addWidget(reportPage);    // 索引2
    stackedWidget->addWidget(timerPage);     // 索引3
    stackedWidget->addWidget(pomodoroStatsPage);  // 索引 4
}

/**
 * @brief 连接信号与槽
 *
 * 建立导航切换和宠物双击事件的信号连接，
 * 实现页面切换和窗口激活功能。
 */
void MainWindow::setupConnections()
{
    // 原有信号连接 + 【加固】增加调试输出，定位信号问题
    connect(navList, &QListWidget::currentRowChanged, this, [=](int row){
        qDebug() << "当前点击导航行索引：" << row; // 控制台打印索引，排查问题
        stackedWidget->setCurrentIndex(row);     // 直接切换页面（替换原有绑定，更稳定）
        if (row == 2) { // 报表页索引，根据你的 navList 顺序调整
            reportPage->executeQuery();
        }
    });

    // 连接宠物双击事件：激活窗口并切换到番茄时钟页面
    connect(timerPage->myPet, &DesktopPet::petDoubleClicked, this, [=](){
        // 1. 把主窗口从最小化/后台拉到最前面
        this->showNormal();
        this->raise();
        this->activateWindow();

        // 2. 自动切换到番茄钟页面 (索引是3)
        navList->setCurrentRow(3);
        stackedWidget->setCurrentIndex(3);
    });
}

/**
 * @brief 应用全局样式表
 *
 * 设置主窗口、导航列表、按钮、表格、输入框等控件的 QSS 样式，
 * 实现统一的视觉风格。
 */
void MainWindow::setupStyle()
{
    // 样式代码完全保留，无需改动
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