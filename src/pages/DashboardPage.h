#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class QLabel;
class QFrame;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);
    void refreshData();

private:
    void setupUI();

    QFrame* createInfoCard(const QString &title,
                           const QString &value,
                           const QString &description,
                           QLabel *&valueLabel);

    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *taskValueLabel;
    QLabel *studyValueLabel;
    QLabel *expenseValueLabel;
    QLabel *timelineValueLabel;
};

#endif // DASHBOARDPAGE_H
