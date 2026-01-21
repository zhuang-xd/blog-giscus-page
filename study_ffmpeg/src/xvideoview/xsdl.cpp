#include "xsdl.h"
#include "SDL3/SDL.h"

extern "C" {
#include <libavcodec/codec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <iostream>
using namespace std;

XSDL::XSDL()
{

}

XSDL::XSDL(int w, int h, void* win_id, int fmt)
{
	Scale(w,h);

	// 初始化SDL接口
	static bool is_first_init = true;
	if (is_first_init) {
		int re = SDL_Init(SDL_INIT_VIDEO);
		if (re < 0) {
			cout << "SDL init failed" << endl;
		}
		else {
			cout << "SDL init ok" << endl;
		}
		is_first_init = false;
	}

	// 创建窗口、渲染器、材质
	if (!win_id) 
	{
		window_ = SDL_CreateWindow("",w,h, SDL_WINDOW_RESIZABLE);
	}
	else 
	{	
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, win_id);
		window_ = SDL_CreateWindowWithProperties(props);
	}

	render_ = SDL_CreateRenderer(window_, NULL);

	// 添加图片到纹理
	SDL_PixelFormat sdl_format;
	switch (fmt)
	{
	case XVideoView::YUV420P:
		sdl_format = SDL_PIXELFORMAT_IYUV;
		break;
	case XVideoView::NV12:
		sdl_format = SDL_PIXELFORMAT_NV12;
		break;
	case XVideoView::RGB:
		sdl_format = SDL_PIXELFORMAT_RGB24;
		break;
	case XVideoView::ARGB:
		sdl_format = SDL_PIXELFORMAT_ARGB32; // 这里不能选ARGB8888，会出现色差问题
		break;
	case XVideoView::RGBA:
		sdl_format = SDL_PIXELFORMAT_RGBA32;
		break;
	case XVideoView::BGRA:
		sdl_format = SDL_PIXELFORMAT_BGRA32;
		break;
	default:
		break;
	}

	texture_ = SDL_CreateTexture(render_,
		sdl_format,
		SDL_TEXTUREACCESS_STREAMING,
		w,h
	);

	// 创建AVFrame
	frame_ = av_frame_alloc();
	frame_->width = w;
	frame_->height = h;
	frame_->format = (AVPixelFormat)fmt;

	// 添加图片到纹理
	switch ((PixFormat)frame_->format)
	{
	case XVideoView::YUV420P:
		frame_->linesize[0] = frame_->width;
		frame_->linesize[1] = frame_->width / 2;
		frame_->linesize[2] = frame_->width / 2;
		break;
	case XVideoView::RGB:
		frame_->linesize[0] = frame_->width * 3;
		break;
	case XVideoView::ARGB:
		frame_->linesize[0] = frame_->width * 4;
		break;
	case XVideoView::RGBA:
	case XVideoView::BGRA:
		frame_->linesize[0] = frame_->width * 4;
		break;
	default:
		break;
	}

	av_frame_get_buffer(frame_, 0);
}

bool XSDL::DrawFrame()
{
	if (!frame_ || !frame_->data[0]) return false;

	// 添加图片到纹理
	switch ((PixFormat)frame_->format)
	{
	case XVideoView::YUV420P:
		SDL_UpdateYUVTexture(
			texture_,
			NULL,
			frame_->data[0], frame_->linesize[0],
			frame_->data[1], frame_->linesize[1],
			frame_->data[2], frame_->linesize[2]
		);
		break;
	case XVideoView::RGB:
		SDL_UpdateTexture(texture_,NULL,frame_->data[0], frame_->linesize[0]);
		break;
	case XVideoView::ARGB:
		SDL_UpdateTexture(texture_,NULL,frame_->data[0], frame_->linesize[0]);
		break;
	case XVideoView::RGBA:
	case XVideoView::BGRA:
		SDL_UpdateTexture(texture_,NULL,frame_->data[0], frame_->linesize[0]);
		break;
	default:
		break;
	}

	// 清空渲染器、材质拷贝到渲染器、显示
	SDL_RenderClear(render_);
	SDL_FRect dstrect = { 0,0,scale_w_,scale_h_ };
	SDL_RenderTexture(render_, texture_, NULL, &dstrect);
	SDL_RenderPresent(render_);

	return true;
}

bool XSDL::DrawFrame(
	unsigned char *data_y, int linesize_y,
	unsigned char *data_u, int linesize_u,
	unsigned char *data_v, int linesize_v)
{
	if (linesize_y <= 0 || linesize_u <= 0 || linesize_v <= 0) return false;

	// 添加图片到纹理
	SDL_UpdateYUVTexture(
		texture_,
		NULL,
		data_y, linesize_y,
		data_u, linesize_u,
		data_v, linesize_v
	);

	// 清空渲染器、材质拷贝到渲染器、显示
	SDL_RenderClear(render_);
	SDL_FRect dstrect = { 0,0,scale_w_,scale_h_ };
	SDL_RenderTexture(render_, texture_, NULL, &dstrect);
	SDL_RenderPresent(render_);

	return true;
}

bool XSDL::DrawFrame(unsigned char* data, int linesize)
{
	if (data == nullptr || linesize <= 0) return false;

	// 添加图片到纹理
	SDL_UpdateTexture(texture_,NULL,data,linesize);

	// 清空渲染器、材质拷贝到渲染器、显示
	SDL_RenderClear(render_);
	SDL_FRect dstrect = { 0,0,scale_w_,scale_h_ };
	SDL_RenderTexture(render_, texture_, NULL, &dstrect);
	SDL_RenderPresent(render_);

	return true;
}

bool XSDL::Close()
{
	if (!window_ || !render_ || !texture_ || !frame_)
	{
		cout << "close failed !";
		return false;
	}

	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(render_);
	SDL_DestroyTexture(texture_);
	av_frame_free(&frame_);

	cout << "close ok !";

	return true;
}