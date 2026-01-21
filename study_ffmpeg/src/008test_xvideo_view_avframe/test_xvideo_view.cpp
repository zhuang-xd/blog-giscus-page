#include "test_xvideo_view.h"
#include "SDL3/SDL.h"
#include "qmessagebox.h"
#include "xvideoview.h"

#define YUV_PATH "C:\\Users\\Shelton\\Workspaces\\code\\vs\\study_ffmpeg\\src\\005test_qt_sdl_play_yuv\\assets\\640_360_30.yuv"

//#define VIEW2_ENABLED
#define YUV_REVESER 
#define AVFRAME

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}


test_xvideo_view::test_xvideo_view(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init_ui();
    init_func();
	startTimer(16);
}

test_xvideo_view::~test_xvideo_view()
{
    
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
    //m_label_sdl->resize(400, 300);
    m_label_sdl->resize(640, 360);
    m_height = m_label_sdl->height();
    m_width = m_label_sdl->width();
}
void test_xvideo_view::init_func()
{

    // 打开yuv文件
    m_file_sdl.setFileName(YUV_PATH);
    if (m_file_sdl.open(QFile::ReadOnly) < 0) {
        QMessageBox::information(this, "Error", "Failed to open file.");
        return;
    }

    m_view = XvideoView::Create(XvideoView::SDL);
    m_view->Init(m_width, m_height, XvideoView::YUV420, (void *)m_label_sdl->winId());
#ifdef VIEW2_ENABLED
    m_view2 = XvideoView::Create();
    m_view2->Init(m_width, m_height, XvideoView::ARGB,nullptr);
#endif

#ifdef AVFRAME
    // 创建对象
    m_yuv_frame = av_frame_alloc();

    // 设置属性
    m_yuv_frame->width = m_width;
    m_yuv_frame->height = m_height;
    m_yuv_frame->format = AV_PIX_FMT_YUV420P;
    m_yuv_frame->linesize[0] = m_width;       // Y
    m_yuv_frame->linesize[1] = m_width / 2;   // U
    m_yuv_frame->linesize[2] = m_width / 2;   // V
    
    // 分配缓冲区
    av_frame_get_buffer(m_yuv_frame, 0);

#endif //AVFRAME
}

//  反转图像
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

void test_xvideo_view::timerEvent(QTimerEvent* event)
{
#ifdef VIEW2_ENABLED
    if (m_view2->IsExit())
    {
		//QMessageBox::information(this, "Info", "Exit.");
        m_view2->Clear();
        m_view->Clear();
        close();
        return;
	}
#endif

    // 3.加载yuv图像帧数据、拷贝到纹理
    int frame_size = m_width * m_height * 3 / 2;
    int frame_buffer_size = m_width * m_height * 2;
	auto frame_buffer = std::make_unique<unsigned char[]>(frame_buffer_size); // 使用智能指针管理内存
    //unsigned char* frame_buffer = new unsigned char[frame_buffer_size];

    // 当文件已经读到末位，在下一次读之前重新回到文件头，实现循环播放
    if (m_file_sdl.atEnd())
    {
        m_file_sdl.seek(0);
	}


#ifdef AVFRAME
    // 读文件到avframe
    m_file_sdl.read((char *)m_yuv_frame->data[0], m_height * m_width);  // 不能只读步长m_width，而是要把这一帧中所有的y都读到
    m_file_sdl.read((char *)m_yuv_frame->data[1], m_height * m_width / 4); 
    m_file_sdl.read((char *)m_yuv_frame->data[2], m_height * m_width / 4); 

    // 绘制
    m_view->DrawFrame(m_yuv_frame);
    //m_view->Draw(
    //    m_yuv_frame->data[0], m_yuv_frame->linesize[0],
    //    m_yuv_frame->data[1], m_yuv_frame->linesize[1],
    //    m_yuv_frame->data[2], m_yuv_frame->linesize[2]
    //);

#else
    m_file_sdl.read((char*)frame_buffer.get(), frame_size);  // 读文件到数组
#ifdef YUV_REVESER
    auto frame_buffer_reverse = std::make_unique<unsigned char[]>(frame_buffer_size);
    reverse_frame_buffer(frame_buffer.get(), frame_buffer_reverse.get());
    m_view->Draw(frame_buffer_reverse.get(), m_width);
#endif // YUV_REVESER
#endif//AVFRAME




#ifdef VIEW2_ENABLED
    // 加载rgb图像帧数据，拷贝到纹理
    int frame_size2 = m_width * m_height * 4;
    int frame_buffer_size2 = m_width * m_height * 4;
    unsigned char* frame_buffer2 = new unsigned char[frame_buffer_size2];

    static int tmp = 255;
    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            int index = i * m_width * 4 + j * 4;
            frame_buffer2[index + 0] = 0;    // B
            frame_buffer2[index + 1] = 0;    // G
            frame_buffer2[index + 2] = 255;  // R
			frame_buffer2[index + 3] = tmp;  // A
        }
    }
    tmp--;
    if (tmp < 0) tmp = 255;

    m_view2->Draw(frame_buffer2, m_width * 4);
	delete[] frame_buffer2; // 手动释放内存
#endif
}