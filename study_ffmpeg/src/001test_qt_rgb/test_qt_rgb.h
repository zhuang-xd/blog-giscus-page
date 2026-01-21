#pragma once

#include <QtWidgets/QWidget>
#include "ui_test_qt_rgb.h"

class test_qt_rgb : public QWidget
{
    Q_OBJECT

public:
    test_qt_rgb(QWidget *parent = nullptr);
    ~test_qt_rgb();
	void paintEvent(QPaintEvent* event) override;
    void init_ui();

private:
    Ui::test_qt_rgbClass ui;
};

