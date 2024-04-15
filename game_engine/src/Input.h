#ifndef INPUT_H
#define INPUT_H

//STL
#include <vector>
#include <unordered_map>
#include <string>

//Third Party
#include "SDL.h"
#include "glm.hpp"

//Internal
#include "networking/network.h"

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_DOWN_THIS_FRAME, INPUT_STATE_DOWN, INPUT_STATE_UP_THIS_FRAME };

class Input {
	const static inline std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
		// Directional (arrow) Keys
		{"up", SDL_SCANCODE_UP},
		{"down", SDL_SCANCODE_DOWN},
		{"right", SDL_SCANCODE_RIGHT},
		{"left", SDL_SCANCODE_LEFT},

		// Misc Keys
		{"escape", SDL_SCANCODE_ESCAPE},

		// Modifier Keys
		{"lshift", SDL_SCANCODE_LSHIFT},
		{"rshift", SDL_SCANCODE_RSHIFT},
		{"lctrl", SDL_SCANCODE_LCTRL},
		{"rctrl", SDL_SCANCODE_RCTRL},
		{"lalt", SDL_SCANCODE_LALT},
		{"ralt", SDL_SCANCODE_RALT},

		// Editing Keys
		{"tab", SDL_SCANCODE_TAB},
		{"return", SDL_SCANCODE_RETURN},
		{"enter", SDL_SCANCODE_RETURN},
		{"backspace", SDL_SCANCODE_BACKSPACE},
		{"delete", SDL_SCANCODE_DELETE},
		{"insert", SDL_SCANCODE_INSERT},

		// Character Keys
		{"space", SDL_SCANCODE_SPACE},
		{"a", SDL_SCANCODE_A},
		{"b", SDL_SCANCODE_B},
		{"c", SDL_SCANCODE_C},
		{"d", SDL_SCANCODE_D},
		{"e", SDL_SCANCODE_E},
		{"f", SDL_SCANCODE_F},
		{"g", SDL_SCANCODE_G},
		{"h", SDL_SCANCODE_H},
		{"i", SDL_SCANCODE_I},
		{"j", SDL_SCANCODE_J},
		{"k", SDL_SCANCODE_K},
		{"l", SDL_SCANCODE_L},
		{"m", SDL_SCANCODE_M},
		{"n", SDL_SCANCODE_N},
		{"o", SDL_SCANCODE_O},
		{"p", SDL_SCANCODE_P},
		{"q", SDL_SCANCODE_Q},
		{"r", SDL_SCANCODE_R},
		{"s", SDL_SCANCODE_S},
		{"t", SDL_SCANCODE_T},
		{"u", SDL_SCANCODE_U},
		{"v", SDL_SCANCODE_V},
		{"w", SDL_SCANCODE_W},
		{"x", SDL_SCANCODE_X},
		{"y", SDL_SCANCODE_Y},
		{"z", SDL_SCANCODE_Z},
		{"0", SDL_SCANCODE_0},
		{"1", SDL_SCANCODE_1},
		{"2", SDL_SCANCODE_2},
		{"3", SDL_SCANCODE_3},
		{"4", SDL_SCANCODE_4},
		{"5", SDL_SCANCODE_5},
		{"6", SDL_SCANCODE_6},
		{"7", SDL_SCANCODE_7},
		{"8", SDL_SCANCODE_8},
		{"9", SDL_SCANCODE_9},
		{"/", SDL_SCANCODE_SLASH},
		{";", SDL_SCANCODE_SEMICOLON},
		{"=", SDL_SCANCODE_EQUALS},
		{"-", SDL_SCANCODE_MINUS},
		{".", SDL_SCANCODE_PERIOD},
		{",", SDL_SCANCODE_COMMA},
		{"[", SDL_SCANCODE_LEFTBRACKET},
		{"]", SDL_SCANCODE_RIGHTBRACKET},
		{"\\", SDL_SCANCODE_BACKSLASH},
		{"'", SDL_SCANCODE_APOSTROPHE}
	};

	const static inline std::unordered_map<SDL_Scancode, std::string> __scancode_to_keycode = {
	{SDL_SCANCODE_UP, "up"},
	{SDL_SCANCODE_DOWN, "down"},
	{SDL_SCANCODE_LEFT, "left"},
	{SDL_SCANCODE_RIGHT, "right"},
	{SDL_SCANCODE_ESCAPE, "escape"},
	{SDL_SCANCODE_LSHIFT, "lshift"},
	{SDL_SCANCODE_RSHIFT, "rshift"},
	{SDL_SCANCODE_LCTRL, "lctrl"},
	{SDL_SCANCODE_RCTRL, "rctrl"},
	{SDL_SCANCODE_LALT, "lalt"},
	{SDL_SCANCODE_RALT, "ralt"},
	{SDL_SCANCODE_TAB, "tab"},
	{SDL_SCANCODE_RETURN, "return"},
	{SDL_SCANCODE_BACKSPACE, "backspace"},
	{SDL_SCANCODE_DELETE, "delete"},
	{SDL_SCANCODE_INSERT, "insert"},
	{SDL_SCANCODE_SPACE, "space"},
	{SDL_SCANCODE_A, "a"},
	{SDL_SCANCODE_B, "b"},
	{SDL_SCANCODE_C, "c"},
	{SDL_SCANCODE_D, "d"},
	{SDL_SCANCODE_E, "e"},
	{SDL_SCANCODE_F, "f"},
	{SDL_SCANCODE_G, "g"},
	{SDL_SCANCODE_H, "h"},
	{SDL_SCANCODE_I, "i"},
	{SDL_SCANCODE_J, "j"},
	{SDL_SCANCODE_K, "k"},
	{SDL_SCANCODE_L, "l"},
	{SDL_SCANCODE_M, "m"},
	{SDL_SCANCODE_N, "n"},
	{SDL_SCANCODE_O, "o"},
	{SDL_SCANCODE_P, "p"},
	{SDL_SCANCODE_Q, "q"},
	{SDL_SCANCODE_R, "r"},
	{SDL_SCANCODE_S, "s"},
	{SDL_SCANCODE_T, "t"},
	{SDL_SCANCODE_U, "u"},
	{SDL_SCANCODE_V, "v"},
	{SDL_SCANCODE_W, "w"},
	{SDL_SCANCODE_X, "x"},
	{SDL_SCANCODE_Y, "y"},
	{SDL_SCANCODE_Z, "z"},
	{SDL_SCANCODE_0, "0"},
	{SDL_SCANCODE_1, "1"},
	{SDL_SCANCODE_2, "2"},
	{SDL_SCANCODE_3, "3"},
	{SDL_SCANCODE_4, "4"},
	{SDL_SCANCODE_5, "5"},
	{SDL_SCANCODE_6, "6"},
	{SDL_SCANCODE_7, "7"},
	{SDL_SCANCODE_8, "8"},
	{SDL_SCANCODE_9, "9"},
	{SDL_SCANCODE_SLASH, "/"},
	{SDL_SCANCODE_SEMICOLON, ";"},
	{SDL_SCANCODE_EQUALS, "="},
	{SDL_SCANCODE_MINUS, "-"},
	{SDL_SCANCODE_PERIOD, "."},
	{SDL_SCANCODE_COMMA, ","},
	{SDL_SCANCODE_LEFTBRACKET, "["},
	{SDL_SCANCODE_RIGHTBRACKET, "]"},
	{SDL_SCANCODE_BACKSLASH, "\\"},
	{SDL_SCANCODE_APOSTROPHE, "'"}
	};

public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate();

	static bool GetKey(std::string key);
	static bool GetKeyDown(std::string key);
	static bool GetKeyUp(std::string key);

	static bool GetMouseButton(int mouse_button_code);
	static bool GetMouseButtonDown(int mouse_button_code);
	static bool GetMouseButtonUp(int mouse_button_code);

	static glm::vec2 GetMousePosition();
	static float GetMouseScrollDelta();

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> down_this_frame_scancodes;
	static inline std::vector<SDL_Scancode> up_this_frame_scancodes;

	static inline std::unordered_map<int, INPUT_STATE> mouse_states;
	static inline std::vector<int> down_this_frame_mouse_buttons;
	static inline std::vector<int> up_this_frame_mouse_buttons;

	static inline glm::vec2 mouse_position;
	static inline float mouse_wheel_motion = 0;
};

#endif
