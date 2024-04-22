#include "Renderer.h"

void Renderer::Start(const char* game_title)
{
	//load in config values//
	if (std::filesystem::exists("resources/rendering.config")) {
		rapidjson::Document rendering_config;
		EngineUtils::ReadJsonFile("resources/rendering.config", rendering_config);
		if (rendering_config.HasMember("x_resolution")) {
			camera.dimensions.x = rendering_config["x_resolution"].GetInt();
		}
		if (rendering_config.HasMember("y_resolution")) {
			camera.dimensions.y = rendering_config["y_resolution"].GetInt();
		}
		if (rendering_config.HasMember("clear_color_r")) {
			clear_color.r = rendering_config["clear_color_r"].GetInt();
		}
		if (rendering_config.HasMember("clear_color_g")) {
			clear_color.g = rendering_config["clear_color_g"].GetInt();
		}
		if (rendering_config.HasMember("clear_color_b")) {
			clear_color.b = rendering_config["clear_color_b"].GetInt();
		}
		if (rendering_config.HasMember("zoom_factor")) {
			camera.zoom_factor = rendering_config["zoom_factor"].GetFloat();
		}
		if (rendering_config.HasMember("cam_offset_x")) {
			camera.offset.x = rendering_config["cam_offset_x"].GetFloat() * ppu;
		}
		if (rendering_config.HasMember("cam_offset_y")) {
			camera.offset.y = rendering_config["cam_offset_y"].GetFloat() * ppu;
		}
		if (rendering_config.HasMember("cam_ease_factor")) {
			cam_ease_factor = rendering_config["cam_ease_factor"].GetFloat();
		}
	}

	camera.position.x = 0;
	camera.position.y = 0;

	//create sdl window and renderer
	game_window = Helper::SDL_CreateWindow498(
		game_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		camera.dimensions.x,
		camera.dimensions.y,
		SDL_WINDOW_SHOWN
	);
	renderer = Helper::SDL_CreateRenderer498(game_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
}

void Renderer::DoesImageExist(const std::string& img_name)
{
	std::string img_path = "resources/images/" + img_name + ".png";
	if (!std::filesystem::exists(img_path)) {
		std::cout << "error: missing image " << img_name;
		exit(0);
	}
}

SDL_Texture* Renderer::LoadImage(std::string image_name)
{
	//if the image has not yet been loaded
	if (ImageDB.find(image_name) == ImageDB.end()) {
		DoesImageExist(image_name);
		std::string img_path = "resources/images/" + image_name + ".png";
		ImageDB[image_name] = IMG_LoadTexture(renderer, img_path.c_str());
	}

	return ImageDB[image_name];
}

void Renderer::SetCameraPosition(float x, float y)
{
	camera.position = { x, y };
}

float Renderer::GetCameraXPos()
{
	return camera.position.x;
}

float Renderer::GetCameraYPos()
{
	return camera.position.y;
}

glm::vec2 Renderer::GetCameraPos()
{
	return { GetCameraXPos(), GetCameraYPos() };
}

glm::ivec2 Renderer::GetCameraDimensions()
{
	return camera.dimensions;
}

void Renderer::SetCameraZoom(float _zoom_factor)
{
	camera.zoom_factor = _zoom_factor;
}

float Renderer::GetCameraZoom()
{
	return camera.zoom_factor;
}

void Renderer::ClearFrame()
{
	//clear the frame
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_RenderClear(renderer);
}

void Renderer::DrawToFrame(SDL_Texture* texture_to_draw, int x, int y)
{
	SDL_Rect dstRect;
	dstRect.x = x;
	dstRect.y = y;
	SDL_QueryTexture(texture_to_draw, NULL, NULL, &dstRect.w, &dstRect.h);
	SDL_RenderCopy(renderer, texture_to_draw, nullptr, &dstRect);
}

void Renderer::QueueImageToDraw(std::string image_name, float x, float y)
{
	switch (grater::network::GetMultiplayerState()) {
	case grater::network::MultiplayerState::SINGLEPLAYER:
		image_queue.push_back(ImageParams(image_name, x, y));
		break;
	case grater::network::MultiplayerState::SERVER:
		grater::network::server->SendDrawRequest();
		break;
	default:
		break;
	}
}

void Renderer::QueueImageToDrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order)
{
	image_queue.push_back(ImageParams(image_name, x, y, rotation_degrees, scale_x, scale_y, pivot_x, pivot_y, r, g, b, a, sorting_order));
}

void Renderer::DrawImages()
{
	std::stable_sort(image_queue.begin(), image_queue.end(), [](const ImageParams& a, const ImageParams& b) {
		return a.sorting_order < b.sorting_order;
	});

	for (const ImageParams& image : image_queue) {
		SDL_Texture* texture = LoadImage(image.image_name);

		glm::vec2 final_rendering_position = glm::vec2(image.x - GetCameraXPos(), image.y - GetCameraYPos());

		SDL_Rect dest_rect;
		SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h);

		int flip_mode = SDL_FLIP_NONE;
		if (image.scale_x < 0) flip_mode |= SDL_FLIP_HORIZONTAL;
		if (image.scale_y < 0) flip_mode |= SDL_FLIP_VERTICAL;

		float x_scale = std::abs(image.scale_x);
		float y_scale = std::abs(image.scale_y);

		SDL_Point pivot_point = { static_cast<int>(image.pivot_x * dest_rect.w * x_scale), static_cast<int>(image.pivot_y * dest_rect.h * y_scale)};

		dest_rect.w *= x_scale;
		dest_rect.h *= y_scale;

		dest_rect.x = static_cast<int>(final_rendering_position.x * ppu + camera.dimensions.x * 0.5f * (1.0f / camera.zoom_factor) - pivot_point.x);
		dest_rect.y = static_cast<int>(final_rendering_position.y * ppu + camera.dimensions.y * 0.5f * (1.0f / camera.zoom_factor) - pivot_point.y);

		SDL_SetTextureColorMod(texture, image.r, image.g, image.b);
		SDL_SetTextureAlphaMod(texture, image.a);
		Helper::SDL_RenderCopyEx498(
			0, //dummy actor id
			"", //dummy actor name
			renderer,
			texture,
			NULL,
			&dest_rect,
			image.rotation_degrees,
			&pivot_point,
			static_cast<SDL_RendererFlip>(flip_mode)
		);
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}

	image_queue.clear();
}

void Renderer::QueueUIToDraw(std::string image_name, float x, float y)
{
	UI_queue.push_back(UIParams(image_name, x, y));//
}

void Renderer::QueueUIToDrawEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order)
{
	UI_queue.push_back(UIParams(image_name, x, y, r, g, b, a, sorting_order));
}

void Renderer::DrawUI()
{
	std::stable_sort(UI_queue.begin(), UI_queue.end(), [](const UIParams& a, const UIParams& b) {
		return a.sorting_order < b.sorting_order;
	});

	for (const auto& image : UI_queue) {
		SDL_Texture* texture = LoadImage(image.image_name);
		SDL_SetTextureColorMod(texture, image.r, image.g, image.b);
		SDL_SetTextureAlphaMod(texture, image.a);
		DrawToFrame(texture, image.x, image.y);
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
	
	UI_queue.clear();
}

void Renderer::QueueTextToDraw(const std::string& text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a)
{
	text_queue.push(TextParams(text_content, x, y, font_name, font_size, r, g, b, a));

}

void Renderer::GraterDrawText() {
	while (!text_queue.empty()) {
		TextParams text_event = text_queue.front();
		text_queue.pop();
		TTF_Font* font = TextDB::LoadFont(text_event.font_name, text_event.font_size);
		SDL_Color font_color = { static_cast<Uint8>(text_event.r), static_cast<Uint8>(text_event.g), static_cast<Uint8>(text_event.b), static_cast<Uint8>(text_event.a) };
		SDL_Surface* surface = TTF_RenderText_Solid(font, text_event.str_content.c_str(), font_color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		DrawToFrame(texture, text_event.x, text_event.y);
	}
}

void Renderer::QueuePixelToDraw(float x, float y, float r, float g, float b, float a)
{
	pixel_queue.push_back(PixelParams(x, y, r, g, b, a));
}

void Renderer::DrawPixels()
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for (const PixelParams& pixel : pixel_queue) {
		SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, pixel.a);
		SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
	}
	SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	pixel_queue.clear();
}

void Renderer::ShowFrame()
{
	Helper::SDL_RenderPresent498(renderer);
}
