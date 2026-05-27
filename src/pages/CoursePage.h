#ifndef COURSEPAGE_H
#define COURSEPAGE_H

#include <QWidget>

class QPushButton;
class QLabel;
class QTableWidget;

class CoursePage : public QWidget
{
    Q_OBJECT

public:
    explicit CoursePage(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupConnection();

    QLabel *titleLabel;
    QLabel *summaryLabel;

    QPushButton *addclassButton;
    QPushButton *addtodoButton;
    QPushButton *deleteButton;
    QPushButton *exportButton;

    QTableWidget *taskTable;
};

#endif // COURSEPAGE_H
