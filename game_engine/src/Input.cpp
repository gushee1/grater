#include "Input.h"

void Input::Init()
{
	
}

void Input::ProcessEvent(const SDL_Event& e)
{
	switch (e.type)
	{
	case SDL_MOUSEMOTION:
		mouse_position = glm::vec2(e.motion.x, e.motion.y);
		break;
	case SDL_MOUSEWHEEL:
		mouse_wheel_motion = e.wheel.preciseY;
		break;
	case SDL_MOUSEBUTTONDOWN:
		mouse_states[e.button.button] = INPUT_STATE_DOWN_THIS_FRAME;
		down_this_frame_mouse_buttons.push_back(e.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		mouse_states[e.button.button] = INPUT_STATE_UP_THIS_FRAME;
		up_this_frame_mouse_buttons.push_back(e.button.button);
		break;
	case SDL_KEYDOWN:
		if (grater::network::GetMultiplayerState() == grater::network::MultiplayerState::CLIENT) {
			grater::network::client->SendKeyboardInput(static_cast<int>(e.key.keysym.scancode));
		}
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_DOWN_THIS_FRAME;
		down_this_frame_scancodes.push_back(e.key.keysym.scancode);
		break;
	case SDL_KEYUP:
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_UP_THIS_FRAME;
		up_this_frame_scancodes.push_back(e.key.keysym.scancode);
		break;
	default:
		break;
	}
}

void Input::LateUpdate()
{
	mouse_wheel_motion = 0;
	for (int mouse_button : up_this_frame_mouse_buttons) {
		mouse_states[mouse_button] = INPUT_STATE_UP;
	}
	for (int mouse_button : down_this_frame_mouse_buttons) {
		mouse_states[mouse_button] = INPUT_STATE_DOWN;
	}
	for (SDL_Scancode scancode : up_this_frame_scancodes) {
		keyboard_states[scancode] = INPUT_STATE_UP;
	}
	for (SDL_Scancode scancode : down_this_frame_scancodes) {
		keyboard_states[scancode] = INPUT_STATE_DOWN;
	}
	down_this_frame_mouse_buttons.clear();
	up_this_frame_mouse_buttons.clear();
	down_this_frame_scancodes.clear();
	up_this_frame_scancodes.clear();
}

bool Input::GetKey(std::string key)
{
	if (__keycode_to_scancode.find(key) != __keycode_to_scancode.end()) {
		SDL_Scancode keycode = __keycode_to_scancode.at(key);
		return keyboard_states[keycode] == INPUT_STATE_DOWN_THIS_FRAME || keyboard_states[keycode] == INPUT_STATE_DOWN;
	}
	else {
		return false;
	}
}

bool Input::GetKeyDown(std::string key)
{
	if (__keycode_to_scancode.find(key) != __keycode_to_scancode.end()) {
		SDL_Scancode keycode = __keycode_to_scancode.at(key);
		return keyboard_states[keycode] == INPUT_STATE_DOWN_THIS_FRAME;
	}
	else {
		return false;
	}
}

bool Input::GetKeyUp(std::string key)
{
	if (__keycode_to_scancode.find(key) != __keycode_to_scancode.end()) {
		SDL_Scancode keycode = __keycode_to_scancode.at(key);
		return keyboard_states[keycode] == INPUT_STATE_UP_THIS_FRAME;
	}
	else {
		return false;
	}
}

bool Input::GetMouseButton(int mouse_button_code)
{
	return mouse_states[mouse_button_code] == INPUT_STATE_DOWN_THIS_FRAME || mouse_states[mouse_button_code] == INPUT_STATE_DOWN;
}

bool Input::GetMouseButtonDown(int mouse_button_code)
{
	return mouse_states[mouse_button_code] == INPUT_STATE_DOWN_THIS_FRAME;
}

bool Input::GetMouseButtonUp(int mouse_button_code)
{
	return mouse_states[mouse_button_code] == INPUT_STATE_UP_THIS_FRAME;
}

glm::vec2 Input::GetMousePosition()
{
	return mouse_position;
}

float Input::GetMouseScrollDelta()
{
	return mouse_wheel_motion;
}
