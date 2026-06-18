#ifndef TIMELINEREPOSITORY_H
#define TIMELINEREPOSITORY_H

#include <QDate>
#include <QList>
#include <QString>

class QSqlDatabase;

struct TimelineRecord {
    QString time;
    QString text;
};

class TimelineRepository
{
public:
    // 创建生活事件和旧版学习记录表
    static bool initTables(QSqlDatabase db, QString *error = nullptr);

    // 汇总一天内各功能留下的记录
    static QList<TimelineRecord> recordsForDate(const QDate &date, QString *error = nullptr);
};

#endif // TIMELINEREPOSITORY_H
