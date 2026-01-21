#include "test_qt_rgb.h"
#include <QPainter>
#include <QDebug>
#include <qrect.h>

#define WIDTH 800
#define HEIGHT 300

test_qt_rgb::test_qt_rgb(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	init_ui();
}

test_qt_rgb::~test_qt_rgb()
{
}

void test_qt_rgb::init_ui()
{
	resize(WIDTH, HEIGHT);
}

void test_qt_rgb::paintEvent(QPaintEvent* event)
{
	// 创建一个空的rgb图像
	QImage img = QImage(WIDTH, HEIGHT, QImage::Format_RGB888);

	// 手动填入rgb数值,rgb是3位一组的，也就是把width会以3为单位等分，不足的补0
	auto data = img.bits(); // 获取图像的首地址

	for (int h = 0; h < HEIGHT; h++) 
	{
		for (int w = 0; w < WIDTH; w++) // 1个像素点中有RGB，24位，3个字节
		{
			// *****************计算像素位置*****************
			// 之前行的像素点x3，即获取之前行RGB数量总和 + 当前行的像素点*3，即获取当前行RGB的偏移量
			int index = h * WIDTH * 3 + w * 3; 

			data[index] = 255;
			data[index+1] = 0;
			data[index+2] = 0;

			// *****************错误点*****************
			//p[w * h] = 255; // 不能这么算，因为这一行的w一直在递增的
		}
	}

	QPainter painter(this);
	painter.begin(this);

	painter.fillRect(rect(), img);

	painter.end();
}