#include "TimelinePage.h"

#include "../app/Theme.h"
#include "../core/DatabaseManager.h"

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
#include <QSqlQuery>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace {

struct TimelineItem {
    QString sortKey;
    QString line;
};

QString normalizeTime(const QString &raw)
{
    if (raw.size() >= 16) {
        return raw.mid(11, 5);
    }
    if (raw.size() >= 5) {
        return raw.left(5);
    }
    return {};
}

void addTimedItem(QList<TimelineItem> &items, const QString &timeText, const QString &body)
{
    const QString displayTime = timeText.isEmpty() ? "??:??" : timeText;
    items.append({displayTime, QString("- %1 %2").arg(displayTime, body)});
}

void addUntimedItem(QStringList &items, const QString &body)
{
    items.append(QString("- %1").arg(body));
}

} // namespace

TimelinePage::TimelinePage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupConnections();
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
    const int dayNumber = selectedDate.toJulianDay();

    QList<TimelineItem> timedItems;
    QStringList untimedItems;

    QSqlQuery courseQuery(DatabaseManager::instance().database());
    courseQuery.prepare(
        "SELECT course_name, start_time, end_time, location, teacher "
        "FROM courses "
        "WHERE weekday = :weekday "
        "AND (start_date IS NULL OR start_date = '' OR start_date <= :date) "
        "AND (end_date IS NULL OR end_date = '' OR end_date >= :date) "
        "ORDER BY start_time ASC");
    courseQuery.bindValue(":weekday", selectedDate.dayOfWeek());
    courseQuery.bindValue(":date", isoDate);
    if (courseQuery.exec()) {
        while (courseQuery.next()) {
            QString body = QString("课程：%1").arg(courseQuery.value(0).toString().trimmed());
            const QString location = courseQuery.value(3).toString().trimmed();
            const QString teacher = courseQuery.value(4).toString().trimmed();
            if (!location.isEmpty()) {
                body += QString(" @ %1").arg(location);
            }
            if (!teacher.isEmpty()) {
                body += QString(" (%1)").arg(teacher);
            }
            body += QString(" [%1-%2]")
                        .arg(courseQuery.value(1).toString().trimmed(),
                             courseQuery.value(2).toString().trimmed());
            addTimedItem(timedItems, normalizeTime(courseQuery.value(1).toString()), body);
        }
    }

    QSqlQuery taskQuery(DatabaseManager::instance().database());
    taskQuery.prepare(
        "SELECT course_name, task_title, deadline, priority, status "
        "FROM course_tasks "
        "WHERE deadline LIKE :prefix "
        "ORDER BY deadline ASC, priority ASC");
    taskQuery.bindValue(":prefix", isoDate + "%");
    if (taskQuery.exec()) {
        while (taskQuery.next()) {
            const QString deadline = taskQuery.value(2).toString().trimmed();
            const QString body = QString("DDL：%1 - %2 [优先级%3，%4]")
                                     .arg(taskQuery.value(0).toString().trimmed(),
                                          taskQuery.value(1).toString().trimmed())
                                     .arg(taskQuery.value(3).toInt())
                                     .arg(taskQuery.value(4).toString().trimmed());
            const QString timeText = normalizeTime(deadline);
            if (timeText.isEmpty()) {
                addUntimedItem(untimedItems, body);
            } else {
                addTimedItem(timedItems, timeText, body);
            }
        }
    }

    QSqlQuery pomodoroQuery(DatabaseManager::instance().database());
    pomodoroQuery.prepare(
        "SELECT task_name, duration_minutes, start_time "
        "FROM pomodoro_records "
        "WHERE record_date = :date "
        "ORDER BY start_time ASC");
    pomodoroQuery.bindValue(":date", isoDate);
    if (pomodoroQuery.exec()) {
        while (pomodoroQuery.next()) {
            const QString body = QString("番茄钟：%1（%2 分钟）")
                                     .arg(pomodoroQuery.value(0).toString().trimmed())
                                     .arg(pomodoroQuery.value(1).toInt());
            addTimedItem(timedItems, normalizeTime(pomodoroQuery.value(2).toString()), body);
        }
    }

    QSqlQuery eventQuery(DatabaseManager::instance().database());
    eventQuery.prepare(
        "SELECT event_type, title, description, event_time "
        "FROM life_events "
        "WHERE event_time LIKE :prefix "
        "ORDER BY event_time ASC");
    eventQuery.bindValue(":prefix", isoDate + "%");
    if (eventQuery.exec()) {
        while (eventQuery.next()) {
            QString body = QString("生活事件[%1]：%2")
                               .arg(eventQuery.value(0).toString().trimmed(),
                                    eventQuery.value(1).toString().trimmed());
            const QString description = eventQuery.value(2).toString().trimmed();
            if (!description.isEmpty()) {
                body += QString(" - %1").arg(description);
            }
            const QString timeText = normalizeTime(eventQuery.value(3).toString());
            if (timeText.isEmpty()) {
                addUntimedItem(untimedItems, body);
            } else {
                addTimedItem(timedItems, timeText, body);
            }
        }
    }

    QSqlQuery billQuery(DatabaseManager::instance().database());
    billQuery.prepare(
        "SELECT b.amount, b.remarks, b.created_at, dc.name, ds.name, dcp.name "
        "FROM bills b "
        "LEFT JOIN dict_category dc ON dc.id = b.category_id "
        "LEFT JOIN dict_subject ds ON ds.id = b.subject_id "
        "LEFT JOIN dict_counterpart dcp ON dcp.id = b.counterpart_id "
        "WHERE b.day_number = :day_number "
        "ORDER BY b.created_at ASC, b.id ASC");
    billQuery.bindValue(":day_number", dayNumber);
    if (billQuery.exec()) {
        while (billQuery.next()) {
            QString body = QString("账单：%1 元")
                               .arg(QString::number(billQuery.value(0).toInt() / 100.0, 'f', 2));
            const QString category = billQuery.value(3).toString().trimmed();
            const QString subject = billQuery.value(4).toString().trimmed();
            const QString counterpart = billQuery.value(5).toString().trimmed();
            const QString remarks = billQuery.value(1).toString().trimmed();
            if (!category.isEmpty()) {
                body += QString(" [%1]").arg(category);
            }
            if (!counterpart.isEmpty()) {
                body += QString(" -> %1").arg(counterpart);
            }
            if (!subject.isEmpty()) {
                body += QString("，账户：%1").arg(subject);
            }
            if (!remarks.isEmpty()) {
                body += QString("，备注：%1").arg(remarks);
            }
            addTimedItem(timedItems, normalizeTime(billQuery.value(2).toString()), body);
        }
    }

    QSqlQuery financeQuery(DatabaseManager::instance().database());
    financeQuery.prepare(
        "SELECT type, amount, category, note, created_at "
        "FROM finance_records "
        "WHERE date = :date "
        "ORDER BY created_at ASC, id ASC");
    financeQuery.bindValue(":date", isoDate);
    if (financeQuery.exec()) {
        while (financeQuery.next()) {
            QString body = QString("旧版财务：%1 %2 [%3]")
                               .arg(financeQuery.value(0).toString().trimmed(),
                                    QString::number(financeQuery.value(1).toDouble(), 'f', 2),
                                    financeQuery.value(2).toString().trimmed());
            const QString note = financeQuery.value(3).toString().trimmed();
            if (!note.isEmpty()) {
                body += QString("，备注：%1").arg(note);
            }
            const QString timeText = normalizeTime(financeQuery.value(4).toString());
            if (timeText.isEmpty()) {
                addUntimedItem(untimedItems, body);
            } else {
                addTimedItem(timedItems, timeText, body);
            }
        }
    }

    std::sort(timedItems.begin(), timedItems.end(), [](const TimelineItem &a, const TimelineItem &b) {
        return a.sortKey == b.sortKey ? a.line < b.line : a.sortKey < b.sortKey;
    });

    QStringList lines;
    lines << QString("# %1 生活轨迹").arg(isoDate) << "";

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
