#include "test_qt_sdl_rgb.h"
#include "SDL3/SDL.h"
#include "qdebug.h"

static SDL_Renderer* sdl_render = nullptr;
static SDL_Texture* sdl_texture = nullptr;
static SDL_Window* sdl_window = nullptr;
static int sdl_win_width = 0;
static int sdl_win_height = 0;

test_qt_sdl_rgb::test_qt_sdl_rgb(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_sdl_label = ui.label;
	ui.label->setFixedSize(800, 300);
	sdl_win_width = m_sdl_label->width();
	sdl_win_height = m_sdl_label->height();

	// 初始化SDL接口
	SDL_Init(SDL_INIT_VIDEO);

	// 创建嵌入qt的SDL窗口
	WId winId = m_sdl_label->winId();
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, (void*)winId);
	SDL_Window* window = SDL_CreateWindowWithProperties(props);

	sdl_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	// 创建渲染器
	sdl_render = SDL_CreateRenderer(sdl_window, NULL);

	// 创建纹理
	sdl_texture = SDL_CreateTexture(sdl_render, 
		SDL_PIXELFORMAT_ARGB8888, 
		SDL_TEXTUREACCESS_STREAMING, 
		sdl_win_width, 
		sdl_win_height
	);
	
	startTimer(16); //60fps
}

test_qt_sdl_rgb::~test_qt_sdl_rgb()
{
	SDL_DestroyWindow(sdl_window);
}

void test_qt_sdl_rgb::timerEvent(QTimerEvent* ev)
{
	// 创建图像数据
	static int tmp = 255;

	std::shared_ptr<unsigned char> rgb(new unsigned char[sdl_win_width * sdl_win_height * 4]);
	auto sdl_win_pixels = rgb.get();
	for (int i = 0; i < sdl_win_height; i++)
	{
		for (int j = 0; j < sdl_win_width; j++)
		{
			int index = sdl_win_width * 4 * i + j * 4;
			// 如果是argb8888格式，则通道顺序如下
			sdl_win_pixels[index] = 0;		// B
			sdl_win_pixels[index + 1] = 0;	// G
			sdl_win_pixels[index + 2] = 255;	// R
			sdl_win_pixels[index + 3] = tmp;	// A
		}
	}
	tmp--;
	if (tmp < 0) {
		tmp = 255;
	}

	// 加载图像为纹理
	SDL_UpdateTexture(sdl_texture, NULL, sdl_win_pixels, sdl_win_width * 4);

	// 清空渲染器
	SDL_RenderClear(sdl_render);

	// 复制纹理到渲染器
	SDL_FRect dstRect = { 0,0,sdl_win_width,sdl_win_height };
	//SDL_RenderCopy( // SDL3中过时
	SDL_RenderTexture( // SDL3新函数
		sdl_render,
		sdl_texture,
		NULL, // 原图坐标、尺寸
		&dstRect // 目标坐标、尺寸
	);

	// 更新窗口
	SDL_RenderPresent(sdl_render);
}