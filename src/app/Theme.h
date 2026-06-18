#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>
#include <QVector>

namespace Theme {

struct Palette {
    QString shellBackground;
    QString pageBackground;
    QString surface;
    QString surfaceAlt;
    QString border;
    QString borderStrong;
    QString textPrimary;
    QString textSecondary;
    QString navBackground;
    QString navHover;
    QString navSelected;
    QString navSelectedText;
    QString primaryButton;
    QString primaryButtonHover;
    QString primaryButtonPressed;
    QString mint;
    QString peach;
    QString rose;
    QString sunny;
    QString lavender;
    QString info;
};

const Palette &palette();
const QVector<QColor> &courseColors();
QColor conflictCellColor();
QColor outflowColor();
QColor inflowColor();

QString appStyleSheet();
QString dialogStyle();
QString subtleTextStyle();
QString footerTextStyle();
QString smartHintStyle();
QString timerStatusStyle(const QString &colorHex);
QString timerDisplayStyle(const QString &colorHex);

} // namespace Theme

#endif // THEME_H
