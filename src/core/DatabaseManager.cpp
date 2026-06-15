#include "DatabaseManager.h"

#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>

#include "models/CategoryID.h"

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

    QString createBillsTable = R"(
        CREATE TABLE IF NOT EXISTS bills (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            day_number INTEGER NOT NULL,
            is_refund INTEGER NOT NULL,
            category_id INTEGER NOT NULL,
            subject_id INTEGER NOT NULL,
            counterpart_id INTEGER NOT NULL,
            amount INTEGER NOT NULL,
            remarks TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    QString createCounterpartDict =
        "CREATE TABLE IF NOT EXISTS dict_counterpart (id INTEGER PRIMARY KEY, name TEXT);";
    QString createSubjectDict =
        "CREATE TABLE IF NOT EXISTS dict_subject (id INTEGER PRIMARY KEY, name TEXT);";
    QString createCategoryDict =
        "CREATE TABLE IF NOT EXISTS dict_category (id INTEGER PRIMARY KEY, name TEXT);";
    QString createPomodoroTable = R"(
        CREATE TABLE IF NOT EXISTS pomodoro_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_name TEXT NOT NULL,
            duration_minutes INTEGER NOT NULL,
            start_time DATETIME NOT NULL,
            end_time DATETIME NOT NULL,
            record_date DATE NOT NULL
        );
    )";

    if (!execute(createBillsTable)
        || !execute(createCounterpartDict)
        || !execute(createSubjectDict)
        || !execute(createCategoryDict)
        || !execute(createPomodoroTable)) {
        return false;
    }

    if (loadDict("dict_counterpart").isEmpty()) {
        saveDictItem("dict_counterpart", 101, "美团外卖");
        saveDictItem("dict_counterpart", 102, "星巴克");
        saveDictItem("dict_counterpart", 103, "公司财务部");
        saveDictItem("dict_counterpart", 104, "湖滨食堂");

        saveDictItem("dict_subject", 0xFF01, "招商银行储蓄卡");
        saveDictItem("dict_subject", 0xFF02, "微信余额");
        saveDictItem("dict_subject", 0xFF03, "支付宝");

        saveDictItem("dict_category", CategoryID(false, 1, false, 1).id(), "餐饮美食");
        saveDictItem("dict_category", CategoryID(false, 2, false, 1).id(), "交通出行");
        saveDictItem("dict_category", CategoryID(true, 1, false, 1).id(), "工资收入");
    }

    return execute(createFinanceTable)
           && execute(createCourseTaskTable)
           && execute(createWorkSessionTable)
           && execute(createLifeEventTable);
}

bool DatabaseManager::saveDictItem(const QString& tableName, quint16 id, const QString& name)
{
    QSqlQuery q(instance().database());
    q.prepare(QString("INSERT OR REPLACE INTO %1 (id, name) VALUES (:id, :name)").arg(tableName));
    q.bindValue(":id", id);
    q.bindValue(":name", name);
    return q.exec();
}

QMap<quint16, QString> DatabaseManager::loadDict(const QString& tableName)
{
    QMap<quint16, QString> result;
    QSqlQuery q(instance().database());
    if (q.exec(QString("SELECT id, name FROM %1").arg(tableName))) {
        while (q.next()) {
            result.insert(static_cast<quint16>(q.value("id").toInt()), q.value("name").toString());
        }
    }
    return result;
}

bool DatabaseManager::addBill(const Bill& bill)
{
    QSqlQuery q(instance().database());
    q.prepare("INSERT INTO bills "
              "(day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks) "
              "VALUES (:day, :refund, :cat, :sub, :counter, :amount, :remarks)");
    q.bindValue(":day", bill.dayNumber());
    q.bindValue(":refund", bill.isRefund() ? 1 : 0);
    q.bindValue(":cat", bill.categoryId());
    q.bindValue(":sub", bill.subjectId());
    q.bindValue(":counter", bill.counterpartId());
    q.bindValue(":amount", static_cast<int>(bill.amount()));
    q.bindValue(":remarks", bill.remarks());
    return q.exec();
}

bool DatabaseManager::deleteBill(int billId)
{
    QSqlQuery q(instance().database());
    q.prepare("DELETE FROM bills WHERE id = :id");
    q.bindValue(":id", billId);
    return q.exec() && q.numRowsAffected() > 0;
}

bool DatabaseManager::getBill(int billId, Bill& bill)
{
    QSqlQuery q(instance().database());
    q.prepare("SELECT id, day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks "
              "FROM bills WHERE id = :id");
    q.bindValue(":id", billId);
    if (!q.exec() || !q.next()) {
        return false;
    }

    bill = Bill(
        q.value("day_number").toInt(),
        q.value("is_refund").toInt() != 0,
        static_cast<quint16>(q.value("category_id").toInt()),
        static_cast<quint16>(q.value("subject_id").toInt()),
        static_cast<quint16>(q.value("counterpart_id").toInt()),
        MoneyRecord(q.value("amount").toInt()),
        q.value("remarks").toString(),
        q.value("id").toInt());
    return true;
}

QList<Bill> DatabaseManager::getAllBills()
{
    QList<Bill> result;
    QSqlQuery q(instance().database());
    q.prepare("SELECT id, day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks "
              "FROM bills ORDER BY day_number DESC, id DESC");
    if (!q.exec()) {
        return result;
    }

    while (q.next()) {
        result.append(Bill(
            q.value("day_number").toInt(),
            q.value("is_refund").toInt() != 0,
            static_cast<quint16>(q.value("category_id").toInt()),
            static_cast<quint16>(q.value("subject_id").toInt()),
            static_cast<quint16>(q.value("counterpart_id").toInt()),
            MoneyRecord(q.value("amount").toInt()),
            q.value("remarks").toString(),
            q.value("id").toInt()));
    }
    return result;
}

bool DatabaseManager::addPomodoroRecord(const QString& taskName, int durationMinutes,
                                        const QDateTime& startTime, const QDateTime& endTime)
{
    QSqlQuery q(instance().database());
    q.prepare("INSERT INTO pomodoro_records "
              "(task_name, duration_minutes, start_time, end_time, record_date) "
              "VALUES (:name, :duration, :start, :end, :date)");
    q.bindValue(":name", taskName);
    q.bindValue(":duration", durationMinutes);
    q.bindValue(":start", startTime.toString("yyyy-MM-dd HH:mm:ss"));
    q.bindValue(":end", endTime.toString("yyyy-MM-dd HH:mm:ss"));
    q.bindValue(":date", startTime.date().toString("yyyy-MM-dd"));
    return q.exec();
}

QList<QMap<QString, QVariant>> DatabaseManager::getPomodoroRecords() const
{
    QList<QMap<QString, QVariant>> result;
    QSqlQuery q(instance().database());
    q.prepare("SELECT id, task_name, duration_minutes, start_time, end_time, record_date "
              "FROM pomodoro_records ORDER BY start_time DESC");
    if (!q.exec()) {
        return result;
    }

    while (q.next()) {
        QMap<QString, QVariant> record;
        record["id"] = q.value("id");
        record["task_name"] = q.value("task_name");
        record["duration_minutes"] = q.value("duration_minutes");
        record["start_time"] = q.value("start_time");
        record["end_time"] = q.value("end_time");
        record["record_date"] = q.value("record_date");
        result.append(record);
    }
    return result;
}
