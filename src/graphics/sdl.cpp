#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <utility>

#include <cstdlib>
#include <cmath>

#include "../graphics.h"
#include "../game-object.h"
#include "sdl.h"

// ---------------------------------------------------

Graphics::SDL::Renderer::Renderer (const uint32_t screen_width_px_, const uint32_t screen_height_px_)
	: Graphics::Renderer (screen_width_px_, screen_height_px_)
{
	this->sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->screen_width_px, this->screen_height_px, SDL_WINDOW_SHOWN);
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

void Graphics::SDL::Renderer::draw_circle (const Game::ShapeCircle& circle, const Vector& offset, const Graphics::Color& color)
{
	Game::ShapeRect rect(circle.get_radius(), circle.get_radius());

	rect.set_delta(circle.get_delta());

	this->draw_rect(rect, offset, color);
}

void Graphics::SDL::Renderer::draw_rect (const Game::ShapeRect& rect, const Vector& offset, const Graphics::Color& color)
{
	SDL_Rect sdl_rect;
	const SDL_Color sdl_color = color;
	const Vector world_pos = offset + rect.get_delta();
	//const Vector world_pos = Vector(4.0f, 4.0f);
	const Vector4d clip_pos = this->projection_matrix * Vector4d(world_pos);

	dprintln( "world_pos:" )
	world_pos.println();

	dprintln( "clip_pos:" )
	clip_pos.println();
exit(1);
	sdl_rect.x = clip_pos.x - (rect.get_w() * 0.5f * this->scale_factor);
	sdl_rect.y = clip_pos.y - (rect.get_h() * 0.5f * this->scale_factor);
	sdl_rect.w = rect.get_w() * this->scale_factor;
	sdl_rect.h = rect.get_h() * this->scale_factor;

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
	SDL_RenderFillRect(this->renderer, &sdl_rect);
}

void Graphics::SDL::Renderer::setup_projection_matrix (const ProjectionMatrixArgs&& args)
{
	const Vector clip_init = args.clip_init_per_cent * Vector(static_cast<float>(this->screen_width_px), static_cast<float>(this->screen_height_px));
	const Vector clip_end = args.clip_end_per_cent * Vector(static_cast<float>(this->screen_width_px), static_cast<float>(this->screen_height_px));
	const Vector clip_size = clip_end - clip_init;

	const float aspect_ratio = clip_size.x / clip_size.y;
	const Vector world_screen_size = Vector(args.world_screen_width, args.world_screen_width * aspect_ratio);
	this->scale_factor = clip_size.x / world_screen_size.x;

	Vector world_camera = args.world_camera_focus - Vector(world_screen_size.x*0.5f, world_screen_size.y*0.5f);

	dprint( "clip_init: " )
	clip_init.println();

	dprint( "clip_end: " )
	clip_end.println();

	dprint( "clip_size: " )
	clip_size.println();

	dprintln( "aspect_ratio: " << aspect_ratio )
	dprintln( "scale_factor: " << this->scale_factor )

	dprint( "world_screen_size: " )
	world_screen_size.println();

	dprint( "world_camera_focus: " )
	args.world_camera_focus.println();

	dprint( "world_camera: " )
	world_camera.println();

	Matrix4d translate_camera;
	translate_camera.set_translate(world_camera);
	dprintln( "translation matrix:" )
	translate_camera.println();

	Matrix4d scale;
	scale.set_scale(this->scale_factor);
	dprintln( "scale matrix:" )
	scale.println();

	this->projection_matrix = scale * translate_camera;
	dprintln( "final matrix:" )
	this->projection_matrix.println();
}

void Graphics::SDL::Renderer::render ()
{
	SDL_RenderPresent(this->renderer);
}
