#include "test_qt_sdl_merge.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    test_qt_sdl_merge window;
    window.show();
    return app.exec();
}
