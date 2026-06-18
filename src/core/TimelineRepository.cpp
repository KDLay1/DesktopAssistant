#include "TimelineRepository.h"

#include "CourseRepository.h"
#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace {

void saveError(QString *error, const QString &text)
{
    if (error != nullptr) {
        *error = text;
    }
}

QString shortTime(const QString &raw)
{
    if (raw.size() >= 16) {
        return raw.mid(11, 5);
    }
    if (raw.size() >= 5) {
        return raw.left(5);
    }
    return {};
}

} // namespace

bool TimelineRepository::initTables(QSqlDatabase db, QString *error)
{
    QSqlQuery query(db);
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS work_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            category TEXT,
            start_time TEXT NOT NULL,
            end_time TEXT,
            duration_minutes INTEGER,
            note TEXT,
            created_at TEXT NOT NULL
        )
    )")) {
        saveError(error, query.lastError().text());
        return false;
    }

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS life_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_type TEXT NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            related_path TEXT,
            event_time TEXT NOT NULL,
            created_at TEXT NOT NULL
        )
    )")) {
        saveError(error, query.lastError().text());
        return false;
    }
    return true;
}

QList<TimelineRecord> TimelineRepository::recordsForDate(const QDate &date, QString *error)
{
    QList<TimelineRecord> records;
    QString isoDate = date.toString("yyyy-MM-dd");
    QSqlDatabase db = DatabaseManager::instance().database();

    // 课程和 DDL 通过课程仓库读取，避免重复写相同 SQL
    const QList<CourseRecord> courses = CourseRepository::coursesForDate(date, error);
    if (error != nullptr && !error->isEmpty()) {
        return records;
    }
    for (const CourseRecord &course : courses) {
        if (course.weekday != date.dayOfWeek()) {
            continue;
        }
        QString text = QString("课程：%1").arg(course.name.trimmed());
        if (!course.location.trimmed().isEmpty()) {
            text += QString(" @ %1").arg(course.location.trimmed());
        }
        if (!course.teacher.trimmed().isEmpty()) {
            text += QString(" (%1)").arg(course.teacher.trimmed());
        }
        text += QString(" [%1-%2]").arg(course.startTime, course.endTime);
        records.append({shortTime(course.startTime), text});
    }

    const QList<CourseTaskRecord> tasks = CourseRepository::tasksForDate(date, error);
    if (error != nullptr && !error->isEmpty()) {
        return records;
    }
    for (const CourseTaskRecord &task : tasks) {
        QString text = QString("DDL：%1 - %2 [优先级%3，%4]")
                           .arg(task.courseName, task.title)
                           .arg(task.priority)
                           .arg(task.status);
        records.append({shortTime(task.deadline), text});
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT task_name, duration_minutes, start_time FROM pomodoro_records "
        "WHERE record_date = :date ORDER BY start_time");
    query.bindValue(":date", isoDate);
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return records;
    }
    while (query.next()) {
        QString text = QString("番茄钟：%1（%2 分钟）")
                           .arg(query.value(0).toString().trimmed())
                           .arg(query.value(1).toInt());
        QString time = shortTime(query.value(2).toString());
        records.append({time.isEmpty() ? "??:??" : time, text});
    }

    query.prepare(
        "SELECT event_type, title, description, event_time FROM life_events "
        "WHERE event_time LIKE :date ORDER BY event_time");
    query.bindValue(":date", isoDate + "%");
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return records;
    }
    while (query.next()) {
        QString text = QString("生活事件[%1]：%2")
                           .arg(query.value(0).toString().trimmed(),
                                query.value(1).toString().trimmed());
        QString description = query.value(2).toString().trimmed();
        if (!description.isEmpty()) {
            text += " - " + description;
        }
        records.append({shortTime(query.value(3).toString()), text});
    }

    query.prepare(
        "SELECT b.amount, b.remarks, b.created_at, dc.name, ds.name, dcp.name "
        "FROM bills b "
        "LEFT JOIN dict_category dc ON dc.id = b.category_id "
        "LEFT JOIN dict_subject ds ON ds.id = b.subject_id "
        "LEFT JOIN dict_counterpart dcp ON dcp.id = b.counterpart_id "
        "WHERE b.day_number = :day ORDER BY b.created_at, b.id");
    query.bindValue(":day", date.toJulianDay());
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return records;
    }
    while (query.next()) {
        QString text = QString("账单：%1 元")
                           .arg(QString::number(query.value(0).toInt() / 100.0, 'f', 2));
        QString category = query.value(3).toString().trimmed();
        QString subject = query.value(4).toString().trimmed();
        QString counterpart = query.value(5).toString().trimmed();
        QString remarks = query.value(1).toString().trimmed();
        if (!category.isEmpty()) text += QString(" [%1]").arg(category);
        if (!counterpart.isEmpty()) text += QString(" -> %1").arg(counterpart);
        if (!subject.isEmpty()) text += QString("，账户：%1").arg(subject);
        if (!remarks.isEmpty()) text += QString("，备注：%1").arg(remarks);
        QString time = shortTime(query.value(2).toString());
        records.append({time.isEmpty() ? "??:??" : time, text});
    }

    // 保留旧版财务表，已有用户数据仍能显示
    query.prepare(
        "SELECT type, amount, category, note, created_at FROM finance_records "
        "WHERE date = :date ORDER BY created_at, id");
    query.bindValue(":date", isoDate);
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return records;
    }
    while (query.next()) {
        QString text = QString("旧版财务：%1 %2 [%3]")
                           .arg(query.value(0).toString().trimmed(),
                                QString::number(query.value(1).toDouble(), 'f', 2),
                                query.value(2).toString().trimmed());
        QString note = query.value(3).toString().trimmed();
        if (!note.isEmpty()) {
            text += QString("，备注：%1").arg(note);
        }
        records.append({shortTime(query.value(4).toString()), text});
    }

    return records;
}
