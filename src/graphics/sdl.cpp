#include <iostream>
#include <fstream>
#include <sstream>
#include <numbers>
#include <utility>

#include <cstdlib>
#include <cmath>

#include "../graphics.h"
#include "sdl.h"


// ---------------------------------------------------


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
	SDL_DestroyWindow(this->sdl_window);
}

void Graphics::SDL::wait_next_frame ()
{
	SDL_SetRenderDrawColor(this->renderer, this->background_color.r, this->background_color.g, this->background_color.b, this->background_color.a);
	SDL_RenderClear(this->renderer);
}

void Graphics::SDL::draw_circle (const Game::ShapeCircle& circle, const float offset_x, const float offset_y, const Graphics::Color& color)
{

}

void Graphics::SDL::draw_rect (const Game::ShapeRect& rect, const float offset_x, const float offset_y, const Graphics::Color& color)
{
	SDL_Rect rect;
	const SDL_Color sdl_color = color;

	rect.x = 100;
	rect.y = 100;
	rect.w = 200;
	rect.h = 300;

	SDL_SetRenderDrawColor(this->renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
	SDL_RenderFillRect(this->renderer, &rect);
}

void Graphics::SDL::set_projection_matrix (const ProjectionMatrix& m)
{
	this->pm = m;
}

void Graphics::SDL::render ()
{
	SDL_RenderPresent(this->renderer);
}
