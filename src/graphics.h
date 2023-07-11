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
#include <my-lib/math-matrix.h>
#include <my-lib/math-vector.h>

#include "lib.h"

// ---------------------------------------------------

namespace Game {
	class ShapeCircle;
	class ShapeRect;
}

// ---------------------------------------------------

namespace Graphics
{

using Game::Vector;
using Mylib::Math::Matrix4d;
using Mylib::Math::Vector4d;

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

struct ProjectionMatrixArgs {
	Vector clip_init_per_cent;
	Vector clip_end_per_cent;
	Vector world_init;
	Vector world_end;
	float world_screen_width;
	// world_screen_height will be calculated automatically from the aspect ratio
	Vector world_camera_focus;
};

// ---------------------------------------------------

class Renderer
{
protected:
	OO_ENCAPSULATE_READONLY(uint32_t, window_width_px)
	OO_ENCAPSULATE_READONLY(uint32_t, window_height_px)
	OO_ENCAPSULATE_READONLY(float, window_aspect_ratio)

public:
	inline Renderer (const uint32_t window_width_px_, const uint32_t window_height_px_)
		: window_width_px(window_width_px_), window_height_px(window_height_px_)
	{
		this->window_aspect_ratio = static_cast<float>(this->window_width_px) / static_cast<float>(this->window_height_px);
	}

	virtual void wait_next_frame () = 0;
	virtual void draw_circle (const Game::ShapeCircle& circle, const Vector& offset, const Color& color) = 0;
	virtual void draw_rect (const Game::ShapeRect& rect, const Vector& offset, const Color& color) = 0;
	virtual void setup_projection_matrix (const ProjectionMatrixArgs&& args) = 0;
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