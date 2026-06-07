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
    // ==========================================
    // 1. 核心改造：账单表完全对齐 Bill 实体模型
    // ==========================================
    QString createFinanceTable = R"(
        CREATE TABLE IF NOT EXISTS finance_records (
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
    
    // 番茄钟核心记录表，这里定义为标准 SQL 格式，并交由安全的 execute 管道统一执行
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

    QString createCounterpartDict = "CREATE TABLE IF NOT EXISTS dict_counterpart (id INTEGER PRIMARY KEY, name TEXT);";
    QString createSubjectDict = "CREATE TABLE IF NOT EXISTS dict_subject (id INTEGER PRIMARY KEY, name TEXT);";
    QString createCategoryDict = "CREATE TABLE IF NOT EXISTS dict_category (id INTEGER PRIMARY KEY, name TEXT);";
    
    // 执行建表操作（确保它们绑定了 "main_connection" 数据库连接通道）
    execute(createCounterpartDict);
    execute(createSubjectDict);
    execute(createCategoryDict);
    execute(createPomodoroTable); // ✅ 修复：使用项目规范的 execute 方法代替没有数据库绑定的裸 query

    // 如果对手方字典为空，插入默认数据 (保证初次运行有数据可用)
    if (loadDict("dict_counterpart").isEmpty()) {
        saveDictItem("dict_counterpart", 101, "美团外卖");
        saveDictItem("dict_counterpart", 102, "星巴克");
        saveDictItem("dict_counterpart", 103, "公司财务部");
        saveDictItem("dict_counterpart", 104, "湖滨食堂");
        
        saveDictItem("dict_subject", 0xFF01, "招商银行储蓄卡");
        saveDictItem("dict_subject", 0xFF02, "微信余额");
        saveDictItem("dict_subject", 0xFF03, "支付宝");

        // 使用 CategoryID 生成合法 ID
        saveDictItem("dict_category", CategoryID(false, 1, false, 1).id(), "餐饮美食");
        saveDictItem("dict_category", CategoryID(false, 2, false, 1).id(), "交通出行");
        saveDictItem("dict_category", CategoryID(true, 1, false, 1).id(), "工资收入");
    }
    
    return execute(createFinanceTable)
           && execute(createCourseTaskTable)
           && execute(createWorkSessionTable)
           && execute(createLifeEventTable);
}

// ==========================================
// 核心改造：实现 BillManager 所需的 CRUD
// ==========================================

bool DatabaseManager::addBill(const Bill& bill)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO finance_records "
              "(day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks) "
              "VALUES (:day, :refund, :cat, :sub, :counter, :amt, :rem)");
              
    q.bindValue(":day", bill.dayNumber());
    q.bindValue(":refund", bill.isRefund() ? 1 : 0);
    q.bindValue(":cat", bill.categoryId());
    q.bindValue(":sub", bill.subjectId());
    q.bindValue(":counter", bill.counterpartId());
    q.bindValue(":amt", static_cast<int>(bill.amount())); 
    q.bindValue(":rem", bill.remarks());

    if (!q.exec()) {
        qDebug() << "Add bill failed:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::saveDictItem(const QString& tableName, quint16 id, const QString& name) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare(QString("INSERT OR REPLACE INTO %1 (id, name) VALUES (:id, :name)").arg(tableName));
    q.bindValue(":id", id);
    q.bindValue(":name", name);
    return q.exec();
}

QMap<quint16, QString> DatabaseManager::loadDict(const QString& tableName) {
    QMap<quint16, QString> map;
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec(QString("SELECT id, name FROM %1").arg(tableName))) {
        while (q.next()) {
            map.insert(static_cast<quint16>(q.value("id").toInt()), q.value("name").toString());
        }
    }
    return map;
}

bool DatabaseManager::deleteBill(int billId)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM finance_records WHERE id = :id");
    q.bindValue(":id", billId);
    
    if (!q.exec()) {
        qDebug() << "Delete bill failed:" << q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}

bool DatabaseManager::getBill(int billId, Bill& bill)
{
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks "
              "FROM finance_records WHERE id = :id");
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
        q.value("id").toInt()
    );
    
    return true;
}

QList<Bill> DatabaseManager::getAllBills()
{
    QList<Bill> list;
    QSqlQuery q(DatabaseManager::instance().database());
    
    q.prepare("SELECT id, day_number, is_refund, category_id, subject_id, counterpart_id, amount, remarks "
              "FROM finance_records ORDER BY day_number DESC, id DESC");
              
    if (!q.exec()) {
        qDebug() << "Get all bills failed:" << q.lastError().text();
        return list;
    }

    while (q.next()) {
        list.append(Bill(
            q.value("day_number").toInt(),
            q.value("is_refund").toInt() != 0,
            static_cast<quint16>(q.value("category_id").toInt()),
            static_cast<quint16>(q.value("subject_id").toInt()),
            static_cast<quint16>(q.value("counterpart_id").toInt()),
            MoneyRecord(q.value("amount").toInt()),
            q.value("remarks").toString(),
            q.value("id").toInt()
        ));
    }
    return list;
}

bool DatabaseManager::addPomodoroRecord(const QString& taskName, int durationMinutes, const QDateTime& startTime, const QDateTime& endTime) 
{
    // ✅ 已修复：绑定 active 数据库连接对象
    QSqlQuery query(DatabaseManager::instance().database()); 
    query.prepare("INSERT INTO pomodoro_records (task_name, duration_minutes, start_time, end_time, record_date) "
                  "VALUES (:name, :duration, :start, :end, :date)");
    
    query.bindValue(":name", taskName);
    query.bindValue(":duration", durationMinutes);
    query.bindValue(":start", startTime.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":end", endTime.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":date", startTime.date().toString("yyyy-MM-dd"));
    
    if (!query.exec()) {
        qDebug() << "番茄钟记录插入失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<QMap<QString, QVariant>> DatabaseManager::getPomodoroRecords() const {
    QList<QMap<QString, QVariant>> records;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT id, task_name, duration_minutes, start_time, end_time, record_date "
              "FROM pomodoro_records ORDER BY start_time DESC");
    
    if (q.exec()) {
        while (q.next()) {
            QMap<QString, QVariant> record;
            record["id"] = q.value("id").toInt();
            record["task_name"] = q.value("task_name").toString();
            record["duration_minutes"] = q.value("duration_minutes").toInt();
            record["start_time"] = q.value("start_time").toString();
            record["end_time"] = q.value("end_time").toString();
            record["record_date"] = q.value("record_date").toString();
            records.append(record);
        }
    }
    return records;
}