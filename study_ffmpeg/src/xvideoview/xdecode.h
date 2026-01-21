#pragma once
#include "xcode.h"

class XVIDEO_VIEW_API XDecode : public XCode
{
public:
    bool SendPkt(AVPacket* pkt);
    bool RecvFrame(AVFrame* frame);
    std::vector<AVFrame*> End();
};