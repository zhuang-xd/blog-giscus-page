#include "xdecode.h"
#include <iostream>

using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
}

static void PrintErr(int re)
{
	char buf[128];
	av_strerror(re, buf, sizeof(buf));
	cout << buf << endl;
}

bool XDecode::SendPkt(AVPacket* pkt)
{
	unique_lock<mutex> lock(mut_);
	if (!pkt) return false;
	int re = avcodec_send_packet(context_,pkt);
	if (re == 0) {
		return true;
	}

	//PrintErr(re);
	return false;
}

bool XDecode::RecvFrame(AVFrame* frame)
{
	unique_lock<mutex> lock(mut_);
	if (!frame) return false;
	int re = avcodec_receive_frame(context_,frame);
	if (re == 0) {
		return true;
	}

	//cout << "RecvFrame " << flush;
	//PrintErr(re);
	return false;
}

std::vector<AVFrame*> XDecode::End()
{
	unique_lock<mutex> lock(mut_);
	avcodec_send_packet(context_, NULL);
	vector<AVFrame*> frames;
	while (true)
	{
		AVFrame* frame = av_frame_alloc();
		int re = avcodec_receive_frame(context_, frame);
		if (re == 0) {
			frames.push_back(frame);
		}
		else {
			av_frame_free(&frame);
			break;
		}
	}
	return frames;
}