#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//前向声明, 减少.h文件中的头文件依赖
class QListWidget;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUi();
    void setupPages();
    void setupConnections();

    QListWidget *navList;
    QStackedWidget *stackedWidget;
};

#endif // MAINWINDOW_H
