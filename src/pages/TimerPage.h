#ifndef TIMERPAGE_H
#define TIMERPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QLineEdit;
class QTableWidget;

class TimerPage : public QWidget
{
    Q_OBJECT

public:
    explicit TimerPage(QWidget *parent = nullptr);

private:
    void setupUi();
    void setupConnections();

private:
    QLabel *titleLabel;
    QLabel *timeLabel;
    QLabel *summaryLabel;

    QLineEdit *taskNameEdit;

    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *finishButton;

    QTableWidget *sessionTable;
};

#endif