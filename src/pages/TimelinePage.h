#ifndef TIMELINEPAGE_H
#define TIMELINEPAGE_H

#include <QWidget>

class QLabel;
class QPushButton;
class QTextEdit;
class QDateEdit;

class TimelinePage : public QWidget
{
    Q_OBJECT

public:
    explicit TimelinePage(QWidget *parent = nullptr);

private:
    void setupUi();
    void setupConnections();

private:
    QLabel *titleLabel;
    QLabel *descriptionLabel;

    QDateEdit *dateEdit;

    QPushButton *generateButton;
    QPushButton *exportButton;

    QTextEdit *timelineTextEdit;
};

#endif