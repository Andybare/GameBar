#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <direct.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <json/json.h>
#include "Game.h"
#include "Constants.h"
#include "RenderEngine.h"

std::vector<Game> games;

RenderEngine* engine;

int firstDisplayedIndex;
int selectedIndex;
bool updateImgCache; //used to indicate a cache check after render
bool quit;
bool moveRequested;

void setup();
void cleanup();
void run();
void moveLeft();
void moveRight();
void checkCache();

int main(int argc, char* argv[]) {
	setup();	//makes cache directory, downloads+parses json, makes games, initializes display
	run();		//renders display, monitors for inputs, updates games
	cleanup();	
	return 0;
}

void setup() {
	//make cache directory
	if (_mkdir(cacheDir.c_str()) != 0) {
		std::cout << "mkdir failed" << std::endl;
		//return 1;
	}

	//download background image and json file
	std::stringstream jsonString;

	try {
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		//json file
		request.setOpt(new curlpp::options::Url(jsonUrl));

		jsonString << request;

		//background image
		FILE* file = fopen(bgFile.c_str(), "wb");
		if (!file) {
			std::cout << "Error opening background file for writing" << std::endl;
			return;
		}
		using namespace std::placeholders;
		curlpp::options::WriteFunction* writer = new curlpp::options::WriteFunction(std::bind(&FileCallback, file, _1, _2, _3));
		request.setOpt(writer);
		request.setOpt(new curlpp::options::Url(backgroundUrl));
		request.perform();
		fclose(file);
	}
	catch (curlpp::LogicError& e) {
		std::cout << e.what() << std::endl;
		//TODO: What now?
	}
	catch (curlpp::RuntimeError& e) {
		std::cout << e.what() << std::endl;
		//TODO: What now?
	}

	//setup SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}
	if (TTF_Init() != 0) {
		std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	engine = new RenderEngine();
	
	//parse json
	Json::Value root;
	jsonString >> root;
	int gameCount = root.get("totalGames", 0).asInt();
	if (gameCount > 0) {
		//sanitize json elements
		if (!root["dates"].isArray() || !root["dates"][0]["games"].isArray() || root["dates"][0]["games"].size() != gameCount) {
			std::cout << "Json parse error" << std::endl;
			return;
		}
		//Construct list of games
		for (Json::Value i : root["dates"][0]["games"]) {
			games.emplace_back(i, engine->getRenderer());
		}
	}

	//cache first page of images, select first game, display first GAMES_ON_SCREEN games
	firstDisplayedIndex = 0;
	selectedIndex = 0;
	for (int i = 0; i < GAMES_ON_SCREEN; i++) {
		games[i].cache();
		games[i].load();
	}
}

void cleanup() {
	//destroy games
	games.clear();

	free(engine);

	//delete background image(all other cached files deleted by games)
	remove(bgFile.c_str());

	//delete cache directory
	_rmdir(cacheDir.c_str());

	TTF_Quit();
	SDL_Quit();
}

void checkEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.scancode) {
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_KP_4:
				moveLeft();
				break;
			case SDL_SCANCODE_RIGHT:
			case SDL_SCANCODE_KP_6:
				moveRight();
				break;
			case SDL_SCANCODE_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
		case SDL_CONTROLLERAXISMOTION:
			//only care about left joystick X values
			if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
				if (e.caxis.value > 16384) {
					moveRight();
				}
				else if (e.caxis.value < -16384) {
					moveLeft();
				}
			}
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			//only care about gamepad buttons
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				moveLeft();
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				moveRight();
				break;
			case SDL_CONTROLLER_BUTTON_GUIDE:
				quit = true;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
};

void moveLeft() {
	//only one move allowed per cycle
	if (moveRequested) return;
	//can't move left past zero
	if (selectedIndex > 0) {
		selectedIndex--;
		moveRequested = true;
		//if we're off the left of the screen, move the screen
		if (selectedIndex < firstDisplayedIndex) {
			firstDisplayedIndex--;
			updateImgCache = true;
		}
	}
};

void moveRight() {
	//only one move allowed per cycle
	if (moveRequested) return;
	//can't move right past end of list
	if (selectedIndex < games.size() - 1) {
		selectedIndex++;
		moveRequested = true;
		//if we're off the right of the screen, move the screen
		if (selectedIndex >= firstDisplayedIndex + GAMES_ON_SCREEN) {
			firstDisplayedIndex++;
			updateImgCache = true;
		}
	}
};

void run() {
	//loop on inputs
	//render as requested
	//clear action queue after render
	quit = false;
	while (!quit) {
		moveRequested = false;
		checkEvents();
		//render screen again
		engine->renderScene(firstDisplayedIndex, selectedIndex, &games);
		//TODO: This should be a separate thread maybe
		if (updateImgCache) {
			checkCache();
		}
		updateImgCache = false;
		SDL_Delay(250);
	};
};

void checkCache() {
	for (int i = 0; i < games.size(); i++) {
		//make sure all games on screen and +/- 2 are cached, +/- 1 are loaded
		if (i < firstDisplayedIndex - 2 || i > firstDisplayedIndex + GAMES_ON_SCREEN + 1) {
			games[i].free();
			games[i].uncache();
		}
		else if (i < firstDisplayedIndex - 1 || i > firstDisplayedIndex + GAMES_ON_SCREEN) {
			games[i].free();
			games[i].cache();
		}
		else {
			games[i].cache();
			games[i].load();
		}
	}
}
