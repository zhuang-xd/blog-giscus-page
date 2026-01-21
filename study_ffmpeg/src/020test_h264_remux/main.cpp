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
#define PATH_OUT "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\out.mp4"

void PrintErr(int re)
{
	char buf[128];
	av_strerror(re,buf,sizeof(buf));
	cout << buf << endl;
}

#define CERR(re)  do{if (re != 0) {PrintErr(re); return -1;}}while(0)

int main()
{
	int re = 0;
	AVFormatContext* context_fmt_in = nullptr;
	AVFormatContext* context_fmt_out = nullptr;
	AVStream* vs = nullptr;
	AVStream* as = nullptr;

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
			vs = context_fmt_in->streams[i];
		else if (context_fmt_in->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			as = context_fmt_in->streams[i];
	}

	////////////////////////////////////////////////////////////////////////////// 
	//		重封装 PATH_OUT
	////////////////////////////////////////////////////////////////////////////// 

	AVPacket* pkt = av_packet_alloc();

	// 创建输出上下文
	re = avformat_alloc_output_context2(&context_fmt_out,NULL,NULL, PATH_OUT);
	CERR(re);

	// 创建音视频流
	//auto enc = avcodec_find_encoder(vs->codecpar->codec_id);
	AVStream* vs_out = avformat_new_stream(context_fmt_out, NULL);
	AVStream* as_out = avformat_new_stream(context_fmt_out, NULL);

	// 打开输出io
	AVIOContext* context_avio = context_fmt_out->pb;
	re = avio_open(&context_fmt_out->pb,PATH_OUT, AVIO_FLAG_WRITE);
	CERR(re);

	if (vs) {	
		vs_out->time_base = vs->time_base;
		re = avcodec_parameters_copy(vs_out->codecpar,vs->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中
		CERR(re);
	}
	if (as) {	
		as_out->time_base = as->time_base;
		re = avcodec_parameters_copy(as_out->codecpar,as->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中
		CERR(re);
	}

	// 写入输出头信息
	re = avformat_write_header(context_fmt_out,NULL);
	CERR(re);

	int cnt = 0;
	while (true)
	{
		re = av_read_frame(context_fmt_in,pkt); // 读取输入文件的pkt
		if (re != 0) break;

		cout << "wirte pkt" << cnt++ << endl;

		// 写入时音视频流
		re = av_interleaved_write_frame(context_fmt_out, pkt);
		if (re != 0) break;
	}

	// 写入输出结束信息
	re = av_write_trailer(context_fmt_out);
	CERR(re);

	cout << "wirte finished!" << endl;

	avformat_close_input(&context_fmt_in);
	avformat_close_input(&context_fmt_out);
	avio_close(context_avio);
}