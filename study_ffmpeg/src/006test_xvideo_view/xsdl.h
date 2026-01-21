#pragma once
#include "xvideoview.h"

// 实现XvideoView接口
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class XSDL :public XvideoView
{
public:
	XSDL();
	bool Init(int width, int height, PixFormat fmt, void* win_id = nullptr) override;
	bool Draw(const void* data, int linesize) override;
	bool Clear() override;
	bool IsExit() override;
private:
	SDL_Window* m_sdl_window = nullptr;
	SDL_Renderer* m_sdl_render = nullptr;
	SDL_Texture* m_sdl_texture = nullptr;
};
