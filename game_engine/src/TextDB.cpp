#include "TextDB.h"

void TextDB::Initialize()
{
	TTF_Init();
}


TTF_Font* TextDB::LoadFont(std::string font_name, int font_size)
{
	std::string file_path = "resources/fonts/" + font_name + ".ttf";
	if (!std::filesystem::exists(file_path)) {
		std::cout << "error: font " << font_name << " missing";
		exit(0);
	}

	if (FontDB.find(font_name) == FontDB.end() || FontDB[font_name].find(font_size) == FontDB[font_name].end()) {
		FontDB[font_name][font_size] = TTF_OpenFont(file_path.c_str(), font_size);
	}

	return FontDB[font_name][font_size];
}

void TextDB::Destroy()
{
	TTF_Quit();
}
