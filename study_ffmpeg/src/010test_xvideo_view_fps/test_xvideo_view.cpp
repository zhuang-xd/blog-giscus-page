#include "test_xvideo_view.h"
#include "SDL3/SDL.h"
#include "qmessagebox.h"
#include "xvideoview.h"
#include <ctime>
#include <qtimer.h>
#include <QSlider>
#include <iostream>
#include <qdebug.h>

#define YUV_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\src\\005test_qt_sdl_play_yuv\\assets\\640_360_30.yuv"

//#define VIEW2_ENABLED
#define YUV_REVESER 
#define AVFRAME
#define FPS

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}

void MSleep(int ms)
{
    auto start = clock();
    for (int i = 0; i < ms; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // 为了防止，因为任务调度，导致延时函数出现的精度问题
        if ((clock() - start) / (CLOCKS_PER_SEC / 1000) >= ms)
            break;
    }
}

test_xvideo_view::test_xvideo_view(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    qDebug() << avcodec_configuration();


    init_ui();
    init_func();

    // 渲染线程
    m_thread_render = std::thread([&]() {
        while (!m_is_exited)
        {
            emit UpdateFrame();
            MSleep(1000 / m_fps_set);
            //std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_fps_set));
        }
    });
}

void test_xvideo_view::OnUpdateFrame()
{
    // 3.加载yuv图像帧数据、拷贝到纹理
    int frame_size = m_width * m_height * 3 / 2;
    int frame_buffer_size = m_width * m_height * 2;
    auto frame_buffer = std::make_unique<unsigned char[]>(frame_buffer_size); // 使用智能指针管理内存

    // 当文件已经读到末位，在下一次读之前重新回到文件头，实现循环播放
    if (m_file_sdl.atEnd())
    {
        m_file_sdl.seek(0);
    }

    // 读文件到avframe
    m_file_sdl.read((char*)m_yuv_frame->data[0], m_height * m_width);  // 不能只读步长m_width，而是要把这一帧中所有的y都读到
    m_file_sdl.read((char*)m_yuv_frame->data[1], m_height * m_width / 4);
    m_file_sdl.read((char*)m_yuv_frame->data[2], m_height * m_width / 4);

    // 绘制
    m_view->DrawFrame(m_yuv_frame);

    // 更新fps
    m_fps++;
    if (m_start_time <= 0)
    {
        m_start_time = clock();
    }
    else if ((clock() - m_start_time) / (CLOCKS_PER_SEC / 1000) >= 1000)
    {
        emit UpdateFps();
        m_start_time = clock();
        m_fps = 0;
    }
}

test_xvideo_view::~test_xvideo_view()
{
    m_is_exited = true;
    m_thread_render.join();
}

void test_xvideo_view::resizeEvent(QResizeEvent* event)
{
    m_label_sdl->resize(size());
    m_label_sdl->move(0, 0);
    m_view->Scale(m_label_sdl->width(),m_label_sdl->height());
}

void test_xvideo_view::init_ui()
{
    m_label_sdl = ui.label;
    m_label_sdl->resize(640, 360);
    m_height = m_label_sdl->height();
    m_width = m_label_sdl->width();

    // 展示fps
    m_label_fps = new QLabel(this);
    m_label_fps->setText(QString("fps = %1").arg(m_fps_set));
    m_label_fps->setObjectName("fpsLabel");
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_label_fps->setStyleSheet("#fpsLabel{ background-color: transparent; color:white}");

    // 展示调节fps
    m_slider_fps = new QSlider(Qt::Horizontal, this);
    m_slider_fps->move(width()-100,0);
    m_slider_fps->setRange(1, 300);
    m_slider_fps->setValue(m_fps_set);

    // 展示调节fps-标签
    m_label_slider_fps = new QLabel(this);
    m_label_slider_fps->setText(QString::number(m_fps_set));
    m_label_slider_fps->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_label_slider_fps->move(width() - 100, 50);
    m_label_slider_fps->setObjectName("m_label_slider_fps");
    m_label_slider_fps->setStyleSheet("#m_label_slider_fps{ background-color: brown; color:white}");
}

void test_xvideo_view::init_func()
{
    connect(this,&test_xvideo_view::UpdateFps,this, [=]() {
        m_label_fps->setText(QString("fps = %1").arg(m_fps));
    });

    connect(this,&test_xvideo_view::UpdateFrame,this, &test_xvideo_view::OnUpdateFrame);

    connect(m_slider_fps, &QSlider::valueChanged, [&](int value) {
        m_fps_set = value;
        m_label_slider_fps->setText(QString("%1").arg(m_fps_set));
    });

    // 打开yuv文件
    m_file_sdl.setFileName(YUV_PATH);
    if (m_file_sdl.open(QFile::ReadOnly) < 0) {
        QMessageBox::information(this, "Error", "Failed to open file.");
        return;
    }

    // 初始化XvideoView
    m_view = XvideoView::Create(XvideoView::SDL);
    m_view->Init(m_width, m_height, XvideoView::YUV420, (void *)m_label_sdl->winId());

    // 创建对象
    m_yuv_frame = av_frame_alloc();

    // 设置属性
    m_yuv_frame->width = m_width;
    m_yuv_frame->height = m_height;
    m_yuv_frame->format = AV_PIX_FMT_YUV420P;
    m_yuv_frame->linesize[0] = m_width;       // Y
    m_yuv_frame->linesize[1] = m_width / 2;   // U
    m_yuv_frame->linesize[2] = m_width / 2;   // V

    // 这里调用会报错，因为av_frame_alloc只是创建了AVFrame这个结构体，并没有创建AVBuffer，而引用计数是在AVBuffer中的
    //qDebug() << "m_yuv_frame->buf[0] ref = " << av_buffer_get_ref_count(m_yuv_frame->buf[0]); 

    // 分配缓冲区
    av_frame_get_buffer(m_yuv_frame, 0);

    // 查看引用计数
    qDebug() << "m_yuv_frame->buf[0] ref = " << av_buffer_get_ref_count(m_yuv_frame->buf[0]); // y分量
}

/////////  反转图像
/*
    yuv420p数据格式是：
    Y0  Y1  Y2  Y3
    Y4  Y5  Y6  Y7
    Y8  Y9  Y10 Y11
    Y12 Y13 Y14 Y15
    U0  U1  U2  U3
    V0  V1  V2  V3  
    因此反转就是
    Y3  Y2  Y1  Y0
    Y7  Y6  Y5  Y4
    Y11 Y10 Y9  Y8
    Y15 Y14 Y13 Y12
    U3  U2  U1  U0  
    V3  V2  V1  V0
*/
void test_xvideo_view::reverse_frame_buffer(unsigned char* src, unsigned char* dst)
{
    int frame_buffer_size = m_width * m_height * 2;
    memset(dst, 0, frame_buffer_size);

    // y分量
    for (int row = 0; row < m_height; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            int src_index = row * m_width + col;
            int dst_index = row * m_width + (m_width - col - 1);
            dst[dst_index] = src[src_index];
        }
    }

    // u分量
    for (int row = m_height; row < m_height * 5 / 4; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            int src_index = row * m_width + col;
            int dst_index = row * m_width + (m_width - col - 1);
            dst[dst_index] = src[src_index];
        }
    }

    // v分量
    for (int row = m_height + m_height * 1 / 4; row < m_height *3 / 2; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            int src_index = row * m_width + col;
            int dst_index = row * m_width + (m_width - col - 1);
            dst[dst_index] = src[src_index];
        }
    }
}