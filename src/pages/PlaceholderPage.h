#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>
#include <QString>

class QLabel;

class PlaceholderPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceholderPage(const QString &title,
                             const QString &description,
                             QWidget *parent = nullptr);

private:
    QLabel *titleLabel;
    QLabel *descriptionLabel;
};

#endif // PLACEHOLDERPAGE_H
