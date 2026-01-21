#include "xdemux.h"
#include <iostream>

using namespace std;

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

static void PrintErr(int re)
{
	char buf[128];
	av_strerror(re, buf, sizeof(buf));
	cout << buf << endl;
}

AVFormatContext* XDemux::Open(const char *path)
{
	// 创建上下文
	int re = avformat_open_input(&context_, path, NULL, NULL);
	if (re != 0)
	{
		PrintErr(re);
		return nullptr;
	}

	// 获取流媒体详细信息
	re = avformat_find_stream_info(context_, NULL); 
	if (re != 0)
	{
		PrintErr(re);
		return nullptr;
	}

	// 打印流媒体信息
	av_dump_format(context_, NULL, path, NULL);


	// 更新index
	for (int i = 0; i < context_->nb_streams; i++)
	{
		if (context_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			video_index_ = i;
		else if (context_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_index_ = i;
	}
	// 存下timebase
	memcpy(&timebase_video_, &context_->streams[video_index_]->time_base, sizeof(AVRational));
	memcpy(&timebase_audio_, &context_->streams[audio_index_]->time_base, sizeof(AVRational));

	return context_;
}

bool XDemux::Read(AVPacket *pkt)
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_) return false;

	int re = av_read_frame(context_,pkt);
	if (re != 0)
	{
		PrintErr(re);
	}

	return re;
}

bool XDemux::SetContext(AVFormatContext* context)
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (context) {
		// 传入了新的地址，就代替原有的
		avformat_close_input(&context_);
	}
	else {
		return false;
	}

	context_ = context;

	// 确定音视频流的顺序
	for (int i = 0; i < context_->nb_streams; i++)
	{
		if (context_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			video_index_ = i;
			//vs_ = context_->streams[i];
		else if (context_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_index_ = i;
			//as_ = context_->streams[i];
	}

	return true;
}

// av_seek_frame只能处理一个流，音频需要视频流为基准，对音频流读取并丢弃pkt包实现音视频同步
bool XDemux::Seek(long long pts_begin_video)
{
	int re = av_seek_frame(context_, video_index_, pts_begin_video, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);

	return re;
}