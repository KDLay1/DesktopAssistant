/**
 * @file MainWindow.h
 * @brief 应用程序主窗口头文件
 *
 * 本文件定义了 MainWindow 类，它是整个应用程序的主窗口。
 * 该类属于应用层（app），负责协调和展示各个功能页面（Pages 层）。
 *
 * @author tys
 * @date 2023-10-27
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 前向声明，减少头文件依赖，提高编译速度
class QListWidget;
class QStackedWidget;
class QWidget;
class FinancePage;
class ReportPage;
class TimerPage;
class PomodoroStatsPage;
/**
 * @brief 应用程序主窗口类
 *
 * 作为应用程序的主界面，MainWindow 负责：
 * 1. 创建和管理左侧导航菜单（QListWidget）。
 * 2. 创建和管理右侧内容区域（QStackedWidget）。
 * 3. 初始化并管理各个功能页面（FinancePage, ReportPage, TimerPage）。
 * 4. 处理页面切换等用户交互逻辑。
 *
 * 该类是 UI 层的核心容器，不直接处理业务逻辑。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为 nullptr
     *
     * 构造函数会依次调用 setupUi(), setupPages(), setupConnections(), setupStyle()
     * 来完成主窗口的初始化工作。
     */
    explicit MainWindow(QWidget *parent = nullptr);

private:
    /**
     * @brief 初始化用户界面
     *
     * 创建并布局主窗口的基本 UI 元素，包括：
     * - 左侧的导航列表（navList）
     * - 右侧的堆叠窗口部件（stackedWidget）
     * 并设置它们的基本属性（如大小、布局策略等）。
     */
    void setupUi();

    /**
     * @brief 初始化各个功能页面
     *
     * 实例化 FinancePage, ReportPage, TimerPage 等页面对象，
     * 并将它们添加到 stackedWidget 中，以便通过导航进行切换。
     */
    void setupPages();

    /**
     * @brief 建立信号与槽连接
     *
     * 连接 UI 控件之间的信号与槽，例如：
     * - navList 的 currentRowChanged 信号与 stackedWidget 的 setCurrentIndex 槽。
     * - 其他必要的信号槽连接，以实现页面交互逻辑。
     */
    void setupConnections();

    /**
     * @brief 设置应用程序样式
     *
     * 加载并应用全局样式表（QSS），或设置窗口的主题、字体等视觉属性。
     */
    void setupStyle();

    // 界面控件
    QListWidget *navList;          /**< 左侧导航列表，用于切换页面 */
    QStackedWidget *stackedWidget; /**< 右侧内容区域，用于显示当前选中的页面 */
    PomodoroStatsPage *pomodoroStatsPage = nullptr;
    // 【修改】所有页面改为成员变量（全局可访问，避免局部变量隐患）
    QWidget      *dummyHomePage  = nullptr; /**< 占位首页，用于初始显示或作为默认页 */
    FinancePage  *financePage   = nullptr;  /**< 财务管理页面指针 */
    ReportPage   *reportPage    = nullptr;  /**< 报表统计页面指针 */
    TimerPage    *timerPage     = nullptr;  /**< 定时提醒页面指针 */
};

#endif // MAINWINDOW_H