#pragma once
#include "xformat.h"

struct AVPacket;
class XVIDEO_VIEW_API XDemux : public XFormat
{
public:
	AVFormatContext* Open(const char* path) override;
	bool Read(AVPacket* pkt);
	bool SetContext(AVFormatContext* context);
	bool Seek(long long pts_begin_video);
};

