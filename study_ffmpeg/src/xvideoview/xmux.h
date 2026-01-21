#pragma once
#include "xformat.h"

struct AVCodecParameters;
struct AVPacket;
class XVIDEO_VIEW_API XMux : public XFormat
{
public:
	AVFormatContext* Open(const char* path) override;
	bool WriteHeader();
	bool WriteEnd();
	bool Write(AVPacket* pkt);
	bool SetContext(AVFormatContext* context);
	bool CopyParams(int stream_index, AVCodecParameters *params);
	bool UpdatePkt(AVPacket* pkt, long long pts_offset, XRational timebase_in);
};

