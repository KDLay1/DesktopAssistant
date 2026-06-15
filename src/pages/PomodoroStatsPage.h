#ifndef POMODOROSTATSPAGE_H
#define POMODOROSTATSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class PomodoroStatsPage : public QWidget {
    Q_OBJECT
public:
    explicit PomodoroStatsPage(QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();
    QTableWidget *recordsTable;
    QLabel *totalCountLabel;
    QLabel *totalDurationLabel;
    QPushButton *refreshButton;
};

#endif