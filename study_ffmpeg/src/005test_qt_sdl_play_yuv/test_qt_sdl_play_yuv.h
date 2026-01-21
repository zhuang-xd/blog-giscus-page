#pragma once

#include <QtWidgets/QWidget>
#include "ui_test_qt_sdl_play_yuv.h"
//#include <fstream> // Á÷
#include <QFile>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class QFile;
class test_qt_sdl_play_yuv : public QWidget
{
	Q_OBJECT

public:
	test_qt_sdl_play_yuv(QWidget* parent = nullptr);
	~test_qt_sdl_play_yuv();
	void init_ui();
	void init_func();
	void timerEvent(QTimerEvent* event) override;

private:
	Ui::test_qt_sdl_play_yuvClass ui;
	QLabel* m_label_sdl;
	int m_pix_size = 4; // yuvÏñËØ×Ö½ÚÊý
	int m_height;
	int m_width;
	SDL_Window* m_sdl_window;
	SDL_Renderer* m_sdl_render;
	SDL_Texture* m_sdl_texture;
	QFile m_yuv_file;
	unsigned char* m_yuv_data;
};

