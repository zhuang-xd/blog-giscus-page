#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FFmpeg 库
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h> // For av_gettime
}

// SDL2 库
#include <SDL3/SDL.h>

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"SDL3.lib")

// 错误处理宏
#define CHECK_FFMPEG_ERROR(ret, func_name) \
    if (ret < 0) { \
        char errbuf[AV_ERROR_MAX_STRING_SIZE]; \
        av_strerror(ret, errbuf, sizeof(errbuf)); \
        fprintf(stderr, "FFmpeg Error in %s: %s\n", func_name, errbuf); \
        return -1; \
    }

// RTSP 流地址 (你可以替换成你自己的)
// 这是一个公共的测试流，可能会不稳定或失效
// const char *rtsp_url = "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4";
// 另一个公共测试流，可能更稳定：
//const char* rtsp_url = "rtsp://rtspstream:12345@62.176.103.186:554/live/ch00_0"; // 示例，可能需要用户名密码或不可用

// 建议使用你自己的 IP 摄像头的 RTSP 地址
const char* rtsp_url = "rtsp://192.168.1.150:8554/live/camera";

int main(int argc, char* argv[]) {
    // FFmpeg 变量
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* video_codec_ctx = NULL;
    const AVCodec* video_codec = NULL;
    AVPacket* packet = NULL;
    AVFrame* frame = NULL;
    AVFrame* frame_yuv = NULL; // 用于SWScaler转换后的YUV帧
    struct SwsContext* sws_ctx = NULL;

    int video_stream_idx = -1;
    uint8_t* buffer_yuv = NULL;
    int num_bytes_yuv;

    // SDL 变量
    SDL_Window* screen = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    SDL_Event event;

    // 同步变量
    int64_t start_time = 0; // 记录开始播放的时间
    double pts_time_base; // 视频流的时间基准，用于将PTS转换为秒

    // 检查命令行参数，允许用户传入RTSP URL
    if (argc > 1) {
        rtsp_url = argv[1];
    }
    else {
        printf("Usage: %s [rtsp_url]\n", argv[0]);
        printf("Using default RTSP URL: %s\n", rtsp_url);
    }

#if 0
    // 1. 初始化 FFmpeg 相关组件
    avformat_network_init(); // 初始化网络库，用于处理RTSP

    // 1.1 打开 RTSP 流
    printf("Opening RTSP stream: %s\n", rtsp_url);
    AVDictionary* opts = NULL;
    // 设置超时时间，单位微秒 (5秒)
    av_dict_set(&opts, "stimeout", "5000000", 0);
    // 尝试使用TCP传输，更稳定
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
#endif

    /*
        解封装
    */

    //int ret = avformat_open_input(&fmt_ctx, rtsp_url, NULL, &opts);
    int ret = avformat_open_input(&fmt_ctx, rtsp_url, NULL, NULL);
    //av_dict_free(&opts); // 释放字典
    CHECK_FFMPEG_ERROR(ret, "avformat_open_input");
    printf("RTSP stream opened.\n");

    // 1.2 获取流信息
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    CHECK_FFMPEG_ERROR(ret, "avformat_find_stream_info");
    printf("Stream info found.\n");

    // 1.3 查找视频流
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }
    if (video_stream_idx == -1) {
        fprintf(stderr, "No video stream found in the RTSP stream.\n");
        return -1;
    }
    printf("Video stream found at index %d.\n", video_stream_idx);


    /*
        解解码
    */


    // 1.4 获取视频解码器
    AVCodecParameters* codec_params = fmt_ctx->streams[video_stream_idx]->codecpar;
    video_codec = avcodec_find_decoder(codec_params->codec_id);
    if (!video_codec) {
        fprintf(stderr, "Unsupported video codec!\n");
        return -1;
    }

    // 1.5 创建解码器上下文
    video_codec_ctx = avcodec_alloc_context3(video_codec);
    if (!video_codec_ctx) {
        fprintf(stderr, "Failed to allocate video codec context.\n");
        return -1;
    }
    ret = avcodec_parameters_to_context(video_codec_ctx, codec_params);
    CHECK_FFMPEG_ERROR(ret, "avcodec_parameters_to_context");

    // 1.6 打开解码器
    ret = avcodec_open2(video_codec_ctx, video_codec, NULL);
    CHECK_FFMPEG_ERROR(ret, "avcodec_open2");
    printf("Video decoder opened. Resolution: %dx%d\n", video_codec_ctx->width, video_codec_ctx->height);


    /*
        创建容器
    */

    // 1.7 初始化 AVPacket 和 AVFrame
    packet = av_packet_alloc();
    frame = av_frame_alloc();
    frame_yuv = av_frame_alloc();
    if (!packet || !frame || !frame_yuv) {
        fprintf(stderr, "Failed to allocate AVPacket or AVFrame.\n");
        return -1;
    }
#if 1
    // 1.8 初始化 SWScaler (用于将解码后的帧转换为YUV420P)
    // SDL_PIXELFORMAT_IYUV 对应 AV_PIX_FMT_YUV420P
    sws_ctx = sws_getContext(
        video_codec_ctx->width, video_codec_ctx->height, video_codec_ctx->pix_fmt,
        video_codec_ctx->width, video_codec_ctx->height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL
    );
    if (!sws_ctx) {
        fprintf(stderr, "Failed to initialize SwsContext.\n");
        return -1;
    }

    // 1.9 为 SWScaler 的输出帧分配内存
    num_bytes_yuv = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, video_codec_ctx->width, video_codec_ctx->height, 1);
    buffer_yuv = (uint8_t*)av_malloc(num_bytes_yuv);
    if (!buffer_yuv) {
        fprintf(stderr, "Failed to allocate YUV buffer.\n");
        return -1;
    }
    av_image_fill_arrays(frame_yuv->data, frame_yuv->linesize, buffer_yuv,
        AV_PIX_FMT_YUV420P, video_codec_ctx->width, video_codec_ctx->height, 1);
#endif

    // 计算视频流的时间基准 (秒/单位)
    pts_time_base = av_q2d(fmt_ctx->streams[video_stream_idx]->time_base);

    // 2. 初始化 SDL3
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    // 2.1 创建 SDL 窗口
    screen = SDL_CreateWindow("Default SDL Window", video_codec_ctx->width, video_codec_ctx->height, SDL_WINDOW_RESIZABLE);
    if (!screen) {
        fprintf(stderr, "SDL: could not create window - exiting: %s\n", SDL_GetError());
        return -1;
    }

    // 2.2 创建 SDL 渲染器
    renderer = SDL_CreateRenderer(screen, NULL);
    if (!renderer) {
        fprintf(stderr, "SDL: could not create renderer - exiting: %s\n", SDL_GetError());
        return -1;
    }

    // 2.3 创建 SDL 纹理 (用于显示 YUV420P 数据)
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_IYUV, // IYUV == YUV420P
        SDL_TEXTUREACCESS_STREAMING,
        video_codec_ctx->width,
        video_codec_ctx->height
    );
    if (!texture) {
        fprintf(stderr, "SDL: could not create texture - exiting: %s\n", SDL_GetError());
        return -1;
    }

    // 3. 主循环：读取、解码、渲染
    printf("Starting playback loop...\n");
    int quit = 0;
    while (!quit) {
        // 处理 SDL 事件
        SDL_PollEvent(&event);
        switch (event.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            quit = 1;
            break;
        default:
            break;
        }

        // 从流中读取一个数据包
        ret = av_read_frame(fmt_ctx, packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF || avio_feof(fmt_ctx->pb)) {
                printf("End of stream (EOF).\n");
                quit = 1; // 读到文件末尾或流结束
            }
            else if (ret == AVERROR(EAGAIN)) {
                // 暂时没有数据，等待一下
                SDL_Delay(10);
                continue;
            }
            else {
                CHECK_FFMPEG_ERROR(ret, "av_read_frame");
            }
        }

        if (packet->stream_index == video_stream_idx) {
            // 将数据包发送到解码器
            ret = avcodec_send_packet(video_codec_ctx, packet);
            if (ret < 0) {
                //fprintf(stderr, "Error sending packet to decoder: %s\n", av_err2str(ret));
                av_packet_unref(packet);
                continue;
            }

            // 从解码器接收解码后的帧
            while (ret >= 0) {
                ret = avcodec_receive_frame(video_codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break; // 需要更多数据包或已解码完所有帧
                }
                else if (ret < 0) {
                    //fprintf(stderr, "Error receiving frame from decoder: %s\n", av_err2str(ret));
                    break;
                }
#if 1
                // 帧解码成功，进行处理
                // 3.1 视频同步
                if (start_time == 0) {
                    start_time = av_gettime(); // 记录第一帧的播放时间
                }
#if 0
                // 计算当前帧的期望播放时间 (微秒)
                double pts_in_seconds = frame->pts * pts_time_base;
                int64_t pts_in_microseconds = (int64_t)(pts_in_seconds * 1000000);

                // 获取当前时间 (微秒)
                int64_t current_time = av_gettime() - start_time;

                // 计算需要延迟的时间
                int64_t delay_microseconds = pts_in_microseconds - current_time;

                if (delay_microseconds > 0) {
                    SDL_Delay(delay_microseconds / 1000); // SDL_Delay 接受毫秒
                }
#endif
#endif

 /*               sws_ctx = sws_getCachedContext(
                    sws_ctx,
                    frame_yuv->width, frame_yuv->height, (AVPixelFormat)frame_yuv->format,
                    frame_yuv->width, frame_yuv->height, AV_PIX_FMT_YUV420P,
                    SWS_BILINEAR,
                    0, 0, 0
                );*/

                // 3.2 转换帧格式为 YUV420P
                sws_scale(
                    sws_ctx,
                    (const uint8_t* const*)frame->data, frame->linesize, 0, video_codec_ctx->height,
                    frame_yuv->data, frame_yuv->linesize
                );

                // 3.3 更新 SDL 纹理
                SDL_UpdateYUVTexture(
                    texture,
                    NULL, // 整个纹理
                    frame_yuv->data[0], frame_yuv->linesize[0], // Y plane
                    frame_yuv->data[1], frame_yuv->linesize[1], // U plane
                    frame_yuv->data[2], frame_yuv->linesize[2]  // V plane
                );

                // 3.4 清除渲染器，复制纹理到渲染器，并显示
                SDL_RenderClear(renderer);
                SDL_RenderTexture(renderer,texture,NULL,NULL);
                SDL_RenderPresent(renderer);

                av_frame_unref(frame); // 释放帧引用
            }
        }
        av_packet_unref(packet); // 释放数据包引用
    }

cleanup:
    printf("Cleaning up resources...\n");
    // 4. 清理资源
    if (packet) av_packet_free(&packet);
    if (frame) av_frame_free(&frame);
    if (frame_yuv) av_frame_free(&frame_yuv);
    if (buffer_yuv) av_free(buffer_yuv);
    if (sws_ctx) sws_freeContext(sws_ctx);
    if (video_codec_ctx) avcodec_free_context(&video_codec_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
    avformat_network_deinit(); // 关闭网络库

    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (screen) SDL_DestroyWindow(screen);
    SDL_Quit();

    printf("Cleanup complete. Exiting.\n");
    return 0;
}
