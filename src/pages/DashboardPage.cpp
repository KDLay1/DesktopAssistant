/**
 * @file DashboardPage.cpp
 * @brief 仪表盘页面实现文件
 *
 * 所属分层：Pages（页面层）
 * 功能描述：实现仪表盘页面的UI构建，包括欢迎标题和四个统计卡片
 *
 * @author LifeMate Team
 * @date 2026-06-08
 */

#include "DashboardPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

/**
 * @brief 构造函数 - 初始化仪表盘页面
 *
 * 创建页面并调用 setupUI() 构建界面元素
 *
 * @param parent 父窗口指针，用于Qt对象树管理
 */
DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI(); // 初始化界面布局
}

/**
 * @brief 设置页面UI布局
 *
 * 创建并排列页面所有UI元素，包括：
 * - 主标题和副标题标签
 * - 四个统计卡片（今日任务、今日学习、本月支出、生活轨迹）
 *
 * @note 卡片内容为静态占位符，后续需通过Service层获取真实数据
 */
void DashboardPage::setupUI()
{
    // 创建主垂直布局，设置边距和间距
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 35, 40, 35); // 设置外边距：左、上、右、下
    mainLayout->setSpacing(24); // 设置控件间距

    // 创建主标题和副标题标签
    titleLabel = new QLabel("欢迎使用 LifeMate 桌面生活助手", this);
    subtitleLabel = new QLabel("管理你的记账、课程DDL、学习计时和生活轨迹", this);

    // 设置主标题字体：24号、粗体
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    // 设置副标题字体：12号
    QFont subtitleFont;
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);

    // 创建网格布局用于放置四个统计卡片
    QGridLayout *cardLayout = new QGridLayout;
    cardLayout->setSpacing(20); // 设置卡片间距

    // 创建四个统计卡片
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