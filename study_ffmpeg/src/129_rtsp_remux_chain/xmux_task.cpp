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

#include "xmux_task.h"
extern "C"
{
#include <libavformat/avformat.h>
}
using namespace std;
void XMuxTask::Do(AVPacket* pkt)
{
    pkts_.Push(pkt);
    Next(pkt);
}
void XMuxTask::Main()
{
    xmux_.WriteHead();
    while (!is_exit_)
    {
        unique_lock<mutex> lock(mux_);
        auto pkt = pkts_.Pop();
        if (!pkt)
        {
            MSleep(1);
            continue;
        }

        xmux_.Write(pkt);
        cout << "W"<< flush;

        av_packet_free(&pkt);
    }
    xmux_.WriteEnd();
    xmux_.set_c(nullptr);
}
bool XMuxTask::Open(const char* url,
    AVCodecParameters* video_para,
    AVRational* video_time_base,
    AVCodecParameters* audio_para ,
    AVRational* audio_time_base 
)
{
    auto c = xmux_.Open(url,video_para,audio_para);
    if (!c)return false;
    xmux_.set_c(c);
    xmux_.set_src_video_time_base(video_time_base);
    xmux_.set_src_audio_time_base(audio_time_base);
    return true;
}