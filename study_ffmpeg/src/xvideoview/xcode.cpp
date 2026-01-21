#include "xcode.h"

#include "xencode.h"
#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

using namespace std;

void PrintErr(int re)
{
	char buf[128];
	av_strerror(re, buf, sizeof(buf));
	cout << buf << endl;
}

AVCodecContext* XCode::Create(int id, bool is_enc)
{
	const AVCodec* code; // 对象只读的

	if (is_enc) {	
		code = avcodec_find_encoder((AVCodecID)id);	// 查找编码器
	} else {
		code = avcodec_find_decoder((AVCodecID)id);	// 查找解码器
	}
	if (!code)
	{
		cout << "avcodec_find_decoder failed" << endl;
		return nullptr;
	}

	// 创建上下文
	AVCodecContext* context = avcodec_alloc_context3(code);;
	if (!context)
	{
		cout << "avcodec_alloc_context3 failed" << endl;
		return nullptr;
	}

	return context;
}

bool XCode::SetContext(AVCodecContext* context)
{
	unique_lock <mutex> lock(mut_);
	//if (context_)
	//{
	//	avcodec_free_context(&context_); // 先释放原有的context
	//}	

	context_ = context;

	return true;
}

bool XCode::SetContextOption(const char* key, int val)
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return false;

	int re = av_opt_set_int(context_->priv_data, key, val, 0);
	if (re < 0) {
		cout << "SetContextOption failed ";
		PrintErr(re);
		return false;
	}

	return true;
}

bool XCode::SetContextOption(const char* key, const char* val)
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return false;

	int re = av_opt_set(context_->priv_data, "preset", "ultrafast", 0);
	//int re = av_opt_set(context_->priv_data,key,val,0);
	if (re < 0) {
		cout << "SetContextOption failed ";
		PrintErr(re);
		return false;
	}

	return true;
}

bool XCode::Open()
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return false;

	// 打开编码器
	int re = avcodec_open2(context_, NULL, NULL);
	if (re < 0) {
		cout << "avcodec_open2 failed ";
		PrintErr(re);
		return false;
	}

	return true;
}

AVFrame* XCode::CreateFrame()
{
	unique_lock<mutex> lock(mut_);
	if (!context_)
	{
		cout << "CreateFrame failed" << endl;
		return nullptr;
	}

	frame_ = av_frame_alloc();
	frame_->width = context_->width;
	frame_->height = context_->height;
	frame_->format = context_->pix_fmt;

	int re = av_frame_get_buffer(frame_, 0);
	if (re < 0) {
		PrintErr(re);
		av_frame_free(&frame_);
		return nullptr;
	}

	return frame_;
}

bool XCode::Close()
{
	if (context_) avcodec_free_context(&context_);

	return true;
}