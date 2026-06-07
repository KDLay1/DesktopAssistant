/**
 * @file DashboardPage.h
 * @brief 仪表盘页面头文件
 *
 * 所属分层：Pages（页面层）
 * 功能：定义仪表盘页面控件，用于展示概览信息（标题、副标题、信息卡片）
 * 依赖：Qt Widgets（QWidget, QLabel, QFrame）
 * 作者：系统架构师
 * 日期：2023-10-01
 */

#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class QLabel;
class QFrame;

/**
 * @class DashboardPage
 * @brief 仪表盘页面主控件
 *
 * 设计目的：作为系统首页，展示关键财务概览信息。
 * 职责：
 * - 显示页面标题和副标题
 * - 通过信息卡片展示多个关键指标（如总资产、本月支出等）
 * - 提供统一的 UI 布局和样式管理
 *
 * @note 该类仅负责 UI 展示，不包含业务逻辑。
 *       业务数据应通过 Services 层获取，并在此处渲染。
 */
class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父控件指针（可选），用于 Qt 对象树管理
     *
     * 内部逻辑：调用 setupUI() 初始化界面布局。
     */
    explicit DashboardPage(QWidget *parent = nullptr);

private:
    /**
     * @brief 初始化界面布局
     *
     * 功能：创建并排列所有 UI 控件，包括标题、副标题和信息卡片。
     * 内部逻辑：
     * - 创建 titleLabel 和 subtitleLabel 并设置文本
     * - 调用 createInfoCard() 创建多个信息卡片
     * - 使用 QVBoxLayout 或 QGridLayout 进行布局
     *
     * @note 该方法仅在构造函数中调用一次。
     */
    void setupUI();

    /**
     * @brief 创建信息卡片控件
     * @param title 卡片标题（如“总资产”）
     * @param value 卡片数值（如“¥12,345.67”）
     * @param description 卡片描述（如“较上月增长 5%”）
     * @return QFrame* 新创建的卡片控件指针
     *
     * 内部逻辑：
     * - 创建一个 QFrame 作为卡片容器
     * - 内部包含三个 QLabel：标题、数值、描述
     * - 设置卡片样式（边框、背景色、字体等）
     *
     * @note 调用方负责管理返回的 QFrame 生命周期（通常由 Qt 对象树自动管理）。
     */
    QFrame* createInfoCard(const QString &title,
                           const QString &value,
                           const QString &description);

    /** @brief 页面主标题标签 */
    QLabel *titleLabel;
    /** @brief 页面副标题标签 */
    QLabel *subtitleLabel;
};

#endif // DASHBOARDPAGE_H
