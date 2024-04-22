#ifndef RENDERER_H
#define RENDERER_H

//STL
#include <filesystem>
#include <cmath>
#include <queue>

//Third Party
#include "SDL_ttf.h"

//Internal
#include "Helper.h"
#include "EngineUtils.h"
#include "TextDB.h"
#include "ActorDB.h"
#include "networking/network.h"

class Renderer
{
public:
	struct ImageParams {
		std::string image_name;
		float x;
		float y;
		int rotation_degrees;
		float scale_x;
		float scale_y;
		float pivot_x;
		float pivot_y;
		int r;
		int g;
		int b;
		int a;
		int sorting_order;

		ImageParams(std::string image_name, int x, int y) :
			image_name(image_name), x(x), y(y), rotation_degrees(0), scale_x(1.0f), scale_y(1.0f), pivot_x(0.5f), pivot_y(0.5f), r(255), g(255), b(255), a(255), sorting_order(0) { };

		ImageParams(std::string image_name, float x, float y, int rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order) :
			image_name(image_name), x(x), y(y), rotation_degrees(rotation_degrees), scale_x(scale_x), scale_y(scale_y), pivot_x(pivot_x), pivot_y(pivot_y), r(r), g(g), b(b), a(a), sorting_order(sorting_order) { };
	};

	struct UIParams {
		std::string image_name;
		int x;
		int y;
		int r;
		int g;
		int b;
		int a;
		int sorting_order;

		UIParams(std::string image_name, int x, int y) : image_name(image_name), x(x), y(y), r(255), g(255), b(255), a(255), sorting_order(0) { };

		UIParams(std::string image_name, int x, int y, int r, int g, int b, int a, int sorting_order) :
			image_name(image_name), x(x), y(y), r(r), g(g), b(b), a(a), sorting_order(sorting_order) { };
	};

	struct TextParams {
		std::string str_content;
		int x;
		int y;
		std::string font_name;
		int font_size;
		int r;
		int g;
		int b;
		int a;

		TextParams(std::string content, int x, int y, std::string font_name, int font_size, int r, int g, int b, int a) :
			str_content(content), x(x), y(y), font_name(font_name), font_size(font_size), r(r), g(g), b(b), a(a) { };
	};

	struct PixelParams {
		int x;
		int y;
		int r;
		int g;
		int b;
		int a;
		
		PixelParams(int x, int y, int r, int g, int b, int a) : x(x), y(y), r(r), g(g), b(b), a(a) { };
	};

	static void Start(const char* game_title);

	static void DoesImageExist(const std::string& img_name);

	static SDL_Texture* LoadImage(std::string image_name);

	static void SetCameraPosition(float x, float y);

	static float GetCameraXPos();

	static float GetCameraYPos();

	static glm::vec2 GetCameraPos();

	static glm::ivec2 GetCameraDimensions();

	static void SetCameraZoom(float zoom_factor);

	static float GetCameraZoom();

	static void ClearFrame();

	static void DrawToFrame(SDL_Texture* texture_to_draw, int x, int y);

	static void QueueImageToDraw(std::string image_name, float x, float y);

	static void QueueImageToDrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);

	static void DrawImages();

	static void QueueUIToDraw(std::string image_name, float x, float y);

	static void QueueUIToDrawEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order);

	static void DrawUI();

	static void QueueTextToDraw(const std::string& text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a);

	static void GraterDrawText();

	static void QueuePixelToDraw(float x, float y, float r, float g, float b, float a);

	static void DrawPixels();

	static void ShowFrame();

	static void SendFrameToClient();

	static inline SDL_Window* game_window = nullptr;

	static inline SDL_Renderer* renderer = nullptr;

	struct Camera {
		glm::vec2 position;
		glm::ivec2 dimensions;
		glm::vec2 offset;
		float zoom_factor;

		Camera(float x, float y, int w, int h, float offset_x, float offset_y, float zoom_factor) : 
			position(x, y), dimensions(w, h), offset(offset_x, offset_y), zoom_factor(zoom_factor) {};
	};

	static inline Camera camera = { 0, 0, 640, 360, 0, 0, 1 };

	struct Color {
		int r;
		int g;
		int b;
		int a;

		Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
	};
	static inline Color clear_color = Color(255, 255, 255, 255);

	static inline float cam_ease_factor = 1.0f;

	static inline std::unordered_map<std::string, SDL_Texture*> ImageDB;

	static inline std::vector<ImageParams> image_queue;
	static inline std::vector<UIParams> UI_queue;
	static inline std::queue<TextParams> text_queue;
	static inline std::vector<PixelParams> pixel_queue;

	//pixels_per_unit
	static inline int ppu = 100;
};

#endif
