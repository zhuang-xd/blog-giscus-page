#pragma once

#include <mutex>
#include <vector>

struct AVCodecContext;
struct AVFrame;
struct AVPacket;
class XEncode
{
public:
	static AVCodecContext* Create(int id);
	bool Open();
	bool SetContextOption(const char *key, const char* val);
	bool SetContextOption(const char* key, int val);
	bool SetContext(AVCodecContext* context);
	AVFrame* CreateFrame();
	//std::vector<AVPacket*> Encode(AVFrame* frame);
	AVPacket* Encode(AVFrame* frame);
	std::vector<AVPacket*> End();
	bool Close();

public:
	std::mutex mut_;
	AVCodecContext* context_ = nullptr;
	//const AVCodec* enc_ = nullptr;		// 对象只读的
};
