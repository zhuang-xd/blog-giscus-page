#pragma once
#include <fstream>

class AVFrame;
class XVideoView
{
public:
	enum PixFormat // 顺序与FFmepeg相同
	{
		YUV420P = 0,
		RGB = 2,
		ARGB = 25,
		RGBA = 26,
		BGRA = 28,
	};

	enum ViewType
	{
		SDL = 0
	};

	// 创建
	static XVideoView* Create(int w, int h, void* win_id, int fmt, ViewType type = SDL);

	// 打开文件
	bool Open(const char* file_path);
	// 读取帧
	void Read();
	// 绘制
	bool Draw();
	// 关闭
	virtual bool Close() = 0;
	// 缩放
	void Scale(int w, int h);
	
	// 获取fsp
	int render_fps() const;
protected:
	virtual bool DrawFrame() = 0;
protected:
	int scale_w_;
	int scale_h_;
	std::ifstream ifs_;
	AVFrame* frame_ = nullptr;
private:
	int file_size_;
	int render_fps_;				// 每一个视频窗口都能拥有独立的fps
	int start_;
	int cnt_;
};

