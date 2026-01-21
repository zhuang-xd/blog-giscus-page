#include <iostream>
#include <fstream>
#include "../xvideoview/xvideoview.h"
#include "../xvideoview/xdecode.h"
#include "../xvideoview/xencode.h"

using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#pragma comment(lib,"../../bin/Win32/Release/xvideoview.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

#define PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\test.h264"
#define H264_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\full.h264"

void test_encode();
void test_decode();

int main()
{
	//test_encode();
	test_decode();
}


void test_decode()
{
	static bool is_first = true;
	XVideoView* view = nullptr;
	XDecode xdec;

	int re = 0;			// 返回值
	int width = 640;
	int height = 360;

	ifstream ifs;
	ifs.open(H264_PATH, ios::binary);
	if (!ifs.is_open()) {
		cout << "open failed" << endl;
	}

	// 查找解码器,创建上下文
	auto context = xdec.Create(AV_CODEC_ID_H264,false);

	//context->thread_count = 16; // 开启多线程

	AVBufferRef* hw_context = nullptr;
	AVHWDeviceType hw_type = AV_HWDEVICE_TYPE_DXVA2;
	re = av_hwdevice_ctx_create(&hw_context, hw_type, NULL, NULL, 0);
	if (re != 0) {
		char buf[128];
		av_strerror(re, buf, sizeof(buf));
		cout << "av_hwdevice_ctx_create failed " << buf << endl;
	}

	context->hw_device_ctx = hw_context; // 在上下文中设置开启硬件加速


	xdec.SetContext(context); // 设置XDecode类的上下文

	// 打开解码器
	xdec.Open();

	unsigned char buf[4 * 1024];
	AVFrame* frame = av_frame_alloc();
	AVFrame* hw_frame = av_frame_alloc();
	AVFrame* p_frame = frame;

	auto start = NowMs();
	static int count = 0;

	// 初始化文件切割
	AVCodecParserContext* h264_context = av_parser_init(AV_CODEC_ID_H264);

	AVPacket* pkt = av_packet_alloc();

	// 循环读文件
	while (true)
	{
		ifs.read((char*)buf, sizeof(buf));

		int cur_buf_size = ifs.gcount(); // 最后一次读到的文件可能就不是4096大小了，所以用变量存
		// 检查是否有读到数据
		if (cur_buf_size == 0)
		{
			cout << "ifs.gcount() == 0" << endl;
			break;
		}
		else {
			//cout << "cur_buf_size =" << cur_buf_size << endl;
		}

		unsigned char* buf_offset = buf; // 数组名是不能移动的，用一个指针变量来偏移

		// 4096个字节中可能有多个压缩帧
		while (true)
		{
			// 切割avpacket，存在缓冲区，上一次未完全读完的数据会在下一次调用的时候继续
			int input_size = av_parser_parse2(		// return 使用过输入的字节大小
				h264_context, context,				// 上下文
				&pkt->data, &pkt->size,				// 输出
				buf_offset, cur_buf_size,			// 输入
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0	// 时间戳
			);

			buf_offset += input_size;
			cur_buf_size -= input_size;

			if (cur_buf_size <= 0) break;
			if (!pkt->size) break;

			// 发送编码帧
			if (xdec.SendPkt(pkt) < 0) break;
			//re = avcodec_send_packet(context, pkt);
			//if (re != 0) {
			//	char buf[128];
			//	av_strerror(re, buf, sizeof(buf));
			//	cout << "avcodec_send_packet failed " << buf << endl;
			//	break;
			//}

			// 发送1个packet可能会，接收到多个原始帧
			while (true)
			{
				// 会自动分配缓冲区，不用手动调用av_frame_get_buffer
				if (!xdec.RecvFrame(frame)) break;
				//re = avcodec_receive_frame(context, frame);
				//if (re == 0) {
				//}
				//else if (re == AVERROR(EINVAL) || re == AVERROR(EAGAIN))
				//{
				//	break;
				//}
				//else
				//{
				//	char buf[128];
				//	av_strerror(re, buf, sizeof(buf));
				//	cout << "avcodec_receive_frame failed " << buf << endl;
				//	break;
				//}
				int ret = av_hwframe_transfer_data(hw_frame, frame, 0);// 转换成硬件加速支持的frame格式nv12
				if (ret == 0) {
					// 转换成功就使用硬件加速的frame
					p_frame = hw_frame;
					cout << "hw_frame fmt = " << p_frame->format << endl; // 打印出来是23 = AV_PIX_FMT_NV12

					if (is_first)
					{
						cout << "hw decode" << endl;
						is_first = false;
						view = XVideoView::Create(frame->width, frame->height, nullptr, XVideoView::NV12);
					}

					auto cache = make_unique<unsigned char[]>(1920 * 1080 * 4 * 3 / 2);

					if (p_frame->linesize[0] == p_frame->width) {
						int y_size = p_frame->width * p_frame->height;
						int uv_size = p_frame->width * p_frame->height / 2;
						memcpy(cache.get(), p_frame->data[0], y_size);				// y平面
						memcpy(cache.get() + y_size, p_frame->data[1], uv_size);	// uv平面
					}
					else {
						// 逐行扫描方式，强制对齐到frame的width宽度
						for (int i = 0; i < p_frame->height; i++) // y平面
						{
							auto offset = p_frame->width * i;
							memcpy(cache.get() + offset, p_frame->data[0] + offset, p_frame->width);
						}
						// uv平面，宽度一致，只需要高度的一半就可以了,如果只有u平面则为height / 4
						for (int i = 0; i < p_frame->height / 2; i++)
						{
							auto offset = p_frame->width * i;
							memcpy(cache.get() + offset + p_frame->width * p_frame->height,
								p_frame->data[1] + offset,
								p_frame->width
							);
						}
					}


					view->Draw(cache.get(), p_frame->width);
				}
				else {

					if (is_first)
					{
						cout << "sw decode" << endl;
						is_first = false;
						view = XVideoView::Create(frame->width, frame->height, nullptr, XVideoView::YUV420P);
					}

					view->Draw(
						p_frame->data[0], p_frame->linesize[0],
						p_frame->data[1], p_frame->linesize[1],
						p_frame->data[2], p_frame->linesize[2]
					);
					//	SDL_UpdateYUVTexture(
					//		texture_, NULL,
					//		p_frame->data[0], p_frame->linesize[0],
					//		p_frame->data[1], p_frame->linesize[1],
					//		p_frame->data[2], p_frame->linesize[2]
					//	);
					//}

					//SDL_RenderClear(render_);
					//SDL_RenderTexture(render_, texture_, NULL, NULL);
					//SDL_RenderPresent(render_);


					//cout << "count = " << count << endl;

					count++;
					auto end = NowMs();
					//cout << "ms = " << end - start << endl;

					if (end - start >= 100) {
						cout << "fps = " << count * 10 << endl;
						count = 0;
						start = end;
					}
				}
			}
		}
	}

	// 缓冲区取出剩余的压缩帧数
	//auto list = xdec.End();
	//for (auto f : list)
	//{
	//	if (f) {
	//		view->Draw(f->data[0], f->width);
	//	}
	//}

	//avcodec_send_packet(context, NULL);
	//while (true)
	//{
	//	re = avcodec_receive_frame(context, p_frame);
	//	if (re == 0) {
	//		SDL_UpdateYUVTexture(
	//			texture_, NULL,
	//			p_frame->data[0], p_frame->linesize[0],
	//			p_frame->data[1], p_frame->linesize[1],
	//			p_frame->data[2], p_frame->linesize[2]
	//		);
	//		SDL_RenderClear(render_);
	//		SDL_RenderTexture(render_, texture_, NULL, NULL);
	//		SDL_RenderPresent(render_);
	//		cout << "end " << p_frame->format << endl;
	//	}
	//	else {
	//		break;
	//	}
	//}

	cout << "closing..." << endl;

	ifs.close();
	view->Close();
	//avcodec_free_context(&context);
	//av_packet_free(&pkt);
	//av_frame_free(&frame);
	//av_parser_close(h264_context);
}

void test_encode()
{
	ofstream ofs;
	ofs.open(PATH, ios::binary);
	if (ofs.is_open()) {
		cout << "is open" << endl;
	}

	XEncode xenc;
	auto context = xenc.Create(27,true);
	context->width = 480;
	context->height = 360;
	context->pix_fmt = AV_PIX_FMT_YUV420P;
	context->thread_count = 16;
	context->time_base = { 1,25 };

	xenc.SetContext(context);
	xenc.SetContextOption("crf", 18);
	xenc.Open();
	auto frame = xenc.CreateFrame();
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
		auto pkt = xenc.Encode(frame);
		if (pkt) {
			cnt++;
			ofs.write((char*)pkt->data, pkt->size);
		}

	}

	// 取出缓冲区
	auto list = xenc.End();
	for (auto pkt : list)
	{
		if (pkt) {
			cnt++;
			ofs.write((char*)pkt->data, pkt->size);
		}
	}
#endif

	cout << "cnt = " << cnt << endl;

	//av_frame_free(&frame);
	ofs.close();
	xenc.Close();
}