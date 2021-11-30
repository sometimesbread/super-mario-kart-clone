#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "RenderWindow.hpp"

int main(int argc, char* args[])
{
	if(SDL_Init(SDL_INIT_VIDEO) > 0) std::cout << "SDL_Init has failed so you need to fix that. SDL_ERROR: " << SDL_GetError() << std::endl;

	if(!IMG_Init(IMG_INIT_PNG)) std::cout << "SDL_IMG_Init has failed so you need to fix that. ERROR: " << SDL_GetError() << std::endl;

	RenderWindow window("Super Mario Kart (SDL)", 256, 224);

	bool running = true;

	SDL_Event event;

	while(running)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT) running = false;
		}
	}

	window.cleanUp();
	SDL_Quit();

	return 0;
}