#pragma once
#include <mutex>
#include <vector>

#ifdef XVIDEO_VIEW_EXPORTS
#define XVIDEO_VIEW_API __declspec(dllexport) 
#else
#define XVIDEO_VIEW_API __declspec(dllimport) 
#endif

void PrintErr(int re);

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
class XVIDEO_VIEW_API XCode
{
public:
	static AVCodecContext* Create(int id, bool is_enc);
	bool Open();
	bool SetContextOption(const char* key, const char* val);
	bool SetContextOption(const char* key, int val);
	bool SetContext(AVCodecContext* context);
	AVFrame* CreateFrame();
	bool Close();

public:
	std::mutex mut_;
	AVCodecContext* context_ = nullptr;
	AVFrame* frame_ = nullptr;
};

