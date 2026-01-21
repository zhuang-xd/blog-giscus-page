#include "test_qt_rgb.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    test_qt_rgb window;
    window.show();
    return app.exec();
}