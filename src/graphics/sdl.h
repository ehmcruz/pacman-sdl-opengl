#ifndef __PACMAN_GRAPHICS_SDL_HEADER_H__
#define __PACMAN_GRAPHICS_SDL_HEADER_H__

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

	SDL_Color () = default;

	inline SDL_Color (const Graphics::Color& other)
	{
		this->copy(other);
	}

	inline SDL_Color& operator= (const Graphics::Color& other)
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
	ProjectionMatrix m;

public:
	Renderer (const uint32_t screen_width_px_, const uint32_t screen_height_px_);
	~Renderer ();

	void wait_next_frame () override;
	virtual void draw_circle (const Game::ShapeCircle& circle, const Vector& offset, const Graphics::Color& color) override;
	virtual void draw_rect (const Game::ShapeRect& rect, const Vector& offset, const Graphics::Color& color) override;
	virtual void setup_projection_matrix (const ProjectionMatrixArgs&& args) override;
	virtual void render () override;

	/*inline int32_t transform_x (float x)
	{
		const ProjectionMatrix::Args& m = this->pm.args;

		x /= m.width;
		x *= static_cast<float>(this->screen_width_px);

		return static_cast<int32_t>(x);
	}

	inline int32_t transform_y (float y)
	{
		const ProjectionMatrix::Args& m = this->pm.args;

		y /= m.height;
		y *= static_cast<float>(this->screen_height_px);

		return static_cast<int32_t>(y);
	}*/
};

// ---------------------------------------------------

} // end namespace SDL
} // end namespace Graphics

#endif