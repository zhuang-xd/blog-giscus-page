#include "test_qt_sdl_merge.h"

#include "SDL3/SDL.h"

test_qt_sdl_merge::test_qt_sdl_merge(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init_ui();
    init_func();

    startTimer(16); // 60fps
}

test_qt_sdl_merge::~test_qt_sdl_merge()
{}

void test_qt_sdl_merge::init_ui()
{
    m_img1 = QImage(":/test_qt_sdl_merge/assets/img1.png");
    m_img2 = QImage(":/test_qt_sdl_merge/assets/img2.png");
    m_width = m_img1.width() + m_img2.width();
    m_height = qMax(m_img1.height(), m_img2.height());
    //m_width = m_img1.width();
    //m_height = m_img1.height();

    m_pixel_size = 4; //单个像素的字节大小
    m_pixels_len = m_height * m_width * m_pixel_size;
    m_pixels = new unsigned char[m_pixels_len]; // 存像素的空间

    m_label_sdl = ui.label;
    m_label_sdl->resize(m_width, m_height);
    m_winId = m_label_sdl->winId();
}

void test_qt_sdl_merge::init_func()
{
    // 1.初始化SDL库
    SDL_Init(SDL_INIT_VIDEO);
    // 2.创建SDL窗口、纹理、渲染器
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER,(void*)m_winId);
    m_sdl_window = SDL_CreateWindowWithProperties(props);
    m_sdl_render = SDL_CreateRenderer(m_sdl_window,NULL);
    m_sdl_texture = SDL_CreateTexture(m_sdl_render,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING ,m_width ,m_height);
}

void test_qt_sdl_merge::timerEvent(QTimerEvent* event)
{
    // 3.把图片添加到纹理，拼接两张图
    memset(m_pixels, 0, m_pixels_len);

    int copy_img1_size = m_img1.width() * m_pixel_size;
    int copy_img2_size = m_img2.width() * m_pixel_size;

    for (int i = 0; i < m_height; i++)
    {
		// 偏移量 = 每一行的起点 = 当前总行数 * 行宽 * 单个像素的字节大小
        int dst_offset = i * m_width * m_pixel_size;
        if (i < m_img1.height())
            // m_pixels为数组起点 + 当前偏移量，复制当前行img1的宽度
            memcpy(m_pixels + dst_offset, m_img1.scanLine(i), copy_img1_size);
        if (i < m_img2.height())
            // m_pixels为数组起点 + 当前偏移量 + img1的宽度，复制当前行img2的宽度
            memcpy(m_pixels + dst_offset + m_img1.width() * m_pixel_size, m_img2.scanLine(i), copy_img2_size);
    }
    
    // 更改透明度
    static int tmp = 255;
    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            int index = (i * m_width + j) * m_pixel_size;
            //m_pixels[index] = 0;         // B
            //m_pixels[index + 1] = 0;     // G
            //m_pixels[index + 2] = 255;   // R
            m_pixels[index + 3] = tmp;   // A
        }
    }
    tmp--;
    if (tmp < 0)
        tmp = 255;

    // 清空渲染器，把纹理拷贝到渲染器，更新纹理
    SDL_RenderClear(m_sdl_render);
    SDL_RenderTexture(m_sdl_render,m_sdl_texture,NULL,NULL);
    SDL_UpdateTexture(m_sdl_texture, NULL, m_pixels, m_width * m_pixel_size);

    // 5.更新窗口
    SDL_RenderPresent(m_sdl_render);
}