/**
 * @file CoursePage.h
 * @brief 课程管理页面头文件
 *
 * 所属分层：Pages（界面层）
 * 功能描述：提供课程管理页面的界面定义，包括课程操作按钮和任务列表表格
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef COURSEPAGE_H
#define COURSEPAGE_H

#include <QWidget>

// 前置声明 Qt 控件类，减少头文件依赖
class QPushButton;
class QLabel;
class QTableWidget;

/**
 * @class CoursePage
 * @brief 课程管理页面类
 *
 * 设计目的：作为课程管理功能的主界面，提供课程添加、待办任务管理、数据导出等操作入口
 * 职责：
 *   - 显示课程标题和统计摘要信息
 *   - 提供添加课程、添加待办任务、删除和导出按钮
 *   - 展示任务列表表格
 *
 * @note 该类继承自 QWidget，可作为独立页面嵌入到主窗口中
 */
class CoursePage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针，默认为 nullptr
     *
     * 初始化界面控件并建立信号连接
     */
    explicit CoursePage(QWidget *parent = nullptr);

private:
    /**
     * @brief 初始化界面布局
     *
     * 创建并布局所有界面控件，包括：
     * - 标题标签 (titleLabel)
     * - 摘要标签 (summaryLabel)
     * - 操作按钮 (addclassButton, addtodoButton, deleteButton, exportButton)
     * - 任务列表表格 (taskTable)
     */
    void setupUI();

    /**
     * @brief 建立信号连接
     *
     * 连接按钮的点击信号到对应的槽函数，实现交互逻辑
     * @note 当前未声明槽函数，需要在后续版本中添加
     */
    void setupConnection();

    // 页面标题标签
    QLabel *titleLabel;
    // 课程摘要/统计信息标签
    QLabel *summaryLabel;

    // 添加课程按钮
    QPushButton *addclassButton;
    // 添加待办任务按钮
    QPushButton *addtodoButton;
    // 删除按钮
    QPushButton *deleteButton;
    // 导出按钮
    QPushButton *exportButton;

    // 任务列表表格，用于展示课程相关的任务信息
    QTableWidget *taskTable;
};

#endif // COURSEPAGE_H
