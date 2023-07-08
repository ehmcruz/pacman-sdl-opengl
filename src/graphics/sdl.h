#ifndef __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__
#define __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

namespace Graphics
{
namespace SDL
{

// ---------------------------------------------------

class SDL_Color {
private:
	inline void copy (const Graphics::Color& other) {
		this->r = static_cast<Uint8>(other.r * 255.0f);
		this->g = static_cast<Uint8>(other.g * 255.0f);
		this->b = static_cast<Uint8>(other.b * 255.0f);
		this->a = static_cast<Uint8>(other.a * 255.0f);
	}

public:
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;

	inline Color& operator= (const Graphics::Color& other)
	{
		this->copy(other);
		return *this;
	}
};

// ---------------------------------------------------

class Renderer : public Graphics::Renderer
{
protected:
	SDL_Window *sdl_window;
	SDL_Renderer *renderer;
	SDL_Color background_color;
	ProjectionMatrix pm;

public:
	Renderer (const uint32_t screen_width_px_, const uint32_t screen_height_px_);
	~Renderer ();

	void wait_next_frame () override;
	virtual void draw_circle (const Game::ShapeCircle& circle, const float offset_x, const float offset_y, const Graphics::Color& color) override;
	virtual void draw_rect (const Game::ShapeRect& rect, const float offset_x, const float offset_y, const Graphics::Color& color) override;
	virtual void set_projection_matrix (const ProjectionMatrix& m) override;
	virtual void render () override;
};

// ---------------------------------------------------

} // end namespace SDL
} // end namespace Graphics

#endif