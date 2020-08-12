#pragma once
#include <SDL2/SDL.h>
#include <string>

//Graphical + file elements
const std::string cacheDir(".\\cache");
const std::string bgFile(".\\cache\\background.jpg");
const std::string leftFile("left.png");
const std::string rightFile("right.png");
const std::string fontFile("OpenSans-Regular.ttf");
const SDL_Color uiColor = { 255, 255, 255, 255 };
const int gameFontSmallSize = 12;
const int gameFontLargeSize = 14;

const std::string jsonUrl("http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1");
const std::string backgroundUrl("http://mlb.mlb.com/mlb/images/devices/ballpark/1920x1080/1.jpg");
const std::string defaultLogoUrl("http://mlb.mlb.com/mlb/images/devices/ballpark/1920x1080/3.jpg");


//UI Sizing constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int ARROW_TEXTURE_WIDTH = 50;
const int LARGE_IMAGE_WIDTH = 320;  //used to find JSON entry for game image
const int LARGE_IMAGE_HEIGHT = 180; //used to find JSON entry for game image
const int SMALL_IMAGE_WIDTH = 215;
const int SMALL_IMAGE_HEIGHT = 121;
const int CENTERLINE = 540; //y-value all elements align to
const int ARROW_WIDTH = 60;
const int ITEM_DISTANCE_WIDTH = 300;
const int GAMES_ON_SCREEN = 6;

//Curl FileCallback function
size_t FileCallback(FILE* f, char* ptr, size_t size, size_t nmemb);

