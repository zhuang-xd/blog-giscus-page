#include <iostream>

#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"
#include "../xvideoview/xencode.h"
#include "../xvideoview/xdemux.h"
#include "../xvideoview/xmux.h"
#include "../xvideoview/xformat.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

using namespace std;

//#define PATH_RTSP "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream"
#define PATH_RTSP "rtsp://127.0.0.1:8554/vlc"

int main()
{
	int re = -1;

	/////////////////////////////////////////
	//	解封装
	/////////////////////////////////////////
	XDemux xdemux;
	auto context_demux = xdemux.Open(PATH_RTSP);
	xdemux.SetContext(context_demux);
	
	/////////////////////////////////////////
	//	解码
	/////////////////////////////////////////

	auto vs_demux = context_demux->streams[xdemux.video_index()];
	auto as_demux = context_demux->streams[xdemux.audio_index()];

	XDecode xdec;
	auto context_dec = xdec.Create(context_demux->video_codec_id,false);
	context_dec->width = vs_demux->codecpar->width;
	context_dec->height = vs_demux->codecpar->height;
	context_dec->pix_fmt = (AVPixelFormat)vs_demux->codecpar->format;
	xdec.SetContext(context_dec);
	if (!xdec.Open()) {
		cout << "xdec open failed" << endl;
		return -1;
	}

	/////////////////////////////////////////
	//	编码
	/////////////////////////////////////////

	/////////////////////////////////////////
	//	封装
	/////////////////////////////////////////


	/////////////////////////////////////////

	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	
	while (true)
	{
		if (!xdemux.Read(pkt))
		{
			cout << "xdemux read failed !" << endl;
			break;
		}

		if (!xdec.SendPkt(pkt))
		{
			cout << "xdemux SendPkt failed !" << endl;
			continue;
		}

		while (true)
		{
			if(!xdec.RecvFrame(frame))
			{
				cout << "xdemux RecvFrame failed !" << endl;
				break;
			}
		}
	}

	cout << "write finished" << endl;

	av_frame_free(&frame);
	av_packet_free(&pkt);
}