#pragma once
#include "xcode.h"

class XVIDEO_VIEW_API XEncode : public XCode
{
public:
	AVPacket* Encode(AVFrame* frame);
	std::vector<AVPacket*> End();
};