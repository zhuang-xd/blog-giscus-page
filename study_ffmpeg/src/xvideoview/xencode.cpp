#include "xencode.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
}

using namespace std;

AVPacket* XEncode::Encode(AVFrame* frame)
{
	unique_lock<mutex> lock(mut_);

	if (!context_) return nullptr;

	int re = avcodec_send_frame(context_, frame);
	if (re != 0) {
		//PrintErr(re);
		return nullptr;
	}

	AVPacket* pkt = av_packet_alloc();
	while (true)
	{
		int re = avcodec_receive_packet(context_, pkt);
		if (re == 0)
		{
			return pkt;
		}
		else if (re == AVERROR(ENOMEM) || re == AVERROR(EINVAL))
		{
			break;
		}
		else {
			//PrintErr(re);
			break;
		}
	}

	av_packet_free(&pkt);
	return nullptr;
}

std::vector<AVPacket*> XEncode::End()
{
	unique_lock<mutex> lock(mut_);

	vector<AVPacket*> pkt_vector;
	if (!context_) return pkt_vector;

	int re = avcodec_send_frame(context_, NULL);
	if (re != 0) {
		return pkt_vector;
	}

	while (true)
	{
		AVPacket* pkt = av_packet_alloc();

		int re = avcodec_receive_packet(context_, pkt);
		if (re == 0) {
			pkt_vector.push_back(pkt);
		}
		else if (re == AVERROR_EOF || re == AVERROR(EINVAL))
		{
			break;
		}
		else {
			PrintErr(re);
			break;
		}
	}

	return pkt_vector;
}