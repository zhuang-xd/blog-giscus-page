#include "xmux.h"
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

AVFormatContext* XMux::Open(const char* path)
{
	// 创建输出上下文
	int re = avformat_alloc_output_context2(&context_, NULL, NULL, path);
	if (re != 0) {
		PrintErr(re);
		return nullptr;
	}

	// 创建音视频流
	auto vs = avformat_new_stream(context_, NULL); // 视频
	if (!vs) {
		cout << "avformat_new_stream video failed" << endl;
		return nullptr;
	}
	video_index_ = 0;
	vs->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

	auto as = avformat_new_stream(context_, NULL); // 音频
	if (!as) {
		cout << "avformat_new_stream audio failed" << endl;
		return nullptr;
	}
	audio_index_ = 1;
	as->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

	// 打开输出io
	re = avio_open(&context_->pb, path, AVIO_FLAG_WRITE);
	if (re != 0) {
		PrintErr(re);
		return nullptr;
	}

	return context_;
}

bool XMux::SetContext(AVFormatContext* context)
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_) {
		// 传入了新的地址，就代替原有的
		//avformat_free_context(context_); // 这个释放会连着stream一起释放
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
		else if (context_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			audio_index_ = i;
	}

	return true;
}

// 传进来的parameters拷贝到当前的parameters
bool XMux::CopyParams(int stream_index, AVCodecParameters* params)
{
	unique_lock<mutex> lock(mut_);
	if (!context_ || !params) return false;

	AVStream* stream_out = context_->streams[stream_index];
	if (!stream_out) return false;

	int re = avcodec_parameters_copy(stream_out->codecpar, params);
	if (re != 0) {
		cout << "CopyParams failed " << flush;
		PrintErr(re);
		return false;
	}

	return true;
}

bool XMux::UpdatePkt(AVPacket *pkt, long long pts_offset, XRational timebase_in)
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_) return false;

	AVStream* stream_out = context_->streams[pkt->stream_index];
	
	AVRational timebase_in_av;
	timebase_in_av.den = timebase_in.den;
	timebase_in_av.num = timebase_in.num;

	// 重新写入pts、dts、duration（这一步主要是为了兼容性）
	pkt->pts = av_rescale_q_rnd(pkt->pts - pts_offset, timebase_in_av, stream_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
	pkt->dts = av_rescale_q_rnd(pkt->dts - pts_offset, timebase_in_av, stream_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
	pkt->duration = av_rescale_q(pkt->duration, timebase_in_av, stream_out->time_base);
	pkt->pos = -1;
	pkt->stream_index = stream_out->index;

	return true;
}

bool XMux::WriteHeader()
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_) return false;

	int re = avformat_write_header(context_, NULL);
	if (re != 0) {
		cout << "XMux::WriteHeader " << flush;
		PrintErr(re);
		return false;
	}

	return true;
}

bool XMux::Write(AVPacket* pkt)
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_ || !pkt) return false;

	int re = av_interleaved_write_frame(context_, pkt);
	if (re != 0) {
		cout << "XMux::Write " << flush;
		PrintErr(re);
		return false;
	}

	return true;
}

bool XMux::WriteEnd()
{
	unique_lock<mutex> lock(mut_); // 线程安全
	if (!context_) return false;

	// 写入缓冲区的pkt
	int re = av_interleaved_write_frame(context_, nullptr);
	if (re != 0) {
		cout << "av_interleaved_write_frame " << flush;
		PrintErr(re);
		return false;
	}

	// 写入输出结束信息
	re = av_write_trailer(context_);
	if (re != 0) {
		cout << "av_write_trailer " << flush;
		PrintErr(re);
		return false;
	}
	
	return true;
}