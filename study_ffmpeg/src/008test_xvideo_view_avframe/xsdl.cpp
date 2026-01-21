#include "xsdl.h"
#include "SDL3/SDL.h"
#include "qmessagebox.h"
#include "qfile.h"
#include <iostream>
using namespace std;

XSDL::XSDL()
{
	// 1. 初始化SDL
	static bool isFirst = true;

	// 线程安全
	unique_lock<mutex> sdl_locker(m_mutex);
	//QMessageBox::information(nullptr, "Info", QString("isFirst = %1").arg(isFirst ? "true" : "false"));
	if (isFirst)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			QMessageBox::information(nullptr, "Error", "Failed to init SDL.");
		}
		isFirst = false;
	}
	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "linear");
}

bool XSDL::Init(int width, int height, PixFormat fmt, void* win_id)
{
	// 检查参数
	if (width <= 0 || height <= 0)
	{
		return false;
	}
	
	// 线程安全
	unique_lock<mutex> sdl_locker(m_mutex);
	m_scale_width = width;
	m_scale_height = height;

	// 2. 创建窗口、纹理、渲染器
	if (win_id == nullptr) 
	{
		m_sdl_window = SDL_CreateWindow("Default SDL Window", width,height, SDL_WINDOW_RESIZABLE);
	}
	else
	{
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER,win_id);
		m_sdl_window = SDL_CreateWindowWithProperties(props);
	}

	m_sdl_render =  SDL_CreateRenderer(m_sdl_window,NULL);
	SDL_PixelFormat sdl_fmt;
	switch (fmt)
	{
	case XvideoView::RGBA:
		sdl_fmt = SDL_PIXELFORMAT_RGBA8888;
		break;
	case XvideoView::ARGB:
		sdl_fmt = SDL_PIXELFORMAT_ARGB8888;
		break;
	case XvideoView::YUV420:
		sdl_fmt = SDL_PIXELFORMAT_IYUV;
		break;
	default:
		sdl_fmt = SDL_PIXELFORMAT_UNKNOWN;
		break;
	}
	m_sdl_texture = SDL_CreateTexture(m_sdl_render, sdl_fmt, SDL_TEXTUREACCESS_STREAMING, width, height);

	return true;
}

/// 通过yuv分量绘制frame
bool XSDL::Draw(const unsigned char* y_data, int y_pitch,
	const unsigned char* u_data, int u_pitch,
	const unsigned char* v_data, int v_pitch)
{
	// 检查参数
	if (y_pitch < 0 || u_pitch < 0 || v_pitch < 0)
	{
		return false;
	}
	// 线程安全
	unique_lock<mutex> sdl_locker(m_mutex);

	// 将数据添加到纹理
	SDL_UpdateYUVTexture(m_sdl_texture,
		NULL,
		y_data, y_pitch,
		u_data, u_pitch,
		v_data, v_pitch
	);
	
	// 清空渲染器、将复制纹理到渲染器、渲染
	SDL_RenderClear(m_sdl_render);
	SDL_FRect dstrect = { 0,0,m_scale_width,m_scale_height };
	SDL_RenderTexture(m_sdl_render,m_sdl_texture,NULL,&dstrect);
	SDL_RenderPresent(m_sdl_render);
}

bool XSDL::Draw(const void* data, int linesize)
{
	// 检查参数
	if (!data || linesize <= 0)
		return false;
	if (!m_sdl_render || !m_sdl_texture || !m_sdl_window)
		return false;

	// 线程安全
	unique_lock<mutex> sdl_locker(m_mutex);

	// 3. 加载帧图像数据到纹理
	SDL_UpdateTexture(m_sdl_texture, NULL, data, linesize);

	// 4. 清空渲染器、将纹理拷贝到渲染器、更新渲染器
	SDL_RenderClear(m_sdl_render);
	SDL_FRect dstrect = {0,0,m_scale_width,m_scale_height};
	SDL_RenderTexture(m_sdl_render,m_sdl_texture,NULL,&dstrect);
	SDL_RenderPresent(m_sdl_render);
	return true;
}

bool XSDL::IsExit()
{
	SDL_Event ev;

	SDL_WaitEventTimeout(&ev, 0);
	if (ev.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
		return true;
	else
		return false;
}

bool XSDL::Clear()
{
	// 清理SDL资源
	if (!m_sdl_window) 
	{
		SDL_DestroyWindow(m_sdl_window);
		m_sdl_window = nullptr;
	}
	if (!m_sdl_texture)
	{
		SDL_DestroyTexture(m_sdl_texture);
		m_sdl_texture = nullptr;
	}
	if (!m_sdl_render)
	{
		SDL_DestroyRenderer(m_sdl_render);
		m_sdl_render = nullptr;
	}

	return true;
}