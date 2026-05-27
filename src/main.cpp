#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include "app/MainWindow.h"
#include "core/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dataDir + "/lifemate.db";

    if (!DatabaseManager::instance().openDatabase(dbPath)) {
        qDebug() << "Database open failed.";
        return -1;
    }

    if (!DatabaseManager::instance().initTables()) {
        qDebug() << "Database init failed.";
        return -1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}