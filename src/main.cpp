#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label("DesktopAssistant is running.");
    label.resize(400, 200);
    label.show();

    return app.exec();
}