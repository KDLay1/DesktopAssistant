#include "TimelinePage.h"

#include "../app/Theme.h"
#include "../core/TimelineRepository.h"

#include <algorithm>

#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace {

// 有时间的记录，最后会按 sortKey 排序
struct TimelineItem {
    QString sortKey;
    QString line;
};

void addTimedItem(QList<TimelineItem> &items, const QString &timeText, const QString &body)
{
    // 时间缺失时仍保留记录，放在排序结果的末尾附近
    const QString displayTime = timeText.isEmpty() ? "??:??" : timeText;
    items.append({displayTime, QString("- %1 %2").arg(displayTime, body)});
}

void addUntimedItem(QStringList &items, const QString &body)
{
    // DDL 和生活事件可能只填日期，没有具体时间
    items.append(QString("- %1").arg(body));
}

} // namespace

TimelinePage::TimelinePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
}

void TimelinePage::refreshData()
{
    // 保留用户选中的日期，只重新读取数据库
    generateTimeline();
}

void TimelinePage::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel("生活轨迹", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    descriptionLabel = new QLabel(
        "按日期汇总课程、DDL、番茄钟、生活事件和财务记录，生成当天的生活轨迹。",
        this);
    descriptionLabel->setWordWrap(true);

    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());

    generateButton = new QPushButton("生成时间线", this);
    exportButton = new QPushButton("导出 Markdown", this);

    generateButton->setProperty("buttonRole", "rose");
    exportButton->setProperty("buttonRole", "sunny");

    auto *topLayout = new QHBoxLayout;
    topLayout->addWidget(dateEdit);
    topLayout->addWidget(generateButton);
    topLayout->addWidget(exportButton);
    topLayout->addStretch();

    timelineTextEdit = new QTextEdit(this);
    timelineTextEdit->setPlaceholderText("点击“生成时间线”后，这里会显示当天的生活轨迹。");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(timelineTextEdit);
}

void TimelinePage::setupConnections()
{
    connect(generateButton, &QPushButton::clicked, this, &TimelinePage::generateTimeline);
    connect(exportButton, &QPushButton::clicked, this, &TimelinePage::exportMarkdown);
}

void TimelinePage::generateTimeline()
{
    const QDate selectedDate = dateEdit->date();
    const QString isoDate = selectedDate.toString("yyyy-MM-dd");

    QList<TimelineItem> timedItems;
    QStringList untimedItems;

    // 页面只负责排版，数据库读取集中在 TimelineRepository
    QString error;
    const QList<TimelineRecord> records =
        TimelineRepository::recordsForDate(selectedDate, &error);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "读取失败", "无法生成时间线：" + error);
        return;
    }
    for (const TimelineRecord &record : records) {
        if (record.time.isEmpty()) {
            addUntimedItem(untimedItems, record.text);
        } else {
            addTimedItem(timedItems, record.time, record.text);
        }
    }

    // 时间相同时再按显示文字排序，保证每次结果顺序一致
    std::sort(timedItems.begin(), timedItems.end(), [](const TimelineItem &a, const TimelineItem &b) {
        return a.sortKey == b.sortKey ? a.line < b.line : a.sortKey < b.sortKey;
    });

    QStringList lines;
    lines << QString("# %1 生活轨迹").arg(isoDate) << "";

    // 没有数据时也生成标题和提示
    if (timedItems.isEmpty() && untimedItems.isEmpty()) {
        lines << "当天没有找到相关记录。";
        timelineTextEdit->setPlainText(lines.join('\n'));
        return;
    }

    if (!timedItems.isEmpty()) {
        lines << "## 按时间记录";
        for (const TimelineItem &item : std::as_const(timedItems)) {
            lines << item.line;
        }
        lines << "";
    }

    if (!untimedItems.isEmpty()) {
        lines << "## 未记录具体时间";
        lines.append(untimedItems);
    }

    timelineTextEdit->setPlainText(lines.join('\n').trimmed());
}

void TimelinePage::exportMarkdown()
{
    const QString content = timelineTextEdit->toPlainText().trimmed();
    if (content.isEmpty()) {
        QMessageBox::information(this, "提示", "请先生成时间线。");
        return;
    }

    const QString defaultName = QString("timeline_%1.md").arg(dateEdit->date().toString("yyyy-MM-dd"));
    const QString filePath = QFileDialog::getSaveFileName(
        this, "导出 Markdown", defaultName, "Markdown Files (*.md)");
    if (filePath.isEmpty()) {
        return;
    }

    // QTextStream 会直接写出当前编辑框里的 Markdown 内容
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "导出失败", "无法写入所选文件。");
        return;
    }

    QTextStream out(&file);
    out << content;
    file.close();

    QMessageBox::information(this, "导出成功", "Markdown 文件已保存。");
}
