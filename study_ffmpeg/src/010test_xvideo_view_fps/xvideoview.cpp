#include "xvideoview.h"
#include "xsdl.h"
#include "libavutil/avutil.h"
#include "libavcodec/codec.h"

// 静态工厂方法，创建XvideoView对象，默认SDL

/// <summary>
/// 创建实例，默认是SDL
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
XvideoView* XvideoView::Create(ViewType type)
{
	switch (type)
	{
	case XvideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}

	return nullptr;
}

void XvideoView::Scale(int width, int height)
{
	m_scale_width = width;
	m_scale_height = height;
}

bool XvideoView::DrawFrame(AVFrame* frame)
{
	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]
		);
		break;
	case AV_PIX_FMT_ARGB:
		return Draw(
			frame->data[0], frame->linesize[0]
		);
		break;
	}

	return false;
}