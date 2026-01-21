#include <iostream>

#include <fstream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
}

#pragma comment(lib,"avcodec")
#pragma comment(lib,"avutil")

using namespace std;

#define YUV_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.yuv"
#define OUT_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.h264"
#define OUT2_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\800_300_30.h264"
#define OUT3_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\800_300_30.h265"

void yuv_file_to_h264();
void create_yuv_to_h264();

int main()
{
	yuv_file_to_h264();
	//create_yuv_to_h264();
}

void create_yuv_to_h264()
{
	ofstream ofs;
	ofs.open(OUT2_PATH, ios::binary);

	// 查找编码器
	auto enc = avcodec_find_encoder(AV_CODEC_ID_H264);
	// 创建编码器上下文
	auto enc_context = avcodec_alloc_context3(enc);
	enc_context->width = 800;
	enc_context->height = 300;
	enc_context->pix_fmt = AV_PIX_FMT_YUV420P;
	enc_context->time_base = { 1,25 };
	enc_context->thread_count = 16;
	// 打开编码器
	int re = avcodec_open2(enc_context, enc, NULL);
	if (re < 0) {
		char buf[128];
		av_strerror(re, buf, 128);
		cout << __LINE__ << "error: " << buf << endl;
	}

	// 分配存储空间
	AVFrame* frame = av_frame_alloc();
	frame->width = enc_context->width;
	frame->height = enc_context->height;
	frame->format = enc_context->pix_fmt;
	re = av_frame_get_buffer(frame, 0);
	if (re < 0) {
		char buf[128];
		av_strerror(re, buf, 128);
		cout << __LINE__ << "error: " << buf << endl;
	}

	AVPacket* pkt = av_packet_alloc();
	for (int i = 0; i < 250; i++)
	{
		// read 数据
		// y
		for (int h = 0; h < frame->height; h++)
		{
			for (int w = 0; w < frame->width; w++)
			{
				frame->data[0][h * frame->linesize[0] + w] = 200+i * 2;
			}
		}
		// uv
		for (int h = 0; h < frame->height / 2; h++)
		{
			for (int w = 0; w < frame->width / 2; w++)
			{
				frame->data[1][h * frame->linesize[1] + w] = 100+i * 4;
				frame->data[2][h * frame->linesize[2] + w] = 100+i * 3;
			}
		}
		frame->pts = i;

		// 发送原始帧
		re = avcodec_send_frame(enc_context, frame);
		if (re != 0) {
			char buf[128];
			av_strerror(re, buf, 128);
			cout << __LINE__ << "error: " << buf << endl;
			break;
		}

		// 接收编码帧
		while (true)
		{
			re = avcodec_receive_packet(enc_context, pkt);
			if (re == 0) {
				ofs.write((char*)pkt->data, pkt->size);
				av_packet_unref(pkt);
			}
			else if (re == AVERROR(EAGAIN) || re == AVERROR(EINVAL)) break; // Resource temporarily unavailable
			else {
				char buf[128];
				av_strerror(re, buf, 128);
				cout << __LINE__ << "error: " << buf << endl;
				break;
			}
		}
	}
}

void yuv_file_to_h264()
{
	ifstream ifs;
	ifs.open(YUV_PATH, ios::binary);
	ofstream ofs;
	ofs.open(OUT2_PATH, ios::binary);

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