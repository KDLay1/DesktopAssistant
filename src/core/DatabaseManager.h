#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QVariant>

#include "models/bill.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance(); //单例模式数据库

    bool openDatabase(const QString& dbPath); //打开数据库
    bool initTables(); //初始化

    QSqlDatabase database() const;

    bool execute(const QString& sql);
    QSqlQuery query(const QString& sql);

    static bool saveDictItem(const QString& tableName, quint16 id, const QString& name);
    static QMap<quint16, QString> loadDict(const QString& tableName);

    static bool addBill(const Bill& bill);
    static bool deleteBill(int billId);
    static bool getBill(int billId, Bill& bill);
    static QList<Bill> getAllBills();

    bool addPomodoroRecord(const QString& taskName, int durationMinutes,
                           const QDateTime& startTime, const QDateTime& endTime);
    QList<QMap<QString, QVariant>> getPomodoroRecords() const;

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
