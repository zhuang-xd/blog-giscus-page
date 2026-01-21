#include "test_xvideo_view.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    test_xvideo_view window;
    window.show();
    return app.exec();
}
