#pragma once

#include <QtWidgets/QWidget>
#include "ui_test_xvideo_view.h"
#include <qfile.h>
#include <thread>

class QSlider;
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
	void resizeEvent(QResizeEvent* event) override;
    void reverse_frame_buffer(unsigned char* src, unsigned char* dst);

    void init_ui();
    void init_func();

signals:
    void UpdateFrame();
    void UpdateFps();

public slots:
    void OnUpdateFrame();

private:
    Ui::test_xvideo_viewClass ui;
    int m_height;
    int m_width;
    SDL_Window* m_sdl_window;
    SDL_Renderer* m_sdl_render;
    SDL_Texture* m_sdl_texture;
    XvideoView* m_view;
    AVFrame* m_yuv_frame;
    QSlider* m_slider_fps;
    QLabel* m_label_sdl;
    QLabel* m_label_slider_fps;
    QLabel* m_label_fps;
    QFile m_file_sdl;                   // yuv文件
    int m_fps;
    int m_fps_set = 60;
    std::thread m_thread_render;        // 渲染线程
    bool m_is_exited = false;           // 渲染线程 退出标志
    clock_t m_start_time = 0;           // 每秒刷新fps用
};

