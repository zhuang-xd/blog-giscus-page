#include <iostream>

// ffmpeg都是c函数，需要用extern
extern "C"
{
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}

using namespace std;

/*
*   av_frame_alloc 创建avframe对象
	av_buffer_get_ref_count 查看avframe的引用计数，使用不会增加引用计数
	av_frame_get_buffer 分配缓冲区，会增加引用计数

*/

int main()
{
	cout <<  "ffmpeg version = " << av_version_info() << endl;

	// 创建对象
	AVFrame * frame = av_frame_alloc();

	// 设置参数
	frame->width = 400;
	frame->height = 300;
	frame->format = AV_PIX_FMT_ARGB;

	// 分配空间-缓冲区
	int re = av_frame_get_buffer(frame,0); // 这里frame的引用计数+1，即为1
	if (re) 
	{
		char errbuf[100];
		av_strerror(re, errbuf, 100);
	}

	// 检查引用计数
	if (frame->buf[0]) // 存储引用计数
		cout << "ref count = " << av_buffer_get_ref_count(frame->buf[0]) << endl;

	// 拷贝frame对象，引用计数也会被拷贝
	/*
		AVFrame *本质就是一个智能指针，除了指向地址，还可以存一些宽高等元数据
		- av_frame_ref，适合只读操作或所有权传递，frame和frame_dst会共享同一片缓冲区
		- av_frame_clone，二者会拥有独立的缓冲区
	*/
	AVFrame * frame_dst = av_frame_alloc();  // 这里frame的引用计数+1，即为2
	av_frame_ref(frame_dst,frame);
	if (frame->buf[0])
		cout << "frame_src ref count = " << av_buffer_get_ref_count(frame->buf[0]) << endl;
	if (frame_dst->buf[0])
		cout << "frame_dst ref count = " << av_buffer_get_ref_count(frame_dst->buf[0]) << endl;

	/*
		手动清空frame的引用计数，但frame_dst是在frame的引用计数基础上+1的来的
		所以此时
		frame = 0
		frame_dst = 1
	*/
	av_frame_unref(frame); 
	if (frame->buf[0])
		cout << "frame_src ref count = " << av_buffer_get_ref_count(frame->buf[0]) << endl;
	if (frame_dst->buf[0])
		cout << "frame_dst ref count = " << av_buffer_get_ref_count(frame_dst->buf[0]) << endl;

	// 释放frame对象
	av_frame_free(&frame);
	av_frame_free(&frame_dst);
}