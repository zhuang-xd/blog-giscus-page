#include "test_qt_sdl_play_yuv.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    test_qt_sdl_play_yuv window;
    window.show();
    return app.exec();
}
