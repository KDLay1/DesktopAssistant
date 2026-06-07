/**
 * @file DatabaseManager.h
 * @brief 数据层 - 数据库管理器
 *
 * 本文件定义了 DatabaseManager 类，作为整个应用的数据库访问入口。
 * 它采用单例模式，封装了数据库的打开、初始化、执行 SQL 以及针对核心业务模型（如 Bill）的 CRUD 操作。
 * 该层位于 Data 层，依赖 Models 层，被 Services 层调用。
 *
 * @author tys
 * @date 2023-10-27
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QDateTime>
#include "models/bill.h" // 引入核心账单实体模型

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    QList<QMap<QString, QVariant>> getPomodoroRecords() const;
    static DatabaseManager& instance(); // 单例模式数据库
    static bool saveDictItem(const QString& tableName, quint16 id, const QString& name);
    static QMap<quint16, QString> loadDict(const QString& tableName);
    bool openDatabase(const QString& dbPath); // 打开数据库
    bool initTables(); // 初始化
    bool addPomodoroRecord(const QString& taskName, int durationMinutes, const QDateTime& startTime, const QDateTime& endTime);
    QSqlDatabase database() const;

    bool execute(const QString& sql);
    QSqlQuery query(const QString& sql);

    // ==========================================
    // 供 services/BillManager 调用的静态 CRUD 接口
    // ==========================================
    static bool addBill(const Bill& bill);
    static bool deleteBill(int billId);
    static bool getBill(int billId, Bill& bill);
    static QList<Bill> getAllBills();

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H