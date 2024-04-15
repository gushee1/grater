#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H

//STL
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <algorithm>

//Third Party
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "box2d/box2d.h"

class EngineUtils {
public:
	static inline lua_State* g_lua_state = nullptr;

	//Lua
	static inline void LuaInit() {
		g_lua_state = luaL_newstate();
		luaL_openlibs(g_lua_state);
	}

	static inline void ReportLuaError(const std::string& actor_name, const luabridge::LuaException& e) {
		std::string error_message = e.what();

		std::replace(error_message.begin(), error_message.end(), '\\', '/');

		std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
	}

	//Math
	static inline float ConvertDegreesToRads(float degrees) {
		return degrees * (b2_pi / 180.0f);
	}

	static inline float ConvertRadsToDegrees(float rads) {
		return rads * (180.0f / b2_pi);
	}

	//Input Parsing
	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
	{
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
		out_document.ParseStream(stream);
		std::fclose(file_pointer);

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);
		}
	}

	static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
		size_t pos = input.find(phrase);

		if (pos == std::string::npos) return "";

		pos += phrase.length();
		while (pos < input.size() && std::isspace(input[pos])) {
			++pos;
		}

		if (pos == input.size()) return "";

		size_t endpos = pos;
		while (endpos < input.size() && !std::isspace(input[endpos])) {
			++endpos;
		}

		return input.substr(pos, endpos - pos);
	}

	//Debug API
	static inline void CppLog(std::string message) {
		std::cout << message << std::endl;
	}

	static inline void CppLogError(std::string message) {
		std::cerr << message << std::endl;
	}

	//Application API
	static inline void QuitApplication() {
		exit(0);
	}

	static inline void Sleep(int milliseconds) {
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}

	static inline void OpenURL(std::string url) {

#if defined(_WIN32)
		std::string command = "start " + url;
#elif defined(__APPLE__)
		std::string command = "open " + url;
#else //Linux
		std::string command = "xdg-open " + url;
#endif

		std::system(command.c_str());
	}
};

#endif
