#ifndef __PACMAN_SDL_GRAPHICS_HEADER_H__
#define __PACMAN_SDL_GRAPHICS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <cstring>

#include <string>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include "lib.h"

// ---------------------------------------------------

namespace Game {
	class ShapeCircle;
	class ShapeRect;
}

// ---------------------------------------------------

namespace Graphics
{

using Vector = Game::Vector;

// ---------------------------------------------------

struct Color {
	float r;
	float g;
	float b;
	float a; // alpha
};

inline constexpr Color config_background_color = {
	.r = 0.0f,
	.g = 0.0f,
	.b = 0.0f,
	.a = 1.0f
};

// ---------------------------------------------------

class ProjectionMatrix: public Mylib::StaticMatrix<float, 4, 4>
{
public:
	struct Args {
		float left;
		float right;
		float top;
		float bottom;
		float znear;
		float zfar;
		
		float width;
		float height;
	};

	Args args;

	void setup (const Args&& args);
};

// ---------------------------------------------------

class Renderer
{
protected:
	OO_ENCAPSULATE_READONLY(uint32_t, screen_width_px)
	OO_ENCAPSULATE_READONLY(uint32_t, screen_height_px)

public:
	inline Renderer (const uint32_t screen_width_px_, const uint32_t screen_height_px_)
		: screen_width_px(screen_width_px_), screen_height_px(screen_height_px_)
	{

	}

	virtual void wait_next_frame () = 0;
	virtual void draw_circle (const Game::ShapeCircle& circle, const Vector& offset, const Color& color) = 0;
	virtual void draw_rect (const Game::ShapeRect& rect, const Vector& offset, const Color& color) = 0;
	virtual void set_projection_matrix (const ProjectionMatrix& m) = 0;
	virtual void render () = 0;
};

// ---------------------------------------------------

class CircleFactory
{
private:
	float *table_sin;
	float *table_cos;
	const uint32_t n_triangles;

public:
	CircleFactory (const uint32_t n_triangles_);
	~CircleFactory ();

	inline uint32_t get_n_vertices () const
	{
		return (this->n_triangles * 3);
	}

	void fill_vertex_buffer (const float radius, float *x, float *y, const uint32_t stride) const;
};

// ---------------------------------------------------

} // end namespace Graphics

#endif