#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;
class QStackedWidget;
class PomodoroStatsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUi();
    void setupPages();
    void setupConnections();
    void setupStyle();

    QListWidget *navList;
    QStackedWidget *stackedWidget;
    PomodoroStatsPage *pomodoroStatsPage;
};

#endif // MAINWINDOW_H
