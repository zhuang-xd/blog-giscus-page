#pragma once

#include <QtWidgets/QWidget>
#include "ui_yuvrgbplayer.h"
#include <thread>

#define VIDEO_CNT 2

class XVideoView;
class YuvRgbPlayer : public QWidget
{
    Q_OBJECT

public:
    YuvRgbPlayer(QWidget *parent = nullptr);
    ~YuvRgbPlayer();

    void InitUi();
    void InitFunc();
    QStringList GetVideoInfo(const QString& filePath);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void VideoOpen(int i);
    void VideoSetFmt(int i, QString fmt);
    void VideoSetFps(int i, QString fmt);

signals:
    void UpdateFrame();
    void UpdateFps();

protected slots:
    void OnUpdateFrame();

private:
    Ui::YuvRgbPlayerClass ui;

    int width_[VIDEO_CNT] = {0,};               // 宽
    int height_[VIDEO_CNT] = { 0, };            // 高
    int fps_[VIDEO_CNT] = { 0, };               // fps
    int fmt_[VIDEO_CNT] = { 0, };               // 格式

    XVideoView* view_[VIDEO_CNT] = { 0, };      // 视频
    QList<QLabel*> list_videos_;
    QList<QLabel*> list_fps_;

    std::thread th_render_;         // 渲染线程
    bool is_exited_ = false;        // 渲染线程退出标志
    QString file_path_;
};

