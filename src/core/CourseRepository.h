#ifndef COURSEREPOSITORY_H
#define COURSEREPOSITORY_H

#include <QDate>
#include <QList>
#include <QString>

class QSqlDatabase;

struct CourseRecord {
    int id = -1;
    QString name;
    int weekday = 1;
    QString startTime;
    QString endTime;
    QString location;
    QString teacher;
    QString startDate;
    QString endDate;
};

struct CourseTaskRecord {
    int id = -1;
    QString courseName;
    QString title;
    QString deadline;
    int priority = 2;
    QString status;
    QString description;
};

class CourseRepository
{
public:
    // 建表和补充旧数据库缺少的日期字段
    static bool initTables(QSqlDatabase db, QString *error = nullptr);

    // 一门课可能一周上多次，所以这里一次保存多条
    static bool addCourses(const QList<CourseRecord> &courses, QString *error = nullptr);
    static bool addTask(const CourseTaskRecord &task, QString *error = nullptr);
    static bool deleteTask(int id, QString *error = nullptr);
    static bool deleteCourses(const QList<int> &ids, QString *error = nullptr);
    static bool updateTaskStatus(int id, const QString &status, QString *error = nullptr);

    static QList<CourseRecord> coursesForDate(const QDate &date, QString *error = nullptr);
    static QList<CourseRecord> allCourses(QString *error = nullptr);
    static QList<CourseTaskRecord> allTasks(QString *error = nullptr);
    static QList<CourseTaskRecord> tasksForExport(QString *error = nullptr);
    static QList<CourseTaskRecord> tasksForDate(const QDate &date, QString *error = nullptr);

    static int unfinishedTaskCount();
    static int unfinishedTaskCount(const QDate &date);
    static int courseCount(const QDate &date);
};

#endif // COURSEREPOSITORY_H
