#pragma once

/**
	工厂模式接口
	1. 定义：
		- 包含枚举类型
		- 函数是static（不需要实现就可以通过类名调用）
		- 函数是纯虚函数（只定义接口，需要派生类去具体实现）
	2. 作用：封装隐藏实现，可以后期扩展内部实现
*/

#include <mutex>

class XvideoView
{
public:
	enum PixFormat
	{
		RGBA = 0,
		ARGB,
		YUV420
	};

	enum ViewType
	{
		SDL = 0
	};

	static XvideoView* Create(ViewType type = SDL);
	virtual bool Init(int width, int height, PixFormat fmt, void* win_id) = 0;
	virtual bool Draw(const void *data, int linesize) = 0;
	virtual bool Clear() = 0;
	virtual bool IsExit() = 0;

	void Scale(int width, int height);
protected:
	int m_scale_width;
	int m_scale_height;
	std::mutex m_mutex;
};

