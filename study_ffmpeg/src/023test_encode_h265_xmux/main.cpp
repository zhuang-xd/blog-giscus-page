#include <iostream>
#include <string>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}
#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"
#include "../xvideoview/xencode.h"
#include "../xvideoview/xdemux.h"
#include "../xvideoview/xmux.h"

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")


#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\400_300_25.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\full.mp4"
#define PATH_OUT "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\out.mp4"

using namespace std;

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
		sec_begin = atoi(argv[3]); 	// ??4??????
	}
	if (argc >= 5) {
		sec_end = atoi(argv[4]);	// ??5??????
	}
	cout << "sec_begin = " << sec_begin << endl;
	cout << "sec_end = " << sec_end << endl;
	cout << "=========================" << endl;
	
	int re = 0;

	// ??????
	int width_dst = 400;
	int height_dst = 300;
	AVFrame* frame_dst = nullptr;

	////////////////////////////////////////////////////////////////////////////// 
	//		??? MP4
	////////////////////////////////////////////////////////////////////////////// 

	XDemux xdemux;
	auto context_fmt_in = xdemux.Open(argv[1]); // ??????????


	////////////////////////////////////////////////////////////////////////////// 
	//		???????
	////////////////////////////////////////////////////////////////////////////// 

	XDecode xdec;
	auto context_dec = xdec.Create(AV_CODEC_ID_H264, false); // ??????????

	// ????????????????????
	avcodec_parameters_to_context(context_dec, context_fmt_in->streams[xdemux.video_index()]->codecpar);

	xdec.SetContext(context_dec);
	xdec.Open();
	auto frame = xdec.CreateFrame();

	////////////////////////////////////////////////////////////////////////////// 
	//		???????
	////////////////////////////////////////////////////////////////////////////// 

	XEncode xenc;
	auto context_enc = xenc.Create(AV_CODEC_ID_HEVC, true); // ??????????
	//context_enc->width = context_fmt_in->streams[xdemux.video_index()]->codecpar->width;
	//context_enc->height = context_fmt_in->streams[xdemux.video_index()]->codecpar->height;
	context_enc->width = width_dst;
	context_enc->height = height_dst;
	context_enc->pix_fmt = (AVPixelFormat)context_fmt_in->streams[xdemux.video_index()]->codecpar->format;
	context_enc->thread_count = 16;
	context_enc->time_base.den = xdemux.timebase_video().den;
	context_enc->time_base.num = xdemux.timebase_video().num;
	xenc.SetContextOption("preset", "medium");
	xenc.SetContextOption("crf", "28");
	//avcodec_parameters_from_context(context_fmt_in->streams[xdemux.video_index()]->codecpar, context_enc);
	xenc.SetContext(context_enc);
	xenc.Open();

	////////////////////////////////////////////////////////////////////////////// 
	//		?? MP4
	////////////////////////////////////////////////////////////////////////////// 

	AVPacket* pkt = av_packet_alloc();
	XMux xmux;
	auto context_fmt_out = xmux.Open(argv[2]); // ?????????

	auto vs_in = context_fmt_in->streams[xdemux.video_index()];
	auto as_in = context_fmt_in->streams[xdemux.audio_index()];
	auto vs_out = context_fmt_out->streams[xdemux.video_index()];
	auto as_out = context_fmt_out->streams[xdemux.audio_index()];

	if (xdemux.video_index() >= 0)
	{
		// ????h265???????????, ?????????????????
		avcodec_parameters_from_context(vs_out->codecpar, context_enc);
		//xmux.CopyParams(xmux.video_index(), vs_in->codecpar);
	}
	if (xdemux.audio_index() >= 0)// ?????????????????????????????????????????
	{
		// ????mp4?????????????, ?????????????????
		xmux.CopyParams(xmux.audio_index(), as_in->codecpar);
	}

	// ???????????pts
	long long pts_begin_audio = 0;	// ?????????pts
	long long pts_begin_video = 0;	// ?????????pts
	long long pts_end_video = 0;	// ??????????pts????????????

	// num ????den ???
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

	////////////////////////////////////////////////////////////////////////////// 
	
	// ??????????????0??
	xdemux.Seek(pts_begin_video);

	// ?????????
	xmux.WriteHeader();

	while (true)
	// 77{
		re = xdemux.Read(pkt);
		if (re != 0) break;

		// ????
		if (pkt->stream_index == xdemux.video_index())
		{
			// ???????0??????????
			if (pkt->pts > pts_end_video) 
			{
				av_packet_unref(pkt);
				break;
			}

			// ???¡¤??????????pts??dts??duration
			xmux.UpdatePkt(pkt, pts_begin_video, xdemux.timebase_video());
#if 0
			cout << "video pts = " << pkt->pts << endl;
			xmux.Write(pkt);
#else
			////////////////////////////////////////////////////////////////////////////// 
			//		??? h265
			////////////////////////////////////////////////////////////////////////////// 

			if (xdec.SendPkt(pkt))
			{
				//cout << "send pkt ok !!" << endl;
				while (true)
				{
					if(!xdec.RecvFrame(frame)) break; // ????????

					static bool is_first = true;
					if (is_first) {	
						is_first = false;
						frame_dst = av_frame_alloc();
						frame_dst->width = width_dst;
						frame_dst->height = height_dst;
						frame_dst->format = frame->format;
						frame_dst->duration = frame->duration;
						frame_dst->time_base = frame->time_base;
						av_frame_get_buffer(frame_dst, 0);
					}

					// scale
					av_frame_copy_props(frame_dst, frame);
					auto context_sws = sws_getCachedContext(
						NULL,
						frame->width,frame->height,(AVPixelFormat)frame->format,
						width_dst,height_dst, (AVPixelFormat)frame->format,
						SWS_BILINEAR,
						0,0,0
					);

					sws_scale(
						context_sws, 
						frame->data,frame->linesize,0,frame->height,
						frame_dst->data,frame_dst->linesize
					);

					auto pkt_recv = xenc.Encode(frame_dst); // ????h265
					if (pkt_recv) {
						cout << "video pts = " << pkt_recv->pts << endl;
						xmux.Write(pkt_recv);
						av_packet_free(&pkt_recv);
					}
				}
			}
			av_packet_unref(pkt);
#endif
		}

		// ????
		else if (pkt->stream_index == xdemux.audio_index())
		{
			// ????????????????????????????
			if (pkt->pts < pts_begin_audio) {
				continue;
			}
			
			// ???¡¤??????????pts??dts??duration
			xmux.UpdatePkt(pkt, pts_begin_audio,xdemux.timebase_audio());

			cout << "audio dts = " << pkt->dts << endl;
			xmux.Write(pkt);
		}
	}

	xmux.WriteEnd();

	cout << "wirte finished!" << endl;
	xdemux.SetContext(nullptr);
	xmux.SetContext(nullptr);
	av_packet_free(&pkt);
	avformat_close_input(&context_fmt_out);
}