#include "xvideoview.h"
#include "xsdl.h"

extern "C" {
#include <libavcodec/codec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib,"SDL3.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")

#include <iostream>
using namespace std;

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}

XVideoView* XVideoView::Create(ViewType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}

	return nullptr;
}

XVideoView* XVideoView::Create(int w, int h, void* win_id, int fmt, ViewType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL(w, h, win_id, fmt);
		break;
	default:
		break;
	}

	return nullptr;
}

void XVideoView::Scale(int w, int h)
{
	scale_w_ = w;
	scale_h_ = h;
}

bool XVideoView::Open(const char *file_path)
{
	if (ifs_.is_open()) {
		ifs_.close();
	}

	ifs_.open(file_path, ios::binary);
	ifs_.seekg(0, ios::end);
	file_size_ = ifs_.tellg();
	ifs_.seekg(0, ios::beg);

	return true;
}

void XVideoView::Read()
{
	if (!frame_) {
		return;
	}

	// 循环读取
	if (file_size_ == ifs_.tellg())
		ifs_.seekg(0, ios::beg);

	// 把视频帧读到AVFrame
	switch (frame_->format)
	{
	case AV_PIX_FMT_YUV420P:
		ifs_.read((char*)frame_->data[0], frame_->width * frame_->height);
		ifs_.read((char*)frame_->data[1], frame_->width * frame_->height / 4);
		ifs_.read((char*)frame_->data[2], frame_->width * frame_->height / 4);
		break;
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_BGRA:
		ifs_.read((char*)frame_->data[0], frame_->width * frame_->height * 4);
		break;
	case AV_PIX_FMT_RGB24:
		ifs_.read((char*)frame_->data[0], frame_->width * frame_->height * 3);
		break;
	case AV_PIX_FMT_NV12:
		ifs_.read((char*)frame_->data[0], frame_->width * frame_->height * 3);
		break;
	default:
		break;
	}
}

bool XVideoView::Draw(
	unsigned char* data_y, int linesize_y,
	unsigned char* data_u, int linesize_u,
	unsigned char* data_v, int linesize_v)
{
	// 计算fps
	cnt_++;
	if (start_ <= 0)
	{
		start_ = clock();
	}
	else {
		if ((clock() - start_) / (CLOCKS_PER_SEC / 1000) >= 1000)
		{
			render_fps_ = cnt_;
			start_ = 0;
			cnt_ = 0;
		}
	}

	// 绘制这一帧图像
	DrawFrame(data_y, linesize_y, data_u, linesize_u, data_v, linesize_v);

	return true;
}

bool XVideoView::Draw(unsigned char* data, int linesize)
{
	// 计算fps
	cnt_++;
	if (start_ <= 0)
	{
		start_ = clock();
	}
	else {
		if ((clock() - start_) / (CLOCKS_PER_SEC / 1000) >= 1000)
		{
			render_fps_ = cnt_;
			start_ = 0;
			cnt_ = 0;
		}
	}

	// 绘制这一帧图像
	DrawFrame(data,linesize);

	return true;
}

bool XVideoView::Draw()
{
	// 计算fps
	cnt_++;
	if (start_ <= 0)
	{
		start_ = clock();
	}
	else {
		if ((clock() - start_) / (CLOCKS_PER_SEC / 1000) >= 1000)
		{
			render_fps_ = cnt_;
			start_ = 0;
			cnt_ = 0;
		}
	}

	// 绘制这一帧图像
	DrawFrame();

	return true;
}

int XVideoView::render_fps() const
{
	return render_fps_;
}