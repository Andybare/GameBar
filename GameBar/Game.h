#pragma once
#include <json/json.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Game
{
public:
	// Parses a "Game" JSON object into the various components. Stores the URL for the game image(doesn't download it), constructs display texts.
	Game(Json::Value json, SDL_Renderer* renderer);

	// Clears all textures and files downloaded for this Game on destruction
	~Game();

	//Downloads image to the /cache directory.
	bool cache();

	//Deletes image from the /cache directory.
	bool uncache();
	
	//Pushes image to a texture in memory.
	bool load();
	
	//Frees texture from memory.
	bool free();
	
	bool isCached();
	bool isLoaded();

	SDL_Texture* getImage();
	std::string getTopText();
	std::string getBottomText();

private:
	SDL_Texture *imgTex;
	TTF_Font *smFont, *lgFont;
	SDL_Surface *botText, *topText;
	SDL_Texture *botTextTex, *topTextTex;
	std::string imgUrl, imgFilename;
	std::string titleText, descriptionText;
	SDL_Renderer* ren;
	int uuid;
	bool cached;
	bool loaded;
};

