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

#pragma once
#include "xformat.h"
//////////////////////////////////////
/// 媒体封装

class XMux :public XFormat
{
public:
    //////////////////////////////////////////////////
    //// 打开封装
    static AVFormatContext* Open(const char* url,
        AVCodecParameters* video_para = nullptr,
        AVCodecParameters* audio_para = nullptr
    );

    bool WriteHead();

    bool Write(AVPacket* pkt);

    bool WriteEnd();

    //音视频时间基础
    void set_src_video_time_base(AVRational* tb);
    void set_src_audio_time_base(AVRational* tb);

    ~XMux();
private:
    AVRational* src_video_time_base_ = nullptr;
    AVRational* src_audio_time_base_ = nullptr;

    long long begin_video_pts_ = -1;//原视频开始时间
    long long begin_audio_pts_ = -1;//原音频开始时间
};

