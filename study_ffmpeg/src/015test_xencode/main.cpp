#include <iostream>
#include <fstream>
#include "xencode.h"

using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#define PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\xenc.h264"

int main()
{
	ofstream ofs;
	ofs.open(PATH,ios::binary);
	if (ofs.is_open()) {
		cout << "is open" << endl;
	}

	XEncode enc;
	auto context = enc.Create(27);
	context->width = 480;
	context->height = 360;
	context->pix_fmt = AV_PIX_FMT_YUV420P;
	context->thread_count = 16;
	context->time_base = { 1,25 };

	enc.SetContext(context);
	enc.SetContextOption("crf", 18);
	enc.Open();
	AVFrame *frame = enc.CreateFrame();
	int cnt = 0;

#if 1
	for (int i = 0; i < 1000; i++)
	{
		// y
		for (int h = 0; h < frame->height; h++)
		{
			for (int w = 0; w < frame->width; w++)
			{
				frame->data[0][frame->linesize[0] * h + w] = 200 + 8 * i;
			}
		}

		// uv
		for (int h = 0; h < frame->height / 2; h++)
		{
			for (int w = 0; w < frame->width / 2; w++)
			{
				frame->data[1][frame->linesize[1] * h + w] = 128 + 3 * i;
				frame->data[2][frame->linesize[2] * h + w] = 64 + 5 * i;
			}
		}
		frame->pts = i;

		// 接收编码帧
		auto pkt = enc.Encode(frame);
		if (pkt) {
			cnt++;
			ofs.write((char *)pkt->data,pkt->size);
		}
		
	}

	// 取出缓冲区
	auto list = enc.End();
	for (auto pkt: list)
	{
		if (pkt) {
			cnt++;
			ofs.write((char *)pkt->data,pkt->size);
		}
	}
#endif

	cout << "cnt = " << cnt << endl;

	av_frame_free(&frame);
	ofs.close();
	enc.Close();
}