#ifndef TIMELINEPAGE_H
#define TIMELINEPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QTextEdit;
class QDateEdit;

class TimelinePage : public QWidget
{
    Q_OBJECT

public:
    explicit TimelinePage(QWidget *parent = nullptr);
    void refreshData();

private:
    // 创建页面上的控件
    void setupUi();
    // 连接两个按钮
    void setupConnections();
    // 从数据库整理选中日期的记录
    void generateTimeline();
    // 保存为 Markdown 文件
    void exportMarkdown();

    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QDateEdit *dateEdit;
    QPushButton *generateButton;
    QPushButton *exportButton;
    QTextEdit *timelineTextEdit;
};

#endif // TIMELINEPAGE_H
