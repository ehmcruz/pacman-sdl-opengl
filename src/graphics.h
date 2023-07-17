#ifndef __PACMAN_SDL_GRAPHICS_HEADER_H__
#define __PACMAN_SDL_GRAPHICS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <cstring>

#include <string>
#include <algorithm>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/any.h>
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

class Shape
{
public:
	// let the user store a pointer for what he wants
	Mylib::Any<sizeof(void*), sizeof(void*)> user_data;

	enum class Type {
		Circle,
		Rect
	};
protected:
	OO_ENCAPSULATE_READONLY(Type, type)

	// distance from the center of the shape to the center of the object
	OO_ENCAPSULATE_REFERENCE(Vector, delta)

public:
	inline Shape (const Type type_)
		: type(type_)
	{
		this->delta = {0.0f, 0.0f};
	}

	inline float get_dx () const
	{
		return this->delta.x;
	}

	inline float get_dy () const
	{
		return this->delta.y;
	}

	inline void set_dx (const float dx)
	{
		this->delta.x = dx;
	}

	inline void set_dy (const float dy)
	{
		this->delta.y = dy;
	}
};

// ---------------------------------------------------

class ShapeCircle: public Shape
{
protected:
	OO_ENCAPSULATE(float, radius)

public:
	inline ShapeCircle (const float radius_)
		: Shape (Type::Circle),
		  radius(radius_)
	{
		//dprint( "circle created r=" << this->radius << std::endl )
	}

	inline ShapeCircle ()
		: ShapeCircle (0.0f)
	{
	}
};

// ---------------------------------------------------

class ShapeRect: public Shape
{
protected:
	OO_ENCAPSULATE(float, w)
	OO_ENCAPSULATE(float, h)

public:
	inline ShapeRect (const float w_, const float h_)
		: Shape (Type::Rect),
		  w(w_), h(h_)
	{
		//dprint( "rect created w=" << this->w << " h=" << this->h << std::endl )
	}

	inline ShapeRect ()
		: ShapeRect (0.0f, 0.0f)
	{
	}
};

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
	/* Normalized clip area.
	   Values must be between 0.0f and 1.0f,
	   where 1.0f represents max(clip_width, clip_height).
	*/
	Vector clip_init_norm;
	Vector clip_end_norm;

	/* The minimum and maximum possible coordinates of the world.
	*/
	Vector world_init;
	Vector world_end;

	Vector world_camera_focus;

	/* This is the width in world coords that will fit in the clip_spice.
	   The lower the value, the highest the zoom.
	   This should NEVER be higher then (world_end.x - world_init.x).
	   In case it is, it will be automatically set to (world_end.x - world_init.x).
	*/
	float world_screen_width;
	// world_screen_height will be calculated automatically from the aspect ratio
};

// ---------------------------------------------------

class Renderer
{
public:
	enum class Type { // any change here will need a change in get_type_str
		SDL,
	#ifdef PACMAN_SUPPORT_OPENGL
		Opengl,
	#endif
	#ifdef PACMAN_SUPPORT_VULKAN
		Vulkan,
	#endif
		Unsupported // must be the last one
	};

	static const char* get_type_str (Type t);

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

	inline float get_inverted_window_aspect_ratio () const
	{
		return 1.0f / this->window_aspect_ratio;
	}

	inline Vector get_normalized_window_size () const
	{
		const float max_value = static_cast<float>( std::max(this->window_width_px, this->window_height_px) );
		return Vector(static_cast<float>(this->window_width_px) / max_value, static_cast<float>(this->window_height_px) / max_value);
	}

	virtual void wait_next_frame () = 0;
	virtual void draw_circle (const ShapeCircle& circle, const Vector& offset, const Color& color) = 0;
	virtual void draw_rect (const ShapeRect& rect, const Vector& offset, const Color& color) = 0;
	virtual void setup_projection_matrix (const ProjectionMatrixArgs& args) = 0;
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