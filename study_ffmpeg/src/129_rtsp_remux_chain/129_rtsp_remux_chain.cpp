//////////////////////////////////  @版权说明  //////////////////////////////////////////////////
///						Jiedi(China nanjing)Ltd.                                    
/// @版权说明 代码和课程版权有夏曹俊所拥有并已经申请著作权，此代码可用作为学习参考并可在项目中使用，
/// 课程中涉及到的其他开源软件，请遵守其相应的授权
/// 课程源码不可以直接转载到公开的博客，或者其他共享平台，不可以用以制作在线课程。
/// 课程中涉及到的其他开源软件，请遵守其相应的授权               
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////  源码说明  //////////////////////////////////////////////////
/// 项目名称      : FFmpeg 4.2 从基础实战-多路H265监控录放开发 实训课
/// Contact       : xiacaojun@qq.com
///  博客   :				http://blog.csdn.net/jiedichina
///	视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun		
///			   腾讯课堂		https://jiedi.ke.qq.com/				
///			   csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
///           51cto学院		https://edu.51cto.com/sd/d3b6d
///			   老夏课堂		http://www.laoxiaketang.com 
/// 更多资料请在此网页下载  http://ffmpeg.club
/// ！！！请加入课程qq群 【639014264】与同学交流和下载资料 
/// 微信公众号: jiedi2007
/// 头条号	 : xiacaojun
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// 课程交流qq群 639014264  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <thread>
#include "xtools.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
#include "xmux_task.h"
using namespace std;

//大华
#define CAM1 \
"rtsp://admin:admin@192.168.2.108/cam/realmonitor?channel=1&subtype=0"
//海康
#define CAM2 \
"rtsp://test:x12345678@192.168.2.64/h264/ch1/main/av_stream"

//"rtsp://127.0.0.1:8554/test"
#define CAM3 \
"rtsp://192.168.1.150:8554/live/camera"
int main(int argc, char* argv[])
{
    XDemuxTask demux_task;
    for (;;)
    {
        if (demux_task.Open(CAM3))
        {
            break;
        }
        MSleep(100);
        continue;
    }

    auto vpara = demux_task.CopyVideoPara();
    AVCodecParameters* video_para = nullptr;
    AVCodecParameters* audio_para = nullptr;
    AVRational* video_time_base = nullptr;
    AVRational* audio_time_base = nullptr;
    if (vpara)
    {
        video_para = vpara->para;
        video_time_base = vpara->time_base;
    }
    auto apara = demux_task.CopyAudioPara();
    if (apara)
    {
        audio_para = apara->para;
        audio_time_base = apara->time_base;
    }


    XMuxTask mux_task;
    if (!mux_task.Open("rtsp_out1.mp4", video_para, video_time_base,
        audio_para, audio_time_base
    ))
    {
        LOGERROR("mux_task.Open failed!");
        return -1;
    }

    XDecodeTask decode_task;
    if (!decode_task.Open(video_para))
    {
        LOGERROR("decode_task.Open failed!");
        return -1;
    }

    demux_task.set_next(&decode_task);
    demux_task.Start();
    decode_task.Start();


    //auto frame = decode_task.GetFrame();


    //MSleep(5000);
    //decode_task.Stop();

    //XVideoView* view = XVideoView::Create();
    //view->Init(video_para);
    //view->DrawFrame();

    //demux_task.set_next(&mux_task);
    //demux_task.Start();
    //mux_task.Start();
    //MSleep(5000);
    //mux_task.Stop();
    //if (!mux_task.Open("rtsp_out2.mp4", video_para, video_time_base,
    //    audio_para, audio_time_base
    //))
    //{
    //    LOGERROR("mux_task.Open failed!");
    //    return -1;
    //}
    //mux_task.Start();
    //MSleep(5000);
    //mux_task.Stop();
    getchar();

    return 0;
}

