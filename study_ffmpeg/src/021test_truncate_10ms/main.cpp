#include <iostream>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}
#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")

using namespace std;

#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\400_300_25.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\full.mp4"
#define PATH_OUT "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\out.mp4"

void PrintErr(int re)
{
	char buf[128];
	av_strerror(re,buf,sizeof(buf));
	cout << buf << endl;
}

#define CERR(re)  do{if (re != 0) {PrintErr(re); cout << __LINE__ <<endl; return -1;}}while(0)

int main()
{
	int re = 0;
	AVFormatContext* context_fmt_in = nullptr;
	AVFormatContext* context_fmt_out = nullptr;
	AVStream* vs_in = nullptr;
	AVStream* as_in = nullptr;

	////////////////////////////////////////////////////////////////////////////// 
	//		解封装 PATH_MP4
	////////////////////////////////////////////////////////////////////////////// 

	re = avformat_open_input(&context_fmt_in,PATH_MP4,NULL,NULL);
	CERR(re);

	re = avformat_find_stream_info(context_fmt_in,NULL); // 获取流媒体详细信息
	CERR(re);
	
	av_dump_format(context_fmt_in,NULL,PATH_MP4,NULL);

	for (int i = 0; i < context_fmt_in->nb_streams; i++)
	{
		if (context_fmt_in->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			vs_in = context_fmt_in->streams[i];
		else if (context_fmt_in->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			as_in = context_fmt_in->streams[i];
	}

	////////////////////////////////////////////////////////////////////////////// 
	//		重封装 PATH_OUT
	////////////////////////////////////////////////////////////////////////////// 

	AVPacket* pkt = av_packet_alloc();

	// 创建输出上下文
	re = avformat_alloc_output_context2(&context_fmt_out,NULL,NULL, PATH_OUT);
	CERR(re);

	// 创建音视频流
	//auto enc = avcodec_find_encoder(vs_in->codecpar->codec_id);
	AVStream* vs_out = avformat_new_stream(context_fmt_out, NULL);
	AVStream* as_out = avformat_new_stream(context_fmt_out, NULL);

	// 打开输出io
	AVIOContext* context_avio = context_fmt_out->pb;
	re = avio_open(&context_fmt_out->pb,PATH_OUT, AVIO_FLAG_WRITE);
	CERR(re);

	if (vs_in) {	
		vs_out->time_base = vs_in->time_base;
		re = avcodec_parameters_copy(vs_out->codecpar,vs_in->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中
		CERR(re);
	}
	if (as_in) {	
		as_out->time_base = as_in->time_base;
		re = avcodec_parameters_copy(as_out->codecpar,as_in->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中
		CERR(re);
	}

	// 截断处理，计算pts
	long long sec_begin = 10.0;
	long long sec_end = 20.0;
	long long pts_begin_audio = 0;	// 截断音频流起始pts
	long long pts_begin_video = 0;	// 截断视频流起始pts
	long long pts_end_video = 0;		// 截断视频流结束pts，音频以视频为准


	if (vs_in && vs_in->time_base.den > 0) {
		auto time_base_of_1 = vs_in->time_base.den / vs_in->time_base.num; // num 分子， den 分母
		// pts = sec / time_base
		pts_begin_video = sec_begin * time_base_of_1;
		pts_end_video = sec_end * time_base_of_1;
	}
	if (as_in && as_in->time_base.den > 0) {
		auto time_base_of_1 = as_in->time_base.den / as_in->time_base.num; // num 分子， den 分母
		pts_begin_audio = sec_begin * time_base_of_1;
	}

	// 视频流开头移动到第10秒
	re = av_seek_frame(context_fmt_in,vs_in->index,pts_begin_video,AVSEEK_FLAG_FRAME| AVSEEK_FLAG_BACKWARD);
	CERR(re);
	// av_seek_frame只能处理一个流，音频需要视频流为基准，对音频流读取并丢弃pkt包实现音视频同步

	cout << "=================================" << endl;
	cout << "audio begin pts = " << pts_begin_audio << endl;
	cout << "video begin pts = " << pts_begin_video << endl;
	cout << "video end pts = " << pts_end_video << endl;
	cout << "=================================" << endl;

	long long pts_offset_video = -1;
	long long pts_offset_audio = -1;

	// 写入输出头信息
	re = avformat_write_header(context_fmt_out,NULL);
	CERR(re);

	int sec_offset = 0;
	int cnt = 0;
	while (true)
	{
		re = av_read_frame(context_fmt_in,pkt); // 读取输入文件的pkt
		if (re != 0) break;

		// 重新分配数据包的pts、dts、duration
		if (vs_in && pkt->stream_index == vs_in->index) // 视频
		{
			// 视频从超过20秒的部分不写入，退出
			if (pkt->pts > pts_end_video) {
				av_packet_unref(pkt);
				break;
			}

			if (pts_offset_video == -1) {			
				pts_offset_video = pts_begin_video; // 更新当前偏移量
				cout << "first video pts = " << pts_offset_video << endl;
			}

			// 重新写入pts、dts、duration（这一步主要是为了兼容性）
			pkt->pts = av_rescale_q_rnd(pkt->pts - pts_offset_video, vs_in->time_base, vs_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
			pkt->dts = av_rescale_q_rnd(pkt->dts - pts_offset_video, vs_in->time_base, vs_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
			pkt->duration = av_rescale_q(pkt->duration, vs_in->time_base, vs_out->time_base);
			pkt->pos = -1;
			pkt->stream_index = vs_out->index;
		}
		else if (as_in && pkt->stream_index == as_in->index) // 音频
		{
			// 接收第一个视频数据包后才接收处理音频数据包
			if (pts_offset_video == -1 || pkt->pts < pts_begin_audio) {
				continue;
			}
			if (pts_offset_audio == -1) {
				pts_offset_audio = pts_begin_audio;
			}

			// 重写pts、dts、duration
			pkt->pts = av_rescale_q_rnd(pkt->pts - pts_offset_audio, as_in->time_base, as_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
			pkt->dts = av_rescale_q_rnd(pkt->dts - pts_offset_audio, as_in->time_base, as_out->time_base, (AVRounding)(AV_ROUND_INF | AV_ROUND_PASS_MINMAX));
			pkt->duration = av_rescale_q(pkt->duration, as_in->time_base, as_out->time_base);
			pkt->pos = -1;
			pkt->stream_index = as_out->index;
		}

		// av_interleaved_write_frame会自动释放pkt，因此要在写入前打印，否则会是随机值
		if (pkt->stream_index == 0) // 视频
			cout << "video pts = " << pkt->pts << " " << flush;
		else if (pkt->stream_index == 1) // 音频
			cout << "audio dts = " << pkt->dts << endl;

		// 写入音频or视频数据包
		re = av_interleaved_write_frame(context_fmt_out, pkt); 
		if (re != 0) break;
	}

	// 写入缓冲区的pkt
	re = av_interleaved_write_frame(context_fmt_out, nullptr); 
	CERR(re);

	// 写入输出结束信息
	re = av_write_trailer(context_fmt_out);
	CERR(re);

	cout << "wirte finished!" << endl;

	avio_close(context_avio);
	avformat_close_input(&context_fmt_in);
	avformat_close_input(&context_fmt_out);
}