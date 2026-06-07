/**
 * @file PlaceholderPage.h
 * @brief 占位页面头文件
 *
 * 所属分层：Pages（UI 页面层）
 * 功能：提供通用的占位页面，用于显示标题和描述信息
 * 依赖：QWidget、QString、QLabel（Qt 基础库）
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>
#include <QString>

class QLabel;

/**
 * @brief 占位页面类
 *
 * 设计目的：作为 UI 占位页面，用于显示标题和描述信息。
 * 通常用于未实现功能的占位或提示页面，例如“功能开发中”或“页面加载失败”等场景。
 * 继承自 QWidget，可作为独立窗口或嵌入到其他布局中。
 */
class PlaceholderPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     *
     * 创建占位页面实例，并设置标题和描述文本。
     * 内部会创建两个 QLabel 分别显示标题和描述信息。
     *
     * @param title 页面标题文本
     * @param description 页面描述文本
     * @param parent 父窗口部件指针，默认为 nullptr（无父窗口）
     */
    explicit PlaceholderPage(const QString &title,
                             const QString &description,
                             QWidget *parent = nullptr);

private:
    QLabel *titleLabel;         /**< 标题标签，用于显示页面标题 */
    QLabel *descriptionLabel;   /**< 描述标签，用于显示页面描述信息 */
};

#endif // PLACEHOLDERPAGE_H
