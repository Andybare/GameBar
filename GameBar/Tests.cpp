#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <direct.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

size_t FileCallback(FILE* f, char* ptr, size_t size, size_t nmemb) {
	return fwrite(ptr, size, nmemb, f);
};

int main(int argc, char* argv[]) {
	if (_mkdir(".\\cache") != 0) {
		std::cout << "mkdir failed" << std::endl;
		//return 1;
	}
	try {
		FILE* file = fopen(".\\cache\\1.jpg", "wb");
		if (!file) {
			std::cout << "Error opening file" << std::endl;
			return 1;
		}

		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		using namespace std::placeholders;
		curlpp::options::WriteFunction* writer = new curlpp::options::WriteFunction(std::bind(&FileCallback, file, _1, _2, _3));
		request.setOpt(writer);
		request.setOpt(new curlpp::options::Url("http://mlb.mlb.com/mlb/images/devices/ballpark/1920x1080/1.jpg"));
		request.perform();
	}
	catch (curlpp::LogicError& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	catch (curlpp::RuntimeError& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_Window* win = SDL_CreateWindow("Hello World!", 200, 200, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	std::string imagePath = "C:\\Users\\coast\\OneDrive\\Documents\\Code\\Assets\\1.jpg";
	//std::string imagePath = "http://mlb.mlb.com/mlb/images/devices/ballpark/1920x1080/1.jpg";
	//std::string imagePath = "C:\\Users\\coast\\OneDrive\\Documents\\Code\\Assets\\hello.bmp";
	/*
	SDL_Surface* bmp = SDL_LoadBMP(imagePath.c_str());
	if (bmp == nullptr) {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	*/
	//SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_Texture* tex = IMG_LoadTexture(ren, imagePath.c_str());
	//SDL_FreeSurface(bmp);
	if (tex == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << IMG_GetError() << std::endl;
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Delay(4000);
		SDL_Quit();
		return 1;
	}
	//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
	for (int i = 0; i < 3; ++i) {
		std::cout << "Loop " << i << std::endl;
		//First clear the renderer
		SDL_RenderClear(ren);
		//Draw the texture
		SDL_RenderCopy(ren, tex, NULL, NULL);
		//Update the screen
		SDL_RenderPresent(ren);
		//Take a quick break after all that hard work
		SDL_Delay(1000);
	}
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}