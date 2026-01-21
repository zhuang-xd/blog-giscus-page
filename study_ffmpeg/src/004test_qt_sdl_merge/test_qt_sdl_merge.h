#pragma once

#include <QtWidgets/QWidget>
#include "ui_test_qt_sdl_merge.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class test_qt_sdl_merge : public QWidget
{
    Q_OBJECT

public:
    test_qt_sdl_merge(QWidget *parent = nullptr);
    ~test_qt_sdl_merge();

    void init_ui();
    void init_func();

	void timerEvent(QTimerEvent* event) override;

private:
    Ui::test_qt_sdl_mergeClass ui;
	QLabel* m_label_sdl;
    int m_width;
    int m_height;
	int m_pixel_size; //单个像素的字节大小
	int m_pixels_len; //像素数据的总字节大小
    unsigned char* m_pixels;
	WId m_winId;

    SDL_Window* m_sdl_window;
    SDL_Renderer* m_sdl_render;
    SDL_Texture* m_sdl_texture;

    QImage m_img1;
    QImage m_img2;
};

