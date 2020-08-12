#include "RenderEngine.h"
#include "Constants.h"
#include <iostream>
#include <vector>


RenderEngine::RenderEngine() {
	win = SDL_CreateWindow("Hello World!", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN || SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return;
	}

	//load fonts
	gameFontSmall = TTF_OpenFont(fontFile.c_str(), gameFontSmallSize);
	gameFontLarge = TTF_OpenFont(fontFile.c_str(), gameFontLargeSize);

	//load background image
	bgTex = IMG_LoadTexture(ren, bgFile.c_str());
	if (bgTex == nullptr) {
		std::cout << "Background failed to load" << std::endl;
		return;
	}
	//load arrows
	leftTex = IMG_LoadTexture(ren, leftFile.c_str());
	if (bgTex == nullptr) {
		std::cout << "Left arrow failed to load" << std::endl;
		return;
	}
	rightTex = IMG_LoadTexture(ren, rightFile.c_str());
	if (bgTex == nullptr) {
		std::cout << "Right arrow failed to load" << std::endl;
		return;
	}
	//define arrow render rectangles. These should be on the centerline, 50*50(10% of asset size), on either edge of the screen.
	leftRect.x = 0;
	rightRect.x = SCREEN_WIDTH - ARROW_TEXTURE_WIDTH;
	leftRect.y = rightRect.y = CENTERLINE - (ARROW_TEXTURE_WIDTH / 2);
	leftRect.w = rightRect.w = ARROW_TEXTURE_WIDTH;
	leftRect.h = rightRect.h = ARROW_TEXTURE_WIDTH;
}

RenderEngine::~RenderEngine() {
	//unload arrow textures
	SDL_DestroyTexture(leftTex);
	SDL_DestroyTexture(rightTex);

	//unload background image
	SDL_DestroyTexture(bgTex);

	//destroy fonts
	TTF_CloseFont(gameFontSmall);
	TTF_CloseFont(gameFontLarge);

	//destroy renderer
	SDL_DestroyRenderer(ren);

	//destroy window
	SDL_DestroyWindow(win);
}

void RenderEngine::renderScene(int firstIndex, int selectedIndex, std::vector<Game>* games) {
	SDL_RenderClear(ren);

	//background
	SDL_RenderCopy(ren, bgTex, NULL, NULL);
	//arrows
	if (firstIndex != 0) {
		//we need a left arrow
		SDL_RenderCopy(ren, leftTex, NULL, &leftRect);
	}
	int gamesToRight = games->size() - (firstIndex + GAMES_ON_SCREEN);
	if (gamesToRight > 0) {
		//we need a right arrow
		SDL_RenderCopy(ren, rightTex, NULL, &rightRect);
	}
	//games. Each game gets a box on the center line evenly spaced between the arrows the size of a large image
	SDL_Rect box;
	box.w = LARGE_IMAGE_WIDTH;
	box.h = LARGE_IMAGE_HEIGHT;
	box.x = 60;
	box.y = CENTERLINE - (LARGE_IMAGE_HEIGHT / 2);
	for (int i = firstIndex; i < firstIndex + GAMES_ON_SCREEN; i++) {
		renderGame(&((*games)[i]), i == selectedIndex, box);
		//move box over to next even spacing. (Screen width minus both arrows) divided by number of games.
		box.x += (1920 - 120) / GAMES_ON_SCREEN;
	}

	//Update the screen
	SDL_RenderPresent(ren);
};

void RenderEngine::renderGame(Game* game, bool selected, SDL_Rect box) {
	SDL_Texture* imgTex = game->getImage();
	if (!imgTex) return;
	if (selected) {
		//Draw outer selection box and full size image
		SDL_Rect outer_rect;
		outer_rect.x = box.x - 5;
		outer_rect.y = box.y - 5;
		outer_rect.h = box.h + 10;
		outer_rect.w = box.w + 10;
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderDrawRect(ren, &outer_rect);

		int x_center = box.x + (box.w / 2);

		SDL_RenderCopy(ren, imgTex, NULL, &box);
		//Draw text renders
		//Top Text box contains titleText in large font, is located 5 pixels above and centered over outer_rect 
		SDL_Surface *topText = TTF_RenderText_Blended_Wrapped(gameFontLarge, game->getTopText().c_str(), uiColor, box.w);
		topTextTex = SDL_CreateTextureFromSurface(ren, topText);
		SDL_FreeSurface(topText);
		SDL_Rect topTextBox;
		SDL_QueryTexture(topTextTex, NULL, NULL, &topTextBox.w, &topTextBox.h);
		topTextBox.x = x_center - (topTextBox.w / 2);
		topTextBox.y = outer_rect.y - topTextBox.h - 5;
		SDL_RenderCopy(ren, topTextTex, NULL, &topTextBox);

		//Bottom Text box contains description text in small font, is located 5 pixels below and centered under outer_rect 

		SDL_Surface *botText = TTF_RenderText_Blended_Wrapped(gameFontSmall, game->getBottomText().c_str(), uiColor, box.w);
		botTextTex = SDL_CreateTextureFromSurface(ren, botText);
		SDL_FreeSurface(botText);
		SDL_Rect botTextBox;
		SDL_QueryTexture(botTextTex, NULL, NULL, &botTextBox.w, &botTextBox.h);
		botTextBox.x = x_center - (botTextBox.w / 2);
		botTextBox.y = outer_rect.y + outer_rect.h + 5;
		SDL_RenderCopy(ren, botTextTex, NULL, &botTextBox);
	}
	else {
		//make smaller box to hold smaller image
		SDL_Rect smallbox;
		smallbox.x = box.x + ((box.w - SMALL_IMAGE_WIDTH) / 2);
		smallbox.y = box.y + ((box.h - SMALL_IMAGE_HEIGHT) / 2);
		smallbox.w = SMALL_IMAGE_WIDTH;
		smallbox.h = SMALL_IMAGE_HEIGHT;
		SDL_RenderCopy(ren, imgTex, NULL, &smallbox);
	}
	return;
}

SDL_Renderer* RenderEngine::getRenderer() {
	return ren;
}