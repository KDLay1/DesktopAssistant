#include "DatabaseManager.h"

#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

bool DatabaseManager::openDatabase(const QString& dbPath)
{
    if (QSqlDatabase::contains("main_connection")) {
        m_db = QSqlDatabase::database("main_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
    }

    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "Database opened:" << dbPath;
    return true;
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}

bool DatabaseManager::execute(const QString& sql)
{
    QSqlQuery q(m_db);

    if (!q.exec(sql)) {
        qDebug() << "SQL execute failed:" << q.lastError().text();
        qDebug() << "SQL:" << sql;
        return false;
    }

    return true;
}

QSqlQuery DatabaseManager::query(const QString& sql)
{
    QSqlQuery q(m_db);

    if (!q.exec(sql)) {
        qDebug() << "SQL query failed:" << q.lastError().text();
        qDebug() << "SQL:" << sql;
    }

    return q;
}

bool DatabaseManager::initTables()
{
    QString createFinanceTable = R"(
        CREATE TABLE IF NOT EXISTS finance_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL,
            amount REAL NOT NULL,
            category TEXT NOT NULL,
            note TEXT,
            date TEXT NOT NULL,
            created_at TEXT NOT NULL,
            updated_at TEXT
        );
    )";

    QString createCourseTaskTable = R"(
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
        );
    )";

    QString createWorkSessionTable = R"(
        CREATE TABLE IF NOT EXISTS work_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            category TEXT,
            start_time TEXT NOT NULL,
            end_time TEXT,
            duration_minutes INTEGER,
            note TEXT,
            created_at TEXT NOT NULL
        );
    )";

    QString createLifeEventTable = R"(
        CREATE TABLE IF NOT EXISTS life_events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            event_type TEXT NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            related_path TEXT,
            event_time TEXT NOT NULL,
            created_at TEXT NOT NULL
        );
    )";

    return execute(createFinanceTable)
           && execute(createCourseTaskTable)
           && execute(createWorkSessionTable)
           && execute(createLifeEventTable);
}