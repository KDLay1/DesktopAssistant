#include "Theme.h"

namespace Theme {

const Palette &palette()
{
    static const Palette theme{
        "#fff5d9",
        "#fff8f3",
        "#fffdf9",
        "#fef1e8",
        "#f6d6c3",
        "#ffd5d9",
        "#5f5654",
        "#8e8684",
        "#fcdfe5",
        "#ffd5d9",
        "#c9d4f7",
        "#6279ba",
        "#c9d4f7",
        "#acbfeb",
        "#778ccc",
        "#9adbc5",
        "#fdb78e",
        "#fa86a9",
        "#fce0b9",
        "#e7d3ed",
        "#71bcec",
    };
    return theme;
}

const QVector<QColor> &courseColors()
{
    static const QVector<QColor> colors{
        QColor("#fcdfe5"),
        QColor("#fce0b9"),
        QColor("#daf1ee"),
        QColor("#c9d4f7"),
        QColor("#e1f9e8"),
        QColor("#ffd5d9"),
        QColor("#feb2da"),
        QColor("#9adbc5"),
    };
    return colors;
}

QColor conflictCellColor()
{
    return QColor("#eeeaeb");
}

QColor outflowColor()
{
    return QColor("#d35b7e");
}

QColor inflowColor()
{
    return QColor("#778ccc");
}

QString appStyleSheet()
{
    const auto &p = palette();

    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            selection-background-color: %3;
            selection-color: %4;
        }

        QMainWindow {
            background-color: %5;
        }

        QListWidget#navList {
            background-color: %6;
            color: %2;
            border: none;
            border-right: 1px solid %7;
            font-size: 15px;
            padding-top: 16px;
            outline: 0;
        }

        QListWidget#navList::item {
            height: 46px;
            padding-left: 18px;
            border-radius: 14px;
            margin: 5px 10px;
        }

        QListWidget#navList::item:hover {
            background-color: %8;
        }

        QListWidget#navList::item:selected {
            background-color: %3;
            color: %9;
            font-weight: 700;
            border: none;
        }

        QLabel {
            background: transparent;
            color: %2;
        }

        QFrame#infoCard {
            background-color: %10;
            border: 1px solid %11;
            border-radius: 18px;
        }

        QFrame#infoCard:hover {
            border: 1px solid %3;
        }

        QFrame#infoCard[cardTone="rose"] {
            background-color: %6;
        }

        QFrame#infoCard[cardTone="peach"] {
            background-color: %12;
        }

        QFrame#infoCard[cardTone="mint"] {
            background-color: %13;
        }

        QFrame#infoCard[cardTone="lavender"] {
            background-color: %3;
        }

        QPushButton {
            background-color: %3;
            color: %9;
            border: 1px solid transparent;
            border-radius: 12px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: 600;
            min-height: 20px;
        }

        QPushButton:hover {
            background-color: %14;
        }

        QPushButton:pressed {
            background-color: %15;
            color: white;
        }

        QPushButton[buttonRole="peach"] {
            background-color: %16;
            color: white;
        }

        QPushButton[buttonRole="peach"]:hover {
            background-color: #fd8d6e;
        }

        QPushButton[buttonRole="peach"]:pressed {
            background-color: #ef836c;
        }

        QPushButton[buttonRole="mint"] {
            background-color: %13;
            color: #426760;
        }

        QPushButton[buttonRole="mint"]:hover {
            background-color: #a1dee0;
        }

        QPushButton[buttonRole="mint"]:pressed {
            background-color: %17;
            color: white;
        }

        QPushButton[buttonRole="rose"] {
            background-color: %18;
            color: white;
        }

        QPushButton[buttonRole="rose"]:hover {
            background-color: #ee84a8;
        }

        QPushButton[buttonRole="rose"]:pressed {
            background-color: #d35b7e;
        }

        QPushButton[buttonRole="sunny"] {
            background-color: %19;
            color: #806736;
        }

        QPushButton[buttonRole="sunny"]:hover {
            background-color: #fad354;
        }

        QPushButton[buttonRole="sunny"]:pressed {
            background-color: #f7cf83;
        }

        QPushButton[buttonRole="neutral"] {
            background-color: %20;
            color: #6f6271;
        }

        QPushButton[buttonRole="neutral"]:hover {
            background-color: #e7d3ed;
        }

        QPushButton[buttonRole="neutral"]:pressed {
            background-color: #c5a6c4;
            color: white;
        }

        QPushButton[buttonRole="lavender"] {
            background-color: %3;
            color: %9;
        }

        QPushButton[buttonRole="lavender"]:hover {
            background-color: %14;
        }

        QPushButton[buttonRole="lavender"]:pressed {
            background-color: %15;
            color: white;
        }

        QPushButton:disabled {
            background-color: #eeeaeb;
            color: #b1a8ab;
        }

        QLineEdit,
        QTextEdit,
        QPlainTextEdit,
        QDateEdit,
        QComboBox,
        QSpinBox,
        QDoubleSpinBox {
            background-color: %10;
            color: %2;
            border: 1px solid %11;
            border-radius: 10px;
            padding: 6px 10px;
            font-size: 14px;
        }

        QLineEdit:focus,
        QTextEdit:focus,
        QPlainTextEdit:focus,
        QDateEdit:focus,
        QComboBox:focus,
        QSpinBox:focus,
        QDoubleSpinBox:focus {
            border: 1px solid %14;
        }

        QComboBox::drop-down,
        QDateEdit::drop-down {
            border: none;
            width: 24px;
        }

        QComboBox QAbstractItemView {
            background-color: %10;
            border: 1px solid %11;
            selection-background-color: %3;
            selection-color: %9;
        }

        QTableView,
        QTableWidget {
            background-color: %10;
            alternate-background-color: %12;
            border: 1px solid %11;
            border-radius: 14px;
            gridline-color: %11;
            font-size: 14px;
        }

        QHeaderView::section {
            background-color: %19;
            color: %2;
            padding: 8px;
            border: none;
            border-bottom: 1px solid %7;
            font-weight: 700;
        }

        QGroupBox {
            font-size: 15px;
            font-weight: 700;
            border: 1px solid %11;
            border-radius: 16px;
            margin-top: 18px;
            padding-top: 16px;
            background-color: %10;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 8px;
            color: %9;
            background-color: %1;
        }

        QTabWidget::pane {
            border: 1px solid %11;
            border-radius: 16px;
            background-color: %10;
            top: -1px;
        }

        QTabBar::tab {
            background-color: %20;
            color: %2;
            padding: 10px 18px;
            margin-right: 6px;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            font-weight: 600;
        }

        QTabBar::tab:selected {
            background-color: %3;
            color: %9;
        }

        QTabBar::tab:hover:!selected {
            background-color: %8;
        }

        QDialog {
            background-color: %1;
        }
    )")
        .arg(p.pageBackground)
        .arg(p.textPrimary)
        .arg(p.navSelected)
        .arg(p.navSelectedText)
        .arg(p.shellBackground)
        .arg(p.navBackground)
        .arg(p.borderStrong)
        .arg(p.navHover)
        .arg(p.navSelectedText)
        .arg(p.surface)
        .arg(p.border)
        .arg(p.surfaceAlt)
        .arg(p.mint)
        .arg(p.primaryButtonHover)
        .arg(p.primaryButtonPressed)
        .arg(p.peach)
        .arg(p.info)
        .arg(p.rose)
        .arg(p.sunny)
        .arg(p.lavender);
}

QString dialogStyle()
{
    const auto &p = palette();
    return QString("QDialog { background-color: %1; } QLabel { color: %2; }")
        .arg(p.pageBackground, p.textPrimary);
}

QString subtleTextStyle()
{
    return QString("color: %1; font-size: 14px;").arg(palette().textSecondary);
}

QString footerTextStyle()
{
    return QString("color: %1; font-size: 12px; font-weight: 600;").arg(palette().textSecondary);
}

QString smartHintStyle()
{
    return QString("color: %1; font-size: 12px; font-weight: 700;").arg(palette().navSelectedText);
}

QString timerStatusStyle(const QString &colorHex)
{
    return QString("font-size: 22px; font-weight: 700; color: %1; margin-top: 30px;").arg(colorHex);
}

QString timerDisplayStyle(const QString &colorHex)
{
    return QString("font-size: 100px; font-weight: 700; color: %1; font-family: 'Segoe UI'; letter-spacing: 2px;")
        .arg(colorHex);
}

} // namespace Theme
