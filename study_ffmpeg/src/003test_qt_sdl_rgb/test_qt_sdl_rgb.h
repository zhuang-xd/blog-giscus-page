#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test_qt_sdl_rgb.h"

class QLabel;
class test_qt_sdl_rgb : public QMainWindow
{
    Q_OBJECT

public:
    test_qt_sdl_rgb(QWidget *parent = nullptr);
    ~test_qt_sdl_rgb();
    void timerEvent(QTimerEvent* ev) override;

private:
    Ui::test_qt_sdl_rgbClass ui;
	QLabel* m_sdl_label = ui.label;
};

