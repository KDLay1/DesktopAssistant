/**
 * @file PlaceholderPage.cpp
 * @brief 占位页面组件实现
 *
 * 所属分层：Pages（UI页面层）
 * 功能：提供未实现功能的占位提示页面，显示标题和描述信息
 * 依赖：Qt Widgets 模块（QLabel, QVBoxLayout, QFont）
 *
 * @author tys
 * @date 2026-06-08
 */

#include "PlaceholderPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

/**
 * @brief 构造函数 - 初始化占位页面
 *
 * 创建并配置标题和描述标签，设置字体样式和布局
 *
 * @param title 页面标题文本，显示在页面顶部
 * @param description 页面描述文本，显示在标题下方
 * @param parent 父窗口指针，默认为nullptr
 *
 * @note 标题字体设置为20pt粗体，描述字体设置为11pt常规
 * @note 标签均居中对齐，上下添加弹性空间实现垂直居中
 */
PlaceholderPage::PlaceholderPage(const QString &title,
                                 const QString &description,
                                 QWidget *parent)
    : QWidget(parent)  // 调用基类构造函数
{
    // 创建垂直布局管理器，管理页面内所有控件
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建标题和描述标签
    titleLabel = new QLabel(title, this);          // 标题标签
    descriptionLabel = new QLabel(description, this); // 描述标签

    // 配置标题字体：20pt，粗体
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    // 配置描述字体：11pt，常规
    QFont descFont;
    descFont.setPointSize(11);
    descriptionLabel->setFont(descFont);

    // 设置标签居中对齐
    titleLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    // 布局结构：弹性空间 → 标题 → 描述 → 弹性空间（实现垂直居中）
    mainLayout->addStretch();          // 上方弹性空间
    mainLayout->addWidget(titleLabel); // 标题标签
    mainLayout->addWidget(descriptionLabel); // 描述标签
    mainLayout->addStretch();          // 下方弹性空间
}