#include <iostream>
#include <fstream>

//#include "../xvideoview/xvideoview.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
}

//#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib") // 包含AVFrame

#define H264_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.h264"
//#define OUT_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\resize.yuv"
#define OUT_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\400_300_30.h264"

using namespace std;

//#define ENABLE_YUV_400_300_30

// 将h264文件解码，改变像素尺寸400x300，重新编码成h264 
int main()
{
	int width = 640;
	int height = 360;
	int width_dst = 400;
	int height_dst = 300;
	int re = 0;
	int pts = 0;

	ifstream ifs;
	ofstream ofs;
	ifs.open(H264_PATH,ios::binary);
	if (!ifs.is_open()) cout << "ifs open failed";
	ofs.open(OUT_PATH,ios::binary);
	if (!ofs.is_open()) cout << "ofs open failed";

	// 1. 将h264文件解码

	auto dec = avcodec_find_decoder(AV_CODEC_ID_H264);			// 查找解码器
	auto enc = avcodec_find_encoder(AV_CODEC_ID_H264);			// 查找编码器
	auto context_dec = avcodec_alloc_context3(dec);				// 创建解码器上下文
	auto context_enc = avcodec_alloc_context3(enc);				// 创建编码器上下文

	context_enc->width = width_dst;
	context_enc->height = height_dst;
	context_enc->thread_count = 16;
	context_enc->time_base = { 1,25 };
	context_enc->pix_fmt = AV_PIX_FMT_YUV420P;

	avcodec_open2(context_dec, NULL, NULL);				// 打开解码器
	avcodec_open2(context_enc, NULL, NULL);				// 打开编码器

	auto parser_context = av_parser_init(AV_CODEC_ID_H264); 
#ifdef ENABLE_YUV_400_300_30
	auto cache = make_unique<unsigned char[]>(1920 * 1080 * 8); // yuv字节对齐的缓存
#endif // ENABLE_YUV_400_300_30
	unsigned char buf[1024 * 4] = {0,};							// 读取的字节数
	AVPacket* pkt = av_packet_alloc();							// 切割pkt的容器
	AVPacket* pkt_dst = av_packet_alloc();						// 接收pkt的容器
	AVFrame* frame = av_frame_alloc();							// 接收frame的容器
	AVFrame* frame_dst = av_frame_alloc();						// 转换后frame的容器
	frame_dst->width = width_dst;
	frame_dst->height = height_dst;
	frame_dst->format = AV_PIX_FMT_YUV420P;
	av_frame_get_buffer(frame_dst,0);

	SwsContext* context_sws = nullptr;					// 变换上下文

	while (true) // 读取h264文件到内存
	{
		ifs.read((char *)buf, sizeof(buf));
		if (ifs.gcount() == 0) break;
			
		int in_len = sizeof(buf);
		unsigned char* in_data = buf;
		while (in_len) // 可能包含多个avpacket，需要切割获得单个pkt
		{
			int read_size = av_parser_parse2(
				parser_context,context_dec, 
				&pkt->data, &pkt->size,				// 输出
				in_data, in_len,					// 输入
				AV_NOPTS_VALUE, AV_NOPTS_VALUE,0
			);

			in_data += read_size;
			in_len -= read_size;

			if (!pkt->size) continue;

			//cout << "pkt size = " << pkt->size << endl;

			// 发送压缩帧
			re = avcodec_send_packet(context_dec,pkt);
			if (re != 0) {
				//cout << __LINE__ << " avcodec_send_packet failed" << endl;
				break;
			}
		
			// 接收原始帧
			while (true) // 消费者模式可能接收到多个原始帧
			{
				re = avcodec_receive_frame(context_dec,frame);
				if (re != 0) {
					//cout << __LINE__ << " avcodec_receive_frame failed" << endl;
					break;
				}
				// 2 .改变像素尺寸400x300
				context_sws = sws_getCachedContext(					// 获取frame转换上下文，null会创建
					context_sws,
					frame->width, frame->height, (AVPixelFormat)frame->format,	// 原始宽高格式
					width_dst, height_dst, (AVPixelFormat)frame->format,		// 目标宽高格式
					SWS_BILINEAR,												// 抗锯齿算法 flag
					0, 0, 0
				);

				sws_scale(											// 变换尺寸
					context_sws,
					frame->data,frame->linesize,0,frame->height,
					frame_dst->data,frame_dst->linesize
				);
#ifdef ENABLE_YUV_400_300_30
				auto size_y = frame_dst->width * frame_dst->height;
				auto size_u = frame_dst->width / 2 * frame_dst->height / 2;
				for (int i = 0; i < frame_dst->height; i++) // y
				{
					int src_offset = i * frame_dst->linesize[0];		// 源：带padding的偏移
					int dst_offset = i * frame_dst->width;				// 目标：连续存储的偏移
					memcpy(cache.get() + dst_offset,
						frame_dst->data[0] + src_offset,
						frame_dst->width);
				}
				for (int i = 0; i < frame_dst->height / 2; i++) // u
				{
					int src_offset = i * frame_dst->linesize[1];		// 源：带padding的偏移
					int dst_offset = i * frame_dst->width / 2;          // 目标：连续存储的偏移
					memcpy(cache.get() + dst_offset + size_y,
						frame_dst->data[1] + src_offset,
						frame_dst->width / 2);
				}
				for (int i = 0; i < frame_dst->height / 2; i++) // v
				{
					int src_offset = i * frame_dst->linesize[2];		// 源：带padding的偏移
					int dst_offset = i * frame_dst->width / 2;			// 目标：连续存储的偏移
					memcpy(cache.get() + dst_offset + size_y + size_u,
						frame_dst->data[2] + src_offset,
						frame_dst->width / 2);
				}

				ofs.write((char*)cache.get(), frame_dst->width * frame_dst->height * 3 / 2);
#endif // ENABLE_YUV_400_300_30

				static bool is_first = true;
				if (is_first) {
					is_first = false;


				}

				// 3. 重新编码成h264
				re = avcodec_send_frame(context_enc,frame_dst); // 发送原始帧
				if (re != 0) {
					//char buf[128];
					//av_strerror(re,buf,sizeof(buf));
					//cout << "avcodec_send_frame failed ：" << buf << endl;
					break;
				}

				frame_dst->pts = pts++;
				cout << "pts = " << frame_dst->pts << endl;

				while (true) // 接收编码帧
				{
					re = avcodec_receive_packet(context_enc, pkt_dst);
					if (re != 0) break;

					if (pkt_dst->size) {
						//cout << "pkt_dst->size = " << pkt_dst->size << endl;
						ofs.write((char*)pkt_dst->data, pkt_dst->size);
					}
					av_packet_unref(pkt_dst);
				}

				//av_frame_unref(frame);
			}
		}
	}

	//avcodec_send_packet(context_dec, NULL);
	//while (true)
	//{
	//	re = avcodec_receive_frame(context_dec, frame);
	//	if (re != 0) {
	//		break;
	//	}

	//	// 写入文件
	//	if (frame->format == AV_PIX_FMT_YUV420P) {
	//		ofs.write((char*)frame->data[0], frame->linesize[0]);
	//		ofs.write((char*)frame->data[1], frame->linesize[1]);
	//		ofs.write((char*)frame->data[2], frame->linesize[2]);
	//	}
	//}

	// 3. 重新编码成h264 
	avcodec_free_context(&context_dec);
}