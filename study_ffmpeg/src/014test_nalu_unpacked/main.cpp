#include <iostream>

#include <fstream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
}

using namespace std;

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

void log_error(int num)
{
	if (num < 0) {
		char buf[128] = { 0, };
		av_strerror(num, buf, sizeof(buf));
	}
}

#define YUV_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.yuv"
#define OUT_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.h264"

int main()
{
	ifstream ifs;
	ifs.open(YUV_PATH, ios::binary);
	ofstream ofs;
	ofs.open(OUT_PATH, ios::binary);

	// 查找编码器
	AVCodecID enc_id = AV_CODEC_ID_H264;
	const AVCodec* enc_context = avcodec_find_encoder(enc_id);

	// 编辑上下文
	AVCodecContext* context = avcodec_alloc_context3(enc_context);
	context->width = 640;
	context->height = 360;
	context->pix_fmt = AV_PIX_FMT_YUV420P;
	context->time_base = { 1,25 };
	context->thread_count = 16;

	// 其他参数设置
	context->max_b_frames = 0; // 无b帧，会减少延迟，但增大编码后的文件体积
	//av_opt_set(context->priv_data,"preset","ultrafast",0); // 预设 最快
	//av_opt_set(context->priv_data,"tune","zerolatency",0); // 优化 零延迟

	av_opt_set(context->priv_data,"nal-hrd","cbr",0); // CBR
	context->bit_rate = 500 * 1000; // kbps
	context->rc_min_rate = context->bit_rate;
	context->rc_max_rate = context->bit_rate;
	context->rc_buffer_size = context->bit_rate * 2;
	//av_opt_set(context->priv_data, "qp","18", 0);

	// 打开编码器
	avcodec_open2(context, enc_context, NULL);

	// 发送原始帧
	AVFrame* frame = av_frame_alloc();
	frame->width = 640;
	frame->height = 360;
	frame->format = AV_PIX_FMT_YUV420P;
	av_frame_get_buffer(frame, 0);

	AVPacket* pkt = av_packet_alloc();

	int i = 0;
	for (;;)
	{
		ifs.read((char*)frame->data[0], frame->width * frame->height);
		if (ifs.gcount() == 0) break; // y的数据不对可以退出了
		ifs.read((char*)frame->data[1], frame->width * frame->height / 4);
		ifs.read((char*)frame->data[2], frame->width * frame->height / 4);
		frame->pts = i++;

		int re = avcodec_send_frame(context, frame);
		if (re != 0) { // 返回值为0是success
			char buf[128];
			av_strerror(re, buf, 128);
			cout << "err: " << buf << endl;
			break;
		}

		// 接收编码帧，消费者模式可能存在接收多个数据帧的情况，因此需要循环
		while (1)
		{
			re = avcodec_receive_packet(context, pkt);
			if (re == 0) {
				
				// 拆包
				// 前4位是0001
				//printf("%02x %02x %02x %02x"
				//	, pkt->data[0]
				//	, pkt->data[1]
				//	, pkt->data[2]
				//	, pkt->data[3]
				//);
				//return -1;

				// header



				// 先通过 001或者0001找到i帧，取后第二个字节就是head了，其中后5位就是nalu
#if 0		
				for (int i = 0; i < pkt->size - 3; i++)
				{
					if (pkt->data[i] == 0 && pkt->data[i + 1] == 0 && pkt->data[i + 2] == 1) {
						unsigned char header = pkt->data[i+3]; // 1个字节
						printf("%02x\n", header); // 67 = 0110 1111

						/*
							0		= 0			// forbiden_bit
							11		= 3			// nal_reference_bit 优先级
							01111	= 7（sps）	// nal_unit_type 类型
						*/
						unsigned char nalu = header & 0x1f; // 与上 0001 1111 就是后5位了
						printf("%x ", nalu);  // 7 = sps , 5 = idr

						//return -1;
					}
				}
#endif

				ofs.write((char*)pkt->data, pkt->size);
				av_packet_unref(pkt); // 出这次循环的时候引用计数不会自动-1，需要手动减
			}
			else if (re == AVERROR(EAGAIN) || re == AVERROR(EINVAL))
			{
				break;
			}
			else
			{
				char buf[128];
				av_strerror(re, buf, 128);
				break;
			}
		}
	}

	// 释放
	avcodec_free_context(&context);
	av_frame_free(&frame);
	av_packet_free(&pkt);
	ifs.close();
	ofs.close();
}