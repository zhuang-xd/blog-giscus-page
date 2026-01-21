#pragma once

#include <QtWidgets/QWidget>
#include "ui_test_xvideo_view.h"
#include <qfile.h>

class QLabel;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class XvideoView;
class AVFrame;
class test_xvideo_view : public QWidget
{
    Q_OBJECT

public:
    test_xvideo_view(QWidget *parent = nullptr);
    ~test_xvideo_view();
	void timerEvent(QTimerEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
    void reverse_frame_buffer(unsigned char* src, unsigned char* dst);

    void init_ui();
    void init_func();

private:
    Ui::test_xvideo_viewClass ui;
    QLabel* m_label_sdl;
    int m_height;
    int m_width;
    SDL_Window* m_sdl_window;
    SDL_Renderer* m_sdl_render;
    SDL_Texture* m_sdl_texture;
    QFile m_file_sdl;
    XvideoView* m_view;
    XvideoView* m_view2;

    AVFrame* m_yuv_frame;
};

