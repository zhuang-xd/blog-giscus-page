#include <iostream>
#include <SDL3/SDL.h>

#define WIDTH	800
#define HEIGHT	300

int main()
{
	// 初始化接口
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "[" << __LINE__ << "]: " << SDL_GetError() << std::endl;
		return -1;
	}

	// 创建窗口
	auto window = SDL_CreateWindow(
		"Hello SDL",
		WIDTH,
		HEIGHT,
		SDL_WINDOW_RESIZABLE
	);

	if (!window)
	{
		std::cout << "[" << __LINE__ << "]: " << SDL_GetError() << std::endl;
		return -1;
	}

	// 创建渲染器
	auto render = SDL_CreateRenderer(window, NULL);
	if (!render) {
		std::cout << "[" << __LINE__ << "]: " << SDL_GetError() << std::endl;
		return -1;
	}

	// 创建纹理
	auto texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	if (!texture)
	{
		std::cout << "[" << __LINE__ << "]: " << SDL_GetError() << std::endl;
		return -1;
	}

	// 创建图像数据
	std::shared_ptr<unsigned char> rgb(new unsigned char[WIDTH * HEIGHT * 4]);
	auto b = rgb.get();
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int index = WIDTH * 4 * i + j * 4;
			// 如果是argb8888格式，则通道顺序如下
			b[index] = 0;		// B
			b[index + 1] = 0;	// G
			b[index + 2] = 255;	// R
			b[index + 3] = 255;	// A

			// 如果是rgba8888格式，则通道顺序如下
			//b[index] = 255;		// R
			//b[index + 1] = 0;		// G
			//b[index + 2] = 0;		// B
			//b[index + 3] = 255;	// A
		}
	}

	// 加载图像为纹理
	SDL_UpdateTexture(texture, NULL, b, WIDTH * 4);

	// 清空渲染器
	SDL_RenderClear(render);

	// 复制纹理到渲染器
	SDL_FRect dstRect = { 0,0,WIDTH,HEIGHT };
	//SDL_RenderCopy( // SDL3中过时
	SDL_RenderTexture( // SDL3新函数
		render,
		texture,
		NULL, // 原图坐标、尺寸
		&dstRect // 目标坐标、尺寸
	);

	// 更新窗口
	SDL_RenderPresent(render);
	SDL_Delay(1500);

	// 关闭流程
	SDL_DestroyTexture(texture); // 销毁材质
	SDL_DestroyRenderer(render); // 销毁渲染器
	SDL_DestroyWindow(window); // 销毁窗口
	SDL_Quit(); 	//关闭SDL
	//std::cin.get();

	return 0;
}