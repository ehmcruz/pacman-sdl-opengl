#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <cmath>

#include "../debug.h"
#include "../graphics.h"
#include "../game-object.h"
#include "../lib.h"
#include "sdl.h"

// ---------------------------------------------------

using Game::dprint;
using Game::dprintln;

//#define DEBUG_SHOW_CENTER_LINE

// ---------------------------------------------------

static void my_SDL_DrawCircle (SDL_Renderer *renderer, const int32_t centreX, const int32_t centreY, const int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

static void my_SDL_DrawFilledCircle (SDL_Renderer *renderer, const int32_t centreX, const int32_t centreY, const int32_t radius)
{
	for (int32_t w = 0; w < radius * 2; w++) {
		for (int32_t h = 0; h < radius * 2; h++) {
			int32_t dx = radius - w; // horizontal offset
			int32_t dy = radius - h; // vertical offset
			if ((dx*dx + dy*dy) <= (radius * radius))
				SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
		}
	}
}

// ---------------------------------------------------

Graphics::SDL::Renderer::Renderer (const uint32_t window_width_px_, const uint32_t window_height_px_)
	: Graphics::Renderer (window_width_px_, window_height_px_)
{
	this->sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->window_width_px, this->window_height_px, SDL_WINDOW_SHOWN);
	this->renderer = SDL_CreateRenderer(this->sdl_window, -1, 0);

	this->background_color = Graphics::config_background_color;
	this->wait_next_frame();
}

Graphics::SDL::Renderer::~Renderer ()
{
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->sdl_window);
}

void Graphics::SDL::Renderer::wait_next_frame ()
{
	SDL_SetRenderDrawColor(this->renderer, this->background_color.r, this->background_color.g, this->background_color.b, this->background_color.a);
	SDL_RenderClear(this->renderer);
}

void Graphics::SDL::Renderer::draw_circle (const ShapeCircle& circle, const Vector& offset, const Graphics::Color& color)
{
	const SDL_Color sdl_color = color;
	const Vector world_pos = offset + circle.get_value_delta();
	const Vector4 clip_pos = this->projection_matrix * Vector4(world_pos.x, world_pos.y, 0.0f, 1.0f);

/*	Game::ShapeRect rect(circle.get_radius()*2.0f, circle.get_radius()*2.0f);
	rect.set_delta(circle.get_delta());
	this->draw_rect(rect, offset, color);*/

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);

	my_SDL_DrawFilledCircle(
		renderer,
		static_cast<int32_t>( clip_pos.x ),
		static_cast<int32_t>( clip_pos.y ),
		static_cast<int32_t>( circle.get_radius() * this->scale_factor )
		);
}

void Graphics::SDL::Renderer::draw_rect (const ShapeRect& rect, const Vector& offset, const Graphics::Color& color)
{
	SDL_Rect sdl_rect;
	const SDL_Color sdl_color = color;
	const Vector world_pos = offset + rect.get_value_delta();
	//const Vector world_pos = Vector(4.0f, 4.0f);
	const Vector4 clip_pos = this->projection_matrix * Vector4(world_pos.x, world_pos.y, 0.0f, 1.0f);
	//const Vector4d clip_pos = translate_to_clip_init * clip_pos_;

#if 0
	dprint( "world_pos:" )
	world_pos.println();

	dprint( "clip_pos:" )
	clip_pos.println();
//exit(1);
#endif

	sdl_rect.x = Game::round_to_nearest(clip_pos.x - (rect.get_w() * 0.5f * this->scale_factor));
	sdl_rect.y = Game::round_to_nearest(clip_pos.y - (rect.get_h() * 0.5f * this->scale_factor));
	sdl_rect.w = Game::round_to_nearest(rect.get_w() * this->scale_factor);
	sdl_rect.h = Game::round_to_nearest(rect.get_h() * this->scale_factor);

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
	SDL_RenderFillRect(this->renderer, &sdl_rect);
}

void Graphics::SDL::Renderer::setup_projection_matrix (const ProjectionMatrixArgs& args)
{
	const float max_value = static_cast<float>( std::max(this->window_width_px, this->window_height_px) );
	const Vector clip_init = args.clip_init_norm * max_value;
	const Vector clip_end = args.clip_end_norm * max_value;
	const Vector clip_size = clip_end - clip_init;
	const float clip_aspect_ratio = clip_size.x / clip_size.y;

	const Vector world_size = args.world_end - args.world_init;
	
	const float world_screen_width = std::min(args.world_screen_width, world_size.x);
	const float world_screen_height = std::min(world_screen_width / clip_aspect_ratio, world_size.y);

	const Vector world_screen_size = Vector(world_screen_width, world_screen_height);

	this->scale_factor = clip_size.x / world_screen_size.x;

	Vector world_camera = args.world_camera_focus - Vector(world_screen_size.x*0.5f, world_screen_size.y*0.5f);

	//dprint( "world_camera PRE: " ) world_camera.println();

	if (args.force_camera_inside_world) {
		if (world_camera.x < args.world_init.x)
			world_camera.x = args.world_init.x;
		else if ((world_camera.x + world_screen_size.x) > args.world_end.x)
			world_camera.x = args.world_end.x - world_screen_size.x;

		//dprint( "world_camera POS: " ) Mylib::Math::println(world_camera);

		if (world_camera.y < args.world_init.y)
			world_camera.y = args.world_init.y;
		else if ((world_camera.y + world_screen_size.y) > args.world_end.y)
			world_camera.y = args.world_end.y - world_screen_size.y;
	}

#if 0
	dprintln( "clip_init: ", clip_init );
	dprintln( "clip_end: ", clip_end );
	dprintln( "clip_size: ", clip_size );
	dprintln( "clip_aspect_ratio: ", clip_aspect_ratio );
	dprintln( "scale_factor: ", this->scale_factor );
	dprintln( "world_size: ", world_size );
	dprintln( "world_screen_size: ", world_screen_size );
	dprintln( "args.world_camera_focus: ", args.world_camera_focus );
	dprintln( "world_camera: ", world_camera );
//exit(1);
#endif

	Matrix4 translate_to_clip_init;
	translate_to_clip_init.set_translate(clip_init);
//	dprintln( "translation to clip init:" ); dprintln( translate_to_clip_init );

	Matrix4 scale;
	scale.set_scale(Vector(this->scale_factor, this->scale_factor));
//	dprintln( "scale matrix:" ); dprintln( scale );
//exit(1);

	Matrix4 translate_camera;
	translate_camera.set_translate(-world_camera);
//	dprintln( "translation matrix:" ); dprintln( translate_camera );

	this->projection_matrix = (translate_to_clip_init * scale) * translate_camera;
	//this->projection_matrix = scale * translate_camera;
//	dprintln( "final matrix:" ); dprintln( this->projection_matrix );
}

void Graphics::SDL::Renderer::render ()
{
#ifdef DEBUG_SHOW_CENTER_LINE
{
	const SDL_Color sdl_color = Color{ .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
	SDL_RenderDrawLine(this->renderer, this->window_width_px / 2, 0, this->window_width_px / 2, this->window_height_px);
	SDL_RenderDrawLine(this->renderer, 0, this->window_height_px / 2, this->window_width_px, this->window_height_px / 2);
}
#endif
	SDL_RenderPresent(this->renderer);
}
