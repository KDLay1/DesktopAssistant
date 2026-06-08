#ifndef FINANCEPAGE_H
#define FINANCEPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

class FinacePage : public QWidget
{
    Q_OBJECT

public:
    explicit FinacePage(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupConnection();

    QLabel *titleLabel;
    QLabel *summaryLabel;

    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *exportButton;

    QTableWidget *recordTable;
};

#endif // FINANCEPAGE_H
