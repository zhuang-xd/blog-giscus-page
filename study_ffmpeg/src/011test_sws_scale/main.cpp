#include <iostream>
#include <fstream>

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")

#define YUV_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\640_360_30.yuv"
#define ARGB_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\1280_720_30.argb"
#define YUV2_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\assets\\320_180_30.yuv"

using namespace std;

void yuv_to_rgb();
void rgb_to_yuv();

int main()
{
	//yuv_to_rgb();
	rgb_to_yuv();
}

void yuv_to_rgb()
{
	int width_yuv = 640;
	int height_yuv = 360;
	int width_rgb = 1280;
	int height_rgb = 720;
	ifstream ifs;
	ofstream ofs;
	ifs.open(YUV_PATH, ios::binary);
	ofs.open(ARGB_PATH, ios::binary);


	// frame_buf
	auto buf_y = make_unique<unsigned char[]>(width_yuv * height_yuv);
	auto buf_u = make_unique<unsigned char[]>(width_yuv * height_yuv / 4);
	auto buf_v = make_unique<unsigned char[]>(width_yuv * height_yuv / 4);
	auto buf_rgb = make_unique<unsigned char[]>(width_rgb * height_rgb * 4);

	unsigned char* rgb[] = { buf_rgb.get() };
	unsigned char* yuv[] = { buf_y.get(), buf_u.get(), buf_v.get() };

	int rgb_line_szes[1] = { width_rgb * 4 };
	int yuv_line_sizes[3] = { width_yuv, width_yuv / 2, width_yuv / 2 };


	struct SwsContext* yuv2rgb = nullptr;
	// yuv转rgb
	for (;;)
	{
		// 读取yuv平面数据
		ifs.read((char*)buf_y.get(), width_yuv * height_yuv);
		ifs.read((char*)buf_u.get(), width_yuv * height_yuv / 4);
		ifs.read((char*)buf_v.get(), width_yuv * height_yuv / 4);

		// 检查是否成功读取了所有数据
		if (ifs.gcount() == 0) break;  // 没有读取到任何数据，退出

		// 获取上下文，NULL时会创建上下文
		yuv2rgb = sws_getCachedContext(
			yuv2rgb,									// 上下文
			width_yuv, height_yuv, AV_PIX_FMT_YUV420P,	// 输入数据 宽高格式 
			width_rgb, height_rgb, AV_PIX_FMT_ARGB,		// 输出数据 宽高格式 
			SWS_BILINEAR,								// 抗锯齿算法
			NULL, NULL, NULL							// 过滤器参数
		);

		// 转换
		sws_scale(
			yuv2rgb,		// 上下文
			yuv,			// 输入图像数组
			yuv_line_sizes,	// 输入图像行字节数数组
			0,				// Y轴图像
			height_yuv,
			rgb,			// 输出图像数组
			rgb_line_szes	// 输出图像行字节数
		);

		// 写文件
		ofs.write((char*)buf_rgb.get(), width_rgb * height_rgb * 4);
		static int cnt = 0;
		cnt++;
		cout << "write" << cnt << endl;
	}


	ifs.close();
	ofs.close();
}

void rgb_to_yuv()
{
	// 新建AVFrame对象
	AVFrame* frame_rgb = av_frame_alloc();
	AVFrame* frame_yuv = av_frame_alloc();

	// 填写参数
	frame_rgb->width = 1280;
	frame_rgb->height = 720;
	frame_rgb->format = AV_PIX_FMT_ARGB;
	frame_rgb->linesize[0] = frame_rgb->width * 4;

	frame_yuv->width = 320;
	frame_yuv->height = 180;
	frame_yuv->format = AV_PIX_FMT_YUV420P;
	frame_yuv->linesize[0] = frame_yuv->width;		// y
	frame_yuv->linesize[1] = frame_yuv->width / 2;	// u
	frame_yuv->linesize[2] = frame_yuv->width / 2;	// v

	// 分配缓冲区
	av_frame_get_buffer(frame_rgb, 0);
	av_frame_get_buffer(frame_yuv, 0);

	// 打开文件
	ifstream ifs;
	ofstream ofs;
	ifs.open(ARGB_PATH,ios::binary);
	ofs.open(YUV2_PATH,ios::binary);

	// 新建上下文
	SwsContext* context = nullptr;
	for (;;)
	{
		// 读文件
		ifs.read((char*)frame_rgb->data[0], frame_rgb->width * frame_rgb->height * 4);
		if (ifs.gcount() == 0) break;

		context = sws_getCachedContext(
			context,
			frame_rgb->width, frame_rgb->height, (AVPixelFormat)frame_rgb->format,
			frame_yuv->width, frame_yuv->height, (AVPixelFormat)frame_yuv->format,
			SWS_BILINEAR,
			0, 0, 0
		);

		sws_scale(
			context,
			frame_rgb->data,
			frame_rgb->linesize,
			0,
			frame_rgb->height,
			frame_yuv->data,
			frame_yuv->linesize
		);

		// 写文件
		ofs.write((char*)frame_yuv->data[0], frame_yuv->width * frame_yuv->height);		// y
		ofs.write((char*)frame_yuv->data[1], frame_yuv->width * frame_yuv->height / 4);	// u
		ofs.write((char*)frame_yuv->data[2], frame_yuv->width * frame_yuv->height / 4);	// v

		static int cnt = 0;
		cout << "write" << cnt++ << endl;
	}

	// 关文件
	ifs.close();
	ofs.close();
	av_frame_free(&frame_yuv); // 会自动释放缓冲区
	av_frame_free(&frame_rgb);
}