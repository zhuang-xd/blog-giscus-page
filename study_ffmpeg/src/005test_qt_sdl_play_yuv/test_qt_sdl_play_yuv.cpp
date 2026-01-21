#include "test_qt_sdl_play_yuv.h"
#include <SDL3/SDL.h>
#include "qtimer.h"
#include <qmessagebox.h>
#include <QFile>

#define YUV_VIDEO_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\src\\005test_qt_sdl_play_yuv\\assets\\640_360_30.yuv"

test_qt_sdl_play_yuv::test_qt_sdl_play_yuv(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init_ui();
	init_func();
	startTimer(16);
}

test_qt_sdl_play_yuv::~test_qt_sdl_play_yuv()
{
	
}

void test_qt_sdl_play_yuv::init_ui()
{
	m_label_sdl = ui.label;
	m_label_sdl->setFixedSize(640,360);
	m_height = m_label_sdl->height();
	m_width = m_label_sdl->width();
	// 打开yuv文件
	m_yuv_file.setFileName(YUV_VIDEO_PATH);
	if (!m_yuv_file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(this,"",m_yuv_file.errorString());
		return;
	}
}
void test_qt_sdl_play_yuv::init_func()
{
	// 1. 初始化sdl
	SDL_Init(SDL_INIT_VIDEO);
	// 2. 创建窗口、纹理、渲染器
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, (void*)m_label_sdl->winId());
	m_sdl_window = SDL_CreateWindowWithProperties(props);
	m_sdl_render = SDL_CreateRenderer(m_sdl_window, NULL);
	// yuv格式
	m_sdl_texture = SDL_CreateTexture(m_sdl_render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);

	//m_sdl_texture = SDL_CreateTexture(m_sdl_render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
}

void test_qt_sdl_play_yuv::timerEvent(QTimerEvent* event)
{
	// 3. 读取yuv视频, 添加到纹理
	int frame_size = m_width * m_height * 3/2; // yuv420p
	QByteArray frame_buffer = m_yuv_file.read(frame_size); // 读取1帧yuv数据到buffer
	SDL_UpdateTexture(m_sdl_texture, NULL, frame_buffer.data(), 
		m_width // 行字节数
	); // 更新纹理

	// 3. 添加图片数据到内存, 图片添加到纹理
	//static int tmp = 255;
	//int pixels_len = m_height * m_width * m_pix_size;
	//unsigned char* pixels = new unsigned char[pixels_len];
	//for (int i = 0; i < m_height; i++)
	//{
	//	for (int j = 0; j < m_width; j++)
	//	{
	//		int index = i * m_width * m_pix_size + j * m_pix_size;
	//		//pixels[index] = 0;			// B
	//		//pixels[index + 1] = 0;		// G 
	//		//pixels[index + 2] = 255;	// R
	//		pixels[index + 3] = tmp;	// A
	//	}
	//}
	//tmp--;
	//if (tmp < 0)
	//	tmp = 255;
	//SDL_UpdateTexture(m_sdl_texture, NULL, pixels, m_width * m_pix_size);
	
	// 4. 清空渲染器、拷贝纹理到渲染器、渲染器显示
	SDL_RenderClear(m_sdl_render);
	SDL_RenderTexture(m_sdl_render, m_sdl_texture, NULL, NULL);
	SDL_RenderPresent(m_sdl_render);
}
