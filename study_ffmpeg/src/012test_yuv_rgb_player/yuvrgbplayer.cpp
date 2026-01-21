#include "yuvrgbplayer.h"
#include "xvideoview.h"
#include "qfiledialog.h"
#include "qtimer.h"
#include "qdebug.h"
#include "qmessagebox.h"

using namespace std;

void MSleep(int ms) {
	int start = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		if ((clock() - start) / (CLOCKS_PER_SEC / 1000) >= ms) {
			break;
		}
	}
}

YuvRgbPlayer::YuvRgbPlayer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	InitUi();
	InitFunc();

	// 渲染线程
	th_render_ = std::thread([&]() {
		while (!is_exited_)
		{
			emit UpdateFps();
			emit UpdateFrame();
			MSleep(1);
		}
	});
}

YuvRgbPlayer::~YuvRgbPlayer()
{
	is_exited_ = true;
	th_render_.join();
}

void YuvRgbPlayer::InitUi()
{
	list_videos_.append(ui.label_video);
	list_videos_.append(ui.label_video2);

	for (int i = 0; i < list_videos_.size(); i++)
	{
		fmt_[i] = XVideoView::YUV420P;
		width_[i] = 640;
		height_[i] = 360;
		fps_[i] = 30;

		list_videos_[i]->setFixedSize(width_[i], height_[i]);
		list_videos_[i]->setAlignment(Qt::AlignCenter);
		list_videos_[i]->setStyleSheet("background-color: rgba(0,0,0,150)");

		list_fps_[i] = new QLabel(this);
		list_fps_[i]->setText("fps:--");
		list_fps_[i]->setFixedWidth(120);
		list_fps_[i]->setStyleSheet("background-color: rgba(0,0,0,150); color: white; padding: 2px 5px;");
	}

	ui.lineEdit_width->setText(QString::number(width_[0]));
	ui.lineEdit_height->setText(QString::number(height_[0]));
	ui.lineEdit_fps->setText(QString::number(fps_[0]));
	ui.lineEdit_width2->setText(QString::number(width_[1]));
	ui.lineEdit_height2->setText(QString::number(height_[1]));
	ui.lineEdit_fps2->setText(QString::number(fps_[1]));

	// 立刻获取的话是0,0
	QTimer::singleShot(0, this, [=]() {
	for (int i = 0; i < list_videos_.size(); i++)
	{
		QPoint p = list_videos_[i]->geometry().topLeft();
		list_fps_[i]->move(p.x() + 12, p.y() + 12);
	}
	});
}

void YuvRgbPlayer::InitFunc()
{
	// 更新图片帧
	connect(this, &YuvRgbPlayer::UpdateFrame, this, &YuvRgbPlayer::OnUpdateFrame);

	// 更新fps
	connect(this, &YuvRgbPlayer::UpdateFps, [&]() {
		for (int i = 0; i < VIDEO_CNT; i++)
		{
			if (!view_[i]) continue;
			list_fps_[i]->setText(QString("fps = %1").arg(view_[i]->render_fps()));
		}
	});

	// 设置fps
	connect(ui.lineEdit_fps, &QLineEdit::textChanged, [&](QString text) {
		VideoSetFps(0,text);
	});
	connect(ui.lineEdit_fps2, &QLineEdit::textChanged, [&](QString text) {
		VideoSetFps(1,text);
	});

	// 设置宽度
	connect(ui.lineEdit_width, &QLineEdit::textChanged, [&](QString text) {
		width_[0] = text.toInt();
	});
	// 设置宽度2
	connect(ui.lineEdit_width2, &QLineEdit::textChanged, [&](QString text) {
		width_[1] = text.toInt();
	});

	// 设置高度
	connect(ui.lineEdit_height, &QLineEdit::textChanged, [&](QString text) {
		height_[0] = text.toInt();
	});
	// 设置高度2
	connect(ui.lineEdit_height2, &QLineEdit::textChanged, [&](QString text) {
		height_[1] = text.toInt();
	});

	// 设置格式
	connect(ui.comboBox_fmt, &QComboBox::currentTextChanged, [&](QString text) {
		VideoSetFmt(0, text);
	});
	// 设置格式2
	connect(ui.comboBox_fmt2, &QComboBox::currentTextChanged, [&](QString text) {
		VideoSetFmt(1, text);
	});

	// 打开
	connect(ui.pushButton_open, &QPushButton::clicked, [&]() {
		VideoOpen(0);
	});
	connect(ui.pushButton_open2, &QPushButton::clicked, [&]() {
		VideoOpen(1);
	});
}

void YuvRgbPlayer::VideoSetFps(int i, QString text)
{
	int fps = text.toInt();
	qDebug() << "fps = " << text;
	if (text == "" || fps <= 0 || fps > 300)
		return;
	fps_[i] = fps;
}

void YuvRgbPlayer::VideoSetFmt(int i,QString text)
{
	if (text == "YUV420P") {
		fmt_[i] = XVideoView::YUV420P;
	}
	else if (text == "RGBA") {
		fmt_[i] = XVideoView::RGBA;
	}
	else if (text == "ARGB") {
		fmt_[i] = XVideoView::ARGB;
	}
	else if (text == "BGRA") {
		fmt_[i] = XVideoView::BGRA;
	}
	else if (text == "RGB") {
		fmt_[i] = XVideoView::RGB;
	}
}
void YuvRgbPlayer::VideoOpen(int i)
{
	if (i < 0) return;

	QFileDialog dlg;
	file_path_ = dlg.getOpenFileName();
	qDebug() << "file_path = " << file_path_;

	if (file_path_ == "") {
		return;
	}

	QStringList file_info = GetVideoInfo(file_path_);
	if (file_info[0] != "" && file_info[1] != "" && file_info[2] != "")
	{
		if (i == 0) {
			ui.lineEdit_width->setText(file_info[0]);
			ui.lineEdit_height->setText(file_info[1]);
			ui.lineEdit_fps->setText(file_info[2]);
			if (file_info[3] == "yuv") {
				ui.comboBox_fmt->setCurrentIndex(0);
			}
		}
		else {
			ui.lineEdit_width2->setText(file_info[0]);
			ui.lineEdit_height2->setText(file_info[1]);
			ui.lineEdit_fps2->setText(file_info[2]);
			if (file_info[3] == "yuv") {
				ui.comboBox_fmt2->setCurrentIndex(0);
			}
		}
	}

	// 如果对象已经存在则先删除
	if (view_[i]) {
		view_[i]->Close();
		delete view_[i];
		view_[i] = nullptr;
	}

	// 创建对象
	view_[i] = XVideoView::Create(
		width_[i], height_[i],
		(void*)list_videos_[i]->winId(),
		(XVideoView::PixFormat)fmt_[i],
		XVideoView::SDL
	);
	if (!view_[i]) {
		qDebug() << "create view failed!";
		return;
	}

	// 打开文件
	int re = view_[i]->Open(file_path_.toLocal8Bit().constData());
	if (!re) {
		qDebug() << "open failed!";
		return;
	}
}

void YuvRgbPlayer::resizeEvent(QResizeEvent* event)
{
	for (int i = 0; i < list_videos_.size(); i++)
	{
		// 设置视频窗口大小
		list_videos_[i]->setFixedSize(width_[i], height_[i]);
		if (view_[i])
			view_[i]->Scale(width_[i], height_[i]);

		// 设置fps显示位置
		list_fps_[i]->move(
			list_videos_[i]->geometry().topLeft().x() + 10,
			list_videos_[i]->geometry().topLeft().y() + 20
		);
	}
}

void YuvRgbPlayer::OnUpdateFrame()
{
	static int last_pts[VIDEO_CNT] = {0,};
	static int start[VIDEO_CNT] = {0,};
	
	for (int i = 0; i < VIDEO_CNT; i++)
	{
		if (!view_[i] || fps_[i] < 0) continue;

		// 更新视频帧
		int ms = 1000 / fps_[i]; // 刷新1张图片需要的时间

		// 判断是否到渲染时间
		if ((clock() - last_pts[i]) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			last_pts[i] = clock();
			view_[i]->Read();
			view_[i]->Draw();
		}
	}
}

QStringList YuvRgbPlayer::GetVideoInfo(const QString& filePath)
{
	
	QString fileName = QFileInfo(filePath).fileName();
	QString baseName = fileName.split('.').first();

	QRegularExpression re("^\\d+_\\d+_\\d+\\.[a-zA-Z0-9]+$");
	qDebug() << QString("w = %1, h = %2, fps = %3")
		.arg(baseName.split("_")[0])
		.arg(baseName.split("_")[1])
		.arg(baseName.split("_")[2]);
	if (!re.match(fileName).hasMatch()) {
		return {0,0,0};
	}
	
	QStringList info = baseName.split('_');
	info.append(fileName.split('.').last());
	return info;
}