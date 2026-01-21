#pragma once
#include <mutex>

#ifdef XVIDEO_VIEW_EXPORTS
#define XVIDEO_VIEW_API __declspec(dllexport) 
#else
#define XVIDEO_VIEW_API __declspec(dllimport) 
#endif

typedef struct XRational {
	int num; ///< Numerator
	int den; ///< Denominator
} XRational;
struct AVFormatContext;
class XFormat
{
public:
	virtual AVFormatContext* Open(const char* path) = 0;

	int video_index() { return video_index_; };
	int audio_index() { return audio_index_; };
	XRational timebase_video() { return timebase_video_; };
	XRational timebase_audio() { return timebase_audio_; };

protected:
	std::mutex mut_;
	AVFormatContext* context_ = nullptr;
	int video_index_ = -1;
	int audio_index_ = -1;
	XRational timebase_video_ = {1,25};
	XRational timebase_audio_ = {1,9000};
};