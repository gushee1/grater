#ifndef TEXTDB_H
#define TEXTDB_H

//STL
#include <string>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <queue>

//Third Party
#include "SDL_ttf.h"

//TODO: may rename to FontDB
class TextDB
{
public:
	static void Initialize();

	static TTF_Font* LoadFont(std::string font_name, int font_size);

	static void Destroy();

	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> FontDB;
};

#endif

