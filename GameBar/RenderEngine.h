#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "Game.h"

class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();
	void renderScene(int firstIndex, int selectedIndex, std::vector<Game> *games);
	SDL_Renderer* getRenderer();
private:
	void renderGame(Game* game, bool selected, SDL_Rect box);

	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Texture *bgTex;
	SDL_Texture *leftTex, *rightTex;
	SDL_Texture *botTextTex, *topTextTex;
	SDL_Rect leftRect, rightRect;
	TTF_Font *gameFontSmall, *gameFontLarge;
};

