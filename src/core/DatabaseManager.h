#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

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

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H