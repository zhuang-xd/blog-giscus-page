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

AVCodecContext* XEncode::Create(int id)
{
	// 查找编码器
	const AVCodec* enc = avcodec_find_encoder((AVCodecID)id);		// 对象只读的
	if (!enc)
	{
		cout << "avcodec_find_decoder failed" << endl;
		return nullptr;
	}

	// 创建上下文
	AVCodecContext* context = avcodec_alloc_context3(enc);;
	if (!context)
	{
		cout << "avcodec_alloc_context3 failed" << endl;
		return nullptr;
	}

	return context;
}

bool XEncode::SetContext(AVCodecContext* context)
{
	unique_lock <mutex> lock(mut_);
	//if (context_)
	//{
	//	avcodec_free_context(&context_); // 先释放原有的context
	//}	
	
	context_ = context;
		
	return true;
}

bool XEncode::SetContextOption(const char* key, int val)
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return false;
	
	int re = av_opt_set_int(context_->priv_data,key,val,0);
	if (re < 0) {
		cout << "SetContextOption failed ";
		PrintErr(re);
		return false;
	}

	return true;
}

bool XEncode::SetContextOption(const char* key, const char* val)
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

bool XEncode::Open()
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

AVFrame* XEncode::CreateFrame()
{
	unique_lock<mutex> lock(mut_);
	if (!context_)
	{
		cout << "CreateFrame failed" << endl;
		return nullptr;
	}

	AVFrame* frame = av_frame_alloc();
	frame->width = context_->width;
	frame->height = context_->height;
	frame->format = context_->pix_fmt;

	int re = av_frame_get_buffer(frame,0);
	if (re < 0) {
		PrintErr(re);
		av_frame_free(&frame);
		return nullptr;
	}

	return frame;
}

AVPacket* XEncode::Encode(AVFrame* frame)
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return nullptr;

	int re = avcodec_send_frame(context_, frame);
	if (re != 0) {
		PrintErr(re);
		return nullptr;
	}

	AVPacket* pkt = av_packet_alloc();
	while (true)
	{
		int re = avcodec_receive_packet(context_, pkt);
		if (re == 0) 
		{
			return pkt;
		}
		else if (re == AVERROR(ENOMEM) || re == AVERROR(EINVAL))
		{
			break;
		}
		else {
			PrintErr(re);
			break;
		}
	}

	av_packet_free(&pkt);
	return nullptr;
}


std::vector<AVPacket*> XEncode::End()
{
	unique_lock<mutex> lock(mut_);

	vector<AVPacket*> pkt_vector;
	if (!context_) return pkt_vector;

	int re = avcodec_send_frame(context_,NULL);
	if (re != 0) {
		return pkt_vector;
	}
	
	while (true)
	{
		AVPacket* pkt = av_packet_alloc();

		int re = avcodec_receive_packet(context_, pkt);
		if (re == 0) {
			pkt_vector.push_back(pkt);
		}
		else if (re == AVERROR_EOF || re == AVERROR(EINVAL))
		{
			break;
		}
		else {
			PrintErr(re);
			break;
		}
	}

	return pkt_vector;
}

bool XEncode::Close()
{
	if (context_) avcodec_free_context(&context_);

	return true;
}