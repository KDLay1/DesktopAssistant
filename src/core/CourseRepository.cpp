#include "CourseRepository.h"

#include "DatabaseManager.h"

#include <QDateTime>
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

CourseRecord readCourse(const QSqlQuery &query)
{
    CourseRecord course;
    course.id = query.value("id").toInt();
    course.name = query.value("course_name").toString();
    course.weekday = query.value("weekday").toInt();
    course.startTime = query.value("start_time").toString();
    course.endTime = query.value("end_time").toString();
    course.location = query.value("location").toString();
    course.teacher = query.value("teacher").toString();
    course.startDate = query.value("start_date").toString();
    course.endDate = query.value("end_date").toString();
    return course;
}

CourseTaskRecord readTask(const QSqlQuery &query)
{
    CourseTaskRecord task;
    task.id = query.value("id").toInt();
    task.courseName = query.value("course_name").toString();
    task.title = query.value("task_title").toString();
    task.deadline = query.value("deadline").toString();
    task.priority = query.value("priority").toInt();
    task.status = query.value("status").toString();
    task.description = query.value("description").toString();
    return task;
}

} // namespace

bool CourseRepository::initTables(QSqlDatabase db, QString *error)
{
    QSqlQuery query(db);
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_name TEXT NOT NULL,
            weekday INTEGER NOT NULL,
            start_time TEXT NOT NULL,
            end_time TEXT NOT NULL,
            location TEXT,
            teacher TEXT,
            start_date TEXT,
            end_date TEXT,
            created_at TEXT NOT NULL,
            updated_at TEXT
        )
    )")) {
        saveError(error, query.lastError().text());
        return false;
    }

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS course_tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_name TEXT NOT NULL,
            task_title TEXT NOT NULL,
            description TEXT,
            deadline TEXT,
            priority INTEGER,
            status TEXT NOT NULL,
            created_at TEXT NOT NULL,
            updated_at TEXT
        )
    )")) {
        saveError(error, query.lastError().text());
        return false;
    }

    // 旧版 courses 表没有开课和结课日期
    if (!query.exec("PRAGMA table_info(courses)")) {
        saveError(error, query.lastError().text());
        return false;
    }

    bool hasStartDate = false;
    bool hasEndDate = false;
    while (query.next()) {
        QString name = query.value("name").toString();
        hasStartDate = hasStartDate || name == "start_date";
        hasEndDate = hasEndDate || name == "end_date";
    }

    if (!hasStartDate && !query.exec("ALTER TABLE courses ADD COLUMN start_date TEXT")) {
        saveError(error, query.lastError().text());
        return false;
    }
    if (!hasEndDate && !query.exec("ALTER TABLE courses ADD COLUMN end_date TEXT")) {
        saveError(error, query.lastError().text());
        return false;
    }
    return true;
}

bool CourseRepository::addCourses(const QList<CourseRecord> &courses, QString *error)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) {
        saveError(error, db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO courses(course_name, weekday, start_time, end_time, location, teacher, "
        "start_date, end_date, created_at, updated_at) "
        "VALUES(:name, :weekday, :start, :end, :location, :teacher, "
        ":start_date, :end_date, :created, :updated)");

    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    for (const CourseRecord &course : courses) {
        query.bindValue(":name", course.name);
        query.bindValue(":weekday", course.weekday);
        query.bindValue(":start", course.startTime);
        query.bindValue(":end", course.endTime);
        query.bindValue(":location", course.location);
        query.bindValue(":teacher", course.teacher);
        query.bindValue(":start_date", course.startDate);
        query.bindValue(":end_date", course.endDate);
        query.bindValue(":created", now);
        query.bindValue(":updated", now);

        if (!query.exec()) {
            saveError(error, query.lastError().text());
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        saveError(error, db.lastError().text());
        db.rollback();
        return false;
    }
    return true;
}

bool CourseRepository::addTask(const CourseTaskRecord &task, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "INSERT INTO course_tasks(course_name, task_title, description, deadline, priority, "
        "status, created_at, updated_at) "
        "VALUES(:course, :title, :description, :deadline, :priority, :status, :created, :updated)");
    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    query.bindValue(":course", task.courseName);
    query.bindValue(":title", task.title);
    query.bindValue(":description", task.description);
    query.bindValue(":deadline", task.deadline);
    query.bindValue(":priority", task.priority);
    query.bindValue(":status", task.status);
    query.bindValue(":created", now);
    query.bindValue(":updated", now);

    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return false;
    }
    return true;
}

bool CourseRepository::deleteTask(int id, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM course_tasks WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return false;
    }
    return true;
}

bool CourseRepository::deleteCourses(const QList<int> &ids, QString *error)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.transaction()) {
        saveError(error, db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM courses WHERE id = :id");
    for (int id : ids) {
        query.bindValue(":id", id);
        if (!query.exec()) {
            saveError(error, query.lastError().text());
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        saveError(error, db.lastError().text());
        db.rollback();
        return false;
    }
    return true;
}

bool CourseRepository::updateTaskStatus(int id, const QString &status, QString *error)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "UPDATE course_tasks SET status = :status, updated_at = :updated WHERE id = :id");
    query.bindValue(":status", status);
    query.bindValue(":updated", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", id);
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return false;
    }
    return true;
}

QList<CourseRecord> CourseRepository::coursesForDate(const QDate &date, QString *error)
{
    QList<CourseRecord> courses;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT * FROM courses "
        "WHERE (start_date IS NULL OR start_date = '' OR start_date <= :date) "
        "AND (end_date IS NULL OR end_date = '' OR end_date >= :date) "
        "ORDER BY weekday, start_time");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return courses;
    }
    while (query.next()) {
        courses.append(readCourse(query));
    }
    return courses;
}

QList<CourseRecord> CourseRepository::allCourses(QString *error)
{
    QList<CourseRecord> courses;
    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec("SELECT * FROM courses ORDER BY weekday, start_time")) {
        saveError(error, query.lastError().text());
        return courses;
    }
    while (query.next()) {
        courses.append(readCourse(query));
    }
    return courses;
}

QList<CourseTaskRecord> CourseRepository::allTasks(QString *error)
{
    QList<CourseTaskRecord> tasks;
    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(
        "SELECT * FROM course_tasks ORDER BY status DESC, deadline ASC, priority ASC")) {
        saveError(error, query.lastError().text());
        return tasks;
    }
    while (query.next()) {
        tasks.append(readTask(query));
    }
    return tasks;
}

QList<CourseTaskRecord> CourseRepository::tasksForExport(QString *error)
{
    QList<CourseTaskRecord> tasks;
    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec(
        "SELECT * FROM course_tasks ORDER BY status, deadline, priority")) {
        saveError(error, query.lastError().text());
        return tasks;
    }
    while (query.next()) {
        tasks.append(readTask(query));
    }
    return tasks;
}

QList<CourseTaskRecord> CourseRepository::tasksForDate(const QDate &date, QString *error)
{
    QList<CourseTaskRecord> tasks;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT * FROM course_tasks WHERE deadline LIKE :date "
        "ORDER BY deadline ASC, priority ASC");
    query.bindValue(":date", date.toString("yyyy-MM-dd") + "%");
    if (!query.exec()) {
        saveError(error, query.lastError().text());
        return tasks;
    }
    while (query.next()) {
        tasks.append(readTask(query));
    }
    return tasks;
}

int CourseRepository::unfinishedTaskCount()
{
    QSqlQuery query(DatabaseManager::instance().database());
    if (query.exec("SELECT COUNT(*) FROM course_tasks WHERE status != '已完成'")
        && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int CourseRepository::unfinishedTaskCount(const QDate &date)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT COUNT(*) FROM course_tasks "
        "WHERE status != '已完成' AND deadline LIKE :date");
    query.bindValue(":date", date.toString("yyyy-MM-dd") + "%");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int CourseRepository::courseCount(const QDate &date)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT COUNT(*) FROM courses "
        "WHERE weekday = :weekday "
        "AND (start_date IS NULL OR start_date = '' OR start_date <= :date) "
        "AND (end_date IS NULL OR end_date = '' OR end_date >= :date)");
    query.bindValue(":weekday", date.dayOfWeek());
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
