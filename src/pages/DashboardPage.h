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

private:
    void setupUI();

    QFrame* createInfoCard(const QString &title,
                           const QString &value,
                           const QString &description);

    QLabel *titleLabel;
    QLabel *subtitleLabel;
};

#endif // DASHBOARDPAGE_H
