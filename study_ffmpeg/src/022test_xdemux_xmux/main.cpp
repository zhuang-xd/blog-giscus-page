#include <iostream>
#include <string>

extern "C"
{
#include "libavformat/avformat.h"
//#include "libavutil/avutil.h"
//#include "libavcodec/avcodec.h"
}
#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"
#include "../xvideoview/xencode.h"
#include "../xvideoview/xdemux.h"
#include "../xvideoview/xmux.h"

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")

using namespace std;

#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\400_300_25.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\full.mp4"
#define PATH_OUT "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\out.mp4"

static void PrintErr(int re)
{
	char buf[128];
	av_strerror(re, buf, sizeof(buf));
	cout << buf << endl;
}

#define CERR(re)  do{if (re != 0) {PrintErr(re); cout << __LINE__ <<endl; return -1;}}while(0)

int main(int argc, const char * argv[])
{
	cout << "argc = " << argc << endl;
	for (int i = 0; i < argc; i++)
	{
		cout << i  << " " << argv[i] << endl;
	}
	cout << "=========================" << endl;

	long long sec_begin = 0;
	long long sec_end = 0;

	if (argc < 4 || argc > 5) return 0;

	if (argc >= 4) {
		sec_begin = atoi(argv[3]); 
	}
	if (argc >= 5) {
		sec_end = atoi(argv[4]);	// 第5个参数
	}
	cout << "sec_begin = " << sec_begin << endl;
	cout << "sec_end = " << sec_end << endl;
	cout << "=========================" << endl;
	

	int re = 0;
	
	////////////////////////////////////////////////////////////////////////////// 
	//		解封装 PATH_MP4
	////////////////////////////////////////////////////////////////////////////// 

	XDemux xdemux;
	auto context_fmt_in = xdemux.Open(argv[1]); // 输入上下文

	////////////////////////////////////////////////////////////////////////////// 
	//		重封装 PATH_OUT
	////////////////////////////////////////////////////////////////////////////// 

	AVPacket* pkt = av_packet_alloc();
	XMux xmux;
	auto context_fmt_out = xmux.Open(argv[2]); // 输出上下文
	xmux.CopyParams(xmux.video_index(), context_fmt_in->streams[xdemux.video_index()]->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中
	xmux.CopyParams(xmux.audio_index(), context_fmt_in->streams[xdemux.audio_index()]->codecpar); // 将输入的mp4文件的信息拷贝到要输入的mp4中

	// 截断处理，计算pts
	long long pts_begin_audio = 0;	// 截断音频流起始pts
	long long pts_begin_video = 0;	// 截断视频流起始pts
	long long pts_end_video = 0;	// 截断视频流结束pts，音频以视频为准
	long long pts_offset = -1;		// pts偏移量

	// num 分子， den 分母
	// pts = sec / timebase
	if (xdemux.video_index() >= 0) {
		pts_begin_video = sec_begin * xdemux.timebase_video().den / xdemux.timebase_video().num ;
		pts_end_video = sec_end * xdemux.timebase_video().den / xdemux.timebase_video().num;
	}
	if (xdemux.audio_index() >= 0) {
		pts_begin_audio = sec_begin * xdemux.timebase_audio().den / xdemux.timebase_audio().num ;
	}

	cout << "=================================" << endl;
	cout << "audio begin pts = " << pts_begin_audio << endl;
	cout << "video begin pts = " << pts_begin_video << endl;
	cout << "video end pts = " << pts_end_video << endl;
	cout << "=================================" << endl;

	// 视频流开头移动到第10秒
	xdemux.Seek(pts_begin_video);

	// 写入输出头信息
	xmux.WriteHeader();

	int sec_offset = 0;
	int cnt = 0;
	while (true)
	{
		re = xdemux.Read(pkt);
		if (re != 0) break;

		// 重新分配数据包的pts、dts、duration
		if (pkt->stream_index == xdemux.video_index()) // 视频
		{
			// 视频从超过20秒的部分不写入，退出
			if (pkt->pts > pts_end_video) {
				av_packet_unref(pkt);
				break;
			}
			pts_offset = pts_begin_video;
			xmux.UpdatePkt(pkt, pts_offset,xdemux.timebase_video());
			cout << "update video pts" << endl;
		}
		else if (pkt->stream_index == xdemux.audio_index()) // 音频
		{
			// 接收第一个视频数据包后才接收处理音频数据包
			if (pts_offset == -1 || pkt->pts < pts_begin_audio) {
				continue;
			}
			pts_offset = pts_begin_audio;
			xmux.UpdatePkt(pkt, pts_offset,xdemux.timebase_audio());
		}


		// av_interleaved_write_frame会自动释放pkt，因此要在写入前打印，否则会是随机值
		if (pkt->stream_index == 0) // 视频
			cout << "video pts = " << pkt->pts << " " << flush;
		else if (pkt->stream_index == 1) // 音频
			cout << "audio dts = " << pkt->dts << endl;

		// 写入音频or视频数据包

		re = xmux.Write(pkt);
		//if (re != 0) break;
	}

	xmux.WriteEnd();


	cout << "wirte finished!" << endl;
	xdemux.SetContext(nullptr);
	xmux.SetContext(nullptr);
	av_packet_free(&pkt);
	avformat_close_input(&context_fmt_out);
}