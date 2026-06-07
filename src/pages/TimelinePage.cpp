/**
 * @file TimelinePage.cpp
 * @brief 生活轨迹生成器页面实现
 *
 * 所属分层：Pages（界面层）
 * 功能：提供生活轨迹生成器的用户界面，包含日期选择、生成按钮、导出按钮和文本显示区域
 *
 * @author 项目团队
 * @date 2026-06-08
 */

#include "TimelinePage.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QDate>

/**
 * @brief 构造函数：初始化生活轨迹生成器页面
 *
 * 创建并配置所有界面控件，建立信号连接
 *
 * @param parent 父窗口部件指针，默认为 nullptr
 */
TimelinePage::TimelinePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();          // 初始化界面布局和控件
    setupConnections(); // 建立信号与槽的连接
}

/**
 * @brief 设置用户界面布局和控件
 *
 * 创建垂直布局作为主布局，依次添加：
 * 1. 标题标签（"生活轨迹生成器"，字体20号加粗）
 * 2. 功能描述标签
 * 3. 水平布局（包含日期选择控件、生成按钮、导出按钮）
 * 4. 时间线文本显示区域（带占位符和示例内容）
 */
void TimelinePage::setupUi()
{
    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建并配置标题标签
    titleLabel = new QLabel("生活轨迹生成器", this);

    QFont titleFont;
    titleFont.setPointSize(20);  // 设置字体大小为20
    titleFont.setBold(true);     // 设置字体加粗
    titleLabel->setFont(titleFont);

    // 创建功能描述标签
    descriptionLabel = new QLabel(
        "根据记账记录、课程任务和学习计时记录，生成某一天的生活时间线。",
        this
        );

    // 创建日期选择控件，启用日历弹出，默认显示当前日期
    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());

    // 创建生成和导出按钮
    generateButton = new QPushButton("生成时间线", this);
    exportButton = new QPushButton("导出 Markdown", this);

    // 创建顶部水平布局，放置日期控件和按钮
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(dateEdit);        // 添加日期选择控件
    topLayout->addWidget(generateButton);  // 添加生成按钮
    topLayout->addWidget(exportButton);    // 添加导出按钮
    topLayout->addStretch();               // 添加弹性空间

    // 创建时间线文本显示区域
    timelineTextEdit = new QTextEdit(this);

    // 设置占位符文本，提示用户操作
    timelineTextEdit->setPlaceholderText("点击“生成时间线”后，这里会显示每日生活轨迹。");

    // 设置示例文本，展示时间线格式
    timelineTextEdit->setPlainText(
        "示例：\n\n"
        "08:30 - 10:00 学习 C++ Qt\n"
        "12:10 添加记账记录：午餐 18 元\n"
        "23:00 今日总结：学习 3 小时，完成 2 个任务"
        );

    // 将所有控件添加到主布局
    mainLayout->addWidget(titleLabel);        // 添加标题标签
    mainLayout->addWidget(descriptionLabel);  // 添加描述标签
    mainLayout->addLayout(topLayout);         // 添加顶部布局
    mainLayout->addWidget(timelineTextEdit);  // 添加文本显示区域
}

/**
 * @brief 建立信号与槽的连接
 *
 * 预留接口，后续由生活轨迹模块负责人实现：
 * - 生成按钮点击 → 调用生成时间线逻辑
 * - 导出按钮点击 → 调用导出 Markdown 逻辑
 *
 * @note 当前为空函数，功能待实现
 */
void TimelinePage::setupConnections()
{
    // 后面由生活轨迹模块负责人继续添加生成和导出逻辑
}