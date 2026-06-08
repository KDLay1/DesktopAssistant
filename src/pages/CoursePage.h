#ifndef COURSEPAGE_H
#define COURSEPAGE_H
#include <QColor>
#include <QWidget>
#include <QDate>

class QPushButton;
class QLabel;
class QTableWidget;
class QTabWidget;
class QDateEdit;

class CoursePage : public QWidget
{
    Q_OBJECT

public:
    explicit CoursePage(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupConnection();

    void addCourse();
    void addTask();
    void deleteSelectedItem();
    void toggleTaskStatus();
    void exportToCSV();

    void refreshScheduleTable(const QDate &date = QDate::currentDate());
    void refreshTaskTable();
    void updateSummary();

    QString priorityText(int priority) const;
    int priorityValue(const QString &priority) const;
    QString weekdayText(int weekday) const;
    int timeToRow(const QString &time) const;
    int endTimeToRow(const QString &time) const;
    QColor courseColor(const QString &courseName) const;
private:
    QLabel *titleLabel;
    QLabel *summaryLabel;

    QPushButton *addclassButton;
    QPushButton *addtodoButton;
    QPushButton *deleteButton;
    QPushButton *finishButton;
    QPushButton *exportButton;
    QPushButton *todayButton;
    QPushButton *nextWeekButton;
    QDateEdit *scheduleDateEdit;
    QTabWidget *tabWidget;
    QTableWidget *scheduleTable;
    QTableWidget *taskTable;
};

#endif // COURSEPAGE_H
