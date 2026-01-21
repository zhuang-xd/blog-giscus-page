#include "xvideoview.h"
#include "xsdl.h"

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