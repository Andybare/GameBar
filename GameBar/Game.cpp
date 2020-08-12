#include "Game.h"
#include "Constants.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <SDL2/SDL_image.h>

bool downloadFile(std::string filePath, std::string url) {
	if (FILE* file = fopen(filePath.c_str(), "r")) {
		fclose(file);
		return true;
	}

	//download file
	try {
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		FILE* file = fopen(filePath.c_str(), "wb");
		if (!file) {
			std::cout << "Error opening file " << filePath << "!" << std::endl;
			return false;
		}
		using namespace std::placeholders;
		curlpp::options::WriteFunction* writer = new curlpp::options::WriteFunction(std::bind(&FileCallback, file, _1, _2, _3));
		request.setOpt(writer);
		request.setOpt(new curlpp::options::Url(url));
		request.perform();
		fclose(file);
	}
	catch (curlpp::LogicError& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	catch (curlpp::RuntimeError& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}

Game::Game(Json::Value json, SDL_Renderer* renderer) {
	ren = renderer;
	
	imgTex = nullptr;

	cached = loaded = false;

	//grab gamePk as uuid
	uuid = json["gamePk"].asInt();

	//Parse titleText with json sanity checks
	if (!json["teams"]["home"]["team"]["name"].isString() ||
		!json["teams"]["away"]["team"]["name"].isString() ||
		!json["officialDate"].isString()) {
		titleText = "Unknown";
	}
	else {
		titleText = json["officialDate"].asString() + " " +
			json["teams"]["away"]["team"]["name"].asString() + " at " +
			json["teams"]["home"]["team"]["name"].asString();
		if (json["doubleHeader"].isString() &&
			json["doubleHeader"].asString() != "N" &&
			json["gameNumber"].isNumeric()) {
			titleText += " (Game " + json["gameNumber"].asString() + ")";
		}
	}
	//parse descriptionText with json sanity checks
	if (json["content"]["editorial"]["recap"]["mlb"]["headline"].isString()) {
		descriptionText = json["content"]["editorial"]["recap"]["mlb"]["headline"].asString();
	}
	else if (json["teams"]["away"]["score"].isNumeric() &&
		json["teams"]["home"]["score"].isNumeric() &&
		json["teams"]["home"]["team"]["name"].isString() &&
		json["teams"]["away"]["team"]["name"].isString()) {
		int homeScore = json["teams"]["home"]["score"].asInt();
		int awayScore = json["teams"]["away"]["score"].asInt();
		descriptionText = std::to_string(awayScore) + "-" + std::to_string(homeScore) + " " +
			(homeScore > awayScore ? json["teams"]["home"]["team"]["name"].asString() : json["teams"]["away"]["team"]["name"].asString());

	}
	
	//populate image url
	Json::Value cuts = json["content"]["editorial"]["recap"]["mlb"]["media"]["image"]["cuts"];
	//set img url to default unless we find better
	imgUrl = "";
	imgFilename = cacheDir + "\\default.jpg";
	if (cuts.isArray()) {
		for (int i = 0; i < cuts.size(); i++) {
			if (cuts[i]["width"].isNumeric() && cuts[i]["width"].asInt() == LARGE_IMAGE_WIDTH &&
				cuts[i]["height"].isNumeric() && cuts[i]["height"].asInt() == LARGE_IMAGE_HEIGHT) {
				imgUrl = cuts[i]["src"].asString();
				imgFilename = cacheDir + "\\" + std::to_string(uuid) + ".jpg";
				break;
			}
		}
	}
}

Game::~Game() {
	free();
	uncache();
}

bool Game::cache() {
	if (cached) return true;
	cached = downloadFile(imgFilename, imgUrl == "" ? defaultLogoUrl : imgUrl);
	return cached;

}

bool Game::uncache() {
	//kluge to test caching
	//return true;
	if (!cached) return true;
	if (imgUrl != "") {
		cached = !(remove(imgFilename.c_str()) == 0);
		return !cached;
	}
}

bool Game::load() {
	//Don't reload already loaded textures
	if (loaded) {
		return true;
	}

	//make sure file exists
	if (FILE* file = fopen(imgFilename.c_str(), "r")) {
		fclose(file);
	} else {
		if(!cache()) {
			return false;
		}
	}
	imgTex = IMG_LoadTexture(ren, imgFilename.c_str());
	if (imgTex == nullptr) {
		std::cout << "image " << imgFilename << " failed to load" << std::endl;
		return false;
	}
	std::cout << "loaded texture for game " << std::to_string(uuid) << std::endl;
	loaded = true;
	return true;
}

bool Game::free() {
	if (loaded) {
		SDL_DestroyTexture(imgTex);
		loaded = false;
	}
	return true;
}

bool Game::isCached() {
	return cached;
}

bool Game::isLoaded() {
	return loaded;
}

SDL_Texture* Game::getImage() {
	if (!imgTex) {
		if (!load()) {
			return nullptr;
		}
	}
	return imgTex;
}

std::string Game::getTopText() {
	return titleText;
}

std::string Game::getBottomText() {
	return descriptionText;
}