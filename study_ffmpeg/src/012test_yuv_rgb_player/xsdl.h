#pragma once
#include "xvideoview.h"

#include <fstream>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class XSDL :public XVideoView
{
public:
	//bool Draw(
	//	int w, int h,
	//	unsigned char* data_y, int linesize_y,
	//	unsigned char* data_u, int linesize_u,
	//	unsigned char* data_v, int linesize_v
	//) override;

	XSDL();
	XSDL(int w, int h, void* win_id, int fmt = XVideoView::YUV420P);
	bool DrawFrame() override;
	bool Close() override;

private:
	SDL_Window* window_;
	SDL_Renderer* render_;
	SDL_Texture* texture_;
};

