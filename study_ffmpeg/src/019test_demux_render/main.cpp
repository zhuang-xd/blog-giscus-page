#include <iostream>
#include <fstream>
#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"
#include <thread>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")

using namespace std;

#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.mp4"
//#define PATH_MP4 "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\400_300_25.mp4"

#define LIB_ENABLE

bool ErrPrint(int re)
{
	if (re != 0)
	{
		char buf[128];
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		return false;
	}

	return true;
}

#define CERR(re) if(!ErrPrint(re)) {  return -1;}

#ifdef LIB_ENABLE
int main()
{
	int re = 0;

	AVFormatContext* context_fmt = nullptr;

	// �������װ��
	re = avformat_open_input(
		&context_fmt,			// �����avformat_alloc_context����ռ䣬��Ҫ�ṩһ��ָ��
		PATH_MP4,				// iput_url
		NULL,
		NULL
	);
	CERR(re)

	// ��ȡý����Ϣ
	re = avformat_find_stream_info(context_fmt, NULL);
	CERR(re)

	// ��ӡ��װ��Ϣ
	av_dump_format(
		context_fmt,
		0,
		PATH_MP4,
		0
	);

	AVPacket* pkt = av_packet_alloc();
	AVStream* vs = nullptr;
	AVStream* as = nullptr;
	for (int i = 0; i < context_fmt->nb_streams; i++)
	{
		if (context_fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			vs = context_fmt->streams[i];
		if (context_fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			as = context_fmt->streams[i];
	}

	// ����xdec����
	XDecode xdec;
	auto context_dec = xdec.Create(vs->codecpar->codec_id,false);

	// ����Ƶ����������context�У���Ȼ���Ҳ���0001������No start code is found.
	re = avcodec_parameters_to_context(context_dec, vs->codecpar);
	CERR(re);

	xdec.SetContext(context_dec);
	auto frame = xdec.CreateFrame();
	xdec.Open();

	// ����view����
	XVideoView* view = XVideoView::Create(frame->width, frame->height, nullptr, frame->format);

	// ������Ƶ
	while (true)
	{
		// ��ȡһ֡���ݣ�������Ƶ����Ƶ��
		re = av_read_frame(context_fmt, pkt);
		if (!ErrPrint(re)) break;

		// ������Ƶ
		if (vs && pkt->stream_index == vs->index) {
			if (xdec.SendPkt(pkt)) { // ���ͱ���֡
				while (xdec.RecvFrame(frame)) // ����ԭʼ֡
				{
					cout << "frame pts :" << frame->pts << endl;
					view->Draw(
						frame->data[0], frame->linesize[0],
						frame->data[1], frame->linesize[1],
						frame->data[2], frame->linesize[2]
					);
					this_thread::sleep_for(chrono::milliseconds(16));
				}
			}

		}

		// ������Ƶ
		else if (as && pkt->stream_index == as->index) {
			{

			}
		}

		//cout << "dts :" << pkt->dts << " pts :" << pkt->pts << " size :" << pkt->size << endl;
		av_packet_unref(pkt);
	}

	avformat_close_input(&context_fmt);
}

#else
int main()
{
	int re = 0;

	AVFormatContext* context_fmt = nullptr;

	// �������װ��
	re = avformat_open_input(
		&context_fmt,			// �����avformat_alloc_context����ռ䣬��Ҫ�ṩһ��ָ��
		PATH_MP4,				// iput_url
		NULL,
		NULL
	);
	CERR(re)


	// ��ȡý����Ϣ
	re = avformat_find_stream_info(context_fmt, NULL);
	CERR(re)

	// ��ӡ��װ��Ϣ
	av_dump_format(
		context_fmt,
		0,
		PATH_MP4,
		0
	);

	AVPacket* pkt = av_packet_alloc();

	AVStream* vs = nullptr;
	AVStream* as = nullptr;
	for (int i = 0; i < context_fmt->nb_streams; i++)
	{
		if (context_fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			vs = context_fmt->streams[i];
		if (context_fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
			as = context_fmt->streams[i];
#if 0
		cout << "stream" << "[" << i << "]" << endl;
		/*
			AVMEDIA_TYPE_VIDEO = 0,
			AVMEDIA_TYPE_AUDIO = 1
		*/
		cout << context_fmt->streams[i]->codecpar->codec_type << endl;	// ��Ƶor��Ƶ
		/*
			AV_CODEC_ID_H264	= 27
			AV_CODEC_ID_AAC		= 86018
		*/
		cout << context_fmt->streams[i]->codecpar->codec_id << endl;	// ��������

		cout << context_fmt->streams[i]->codecpar->width << endl;		// ��
		cout << context_fmt->streams[i]->codecpar->height << endl;		// ��
		cout << "==========================" << endl;
#endif
	}


#if 1
	auto dec = avcodec_find_decoder(vs->codecpar->codec_id);
	auto context_dec = avcodec_alloc_context3(dec);
#endif
	// ����xdec����
	//XDecode xdec;
	//AVCodecContext* context_dec = XDecode::Create(vs->codecpar->codec_id, false);

	// ����Ƶ����������context�У���Ȼ���Ҳ���0001������No start code is found.
	re = avcodec_parameters_to_context(context_dec, vs->codecpar);
	CERR(re);


	cout << "width = " << context_dec->width << endl;
	cout << "height = " << context_dec->height << endl;
	cout << "fmt = " << context_dec->pix_fmt << endl;

	AVFrame* frame = av_frame_alloc();
	frame->width = context_dec->width;
	frame->height = context_dec->height;
	frame->format = context_dec->pix_fmt;
	//xdec.SetContext(context_dec); // �ⲿ����ʹ��context_dec
	//xdec.Open();
	//auto frame = xdec.CreateFrame();
	XVideoView *view = XVideoView::Create(frame->width,frame->height,nullptr,frame->format);
	
	re = avcodec_open2(context_dec,NULL,NULL);
	CERR(re);

	// ������Ƶ
	while (true)
	{
		// ��ȡһ֡���ݣ�������Ƶ����Ƶ��
		re = av_read_frame(context_fmt, pkt);
		if (!ErrPrint(re)) break;

		// ������Ƶ
		if (vs && pkt->stream_index == vs->index) { 
			re = avcodec_send_packet(context_dec,pkt); // ���ͱ���֡
			if (re != 0) break;

			while (true)
			{
				re = avcodec_receive_frame(context_dec,frame); // ����ԭʼ֡
				if (re != 0) break;

				cout << "frame pts :" << frame->pts << endl;
				view->Draw(
					frame->data[0], frame->linesize[0],
					frame->data[1], frame->linesize[1],
					frame->data[2], frame->linesize[2]
				);
				this_thread::sleep_for(chrono::milliseconds(16));
			}
		}

		// ������Ƶ
		else if (as && pkt->stream_index == as->index) { 
			{

			}
		}

		//cout << "dts :" << pkt->dts << " pts :" << pkt->pts << " size :" << pkt->size << endl;
		av_packet_unref(pkt);
	}

	avformat_close_input(&context_fmt);
}
#endif // LIB_ENABLE