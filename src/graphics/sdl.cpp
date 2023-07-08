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

void Graphics::SDL::Renderer::draw_circle (const Game::ShapeCircle& circle, const float offset_x, const float offset_y, const Graphics::Color& color)
{
	Game::ShapeRect rect(circle.get_radius(), circle.get_radius());

	rect.set_dx(circle.get_dx());
	rect.set_dy(circle.get_dy());

	this->draw_rect(rect, offset_x, offset_y, color);
}

void Graphics::SDL::Renderer::draw_rect (const Game::ShapeRect& rect, const float offset_x, const float offset_y, const Graphics::Color& color)
{
	SDL_Rect sdl_rect;
	const SDL_Color sdl_color = color;

	sdl_rect.x = this->transform_x(offset_x - rect.get_w()*0.5f + rect.get_dx());
	sdl_rect.y = this->transform_y(offset_y - rect.get_h()*0.5f + rect.get_dy());
	sdl_rect.w = this->transform_x(rect.get_w());
	sdl_rect.h = this->transform_y(rect.get_h());

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
	SDL_RenderFillRect(this->renderer, &sdl_rect);
}

void Graphics::SDL::Renderer::set_projection_matrix (const ProjectionMatrix& m)
{
	this->pm = m;
}

void Graphics::SDL::Renderer::render ()
{
	SDL_RenderPresent(this->renderer);
}
