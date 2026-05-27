#include "PlaceholderPage.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

PlaceholderPage::PlaceholderPage(const QString &title,
                                 const QString &description,
                                 QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel(title, this);
    descriptionLabel = new QLabel(description, this);

    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QFont descFont;
    descFont.setPointSize(11);
    descriptionLabel->setFont(descFont);

    titleLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addStretch();
}