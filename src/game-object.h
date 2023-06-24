#ifndef __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include "config.h"
#include "opengl.h"

namespace Game
{

// ---------------------------------------------------

class World;
class Object;

// ---------------------------------------------------

class Shape
{
public:
	enum class Type {
		circle,
		rect
	};
protected:
	OO_ENCAPSULATE_READONLY(Type, type)
	OO_ENCAPSULATE(const Object*, object)

	// distance from the center of the shape to the center of the object
	OO_ENCAPSULATE(float, dx)
	OO_ENCAPSULATE(float, dy)

public:
	inline Shape (const Type type_, const Object *object_)
		: type(type_), object(object_)
	{
		this->dx = 0.0f;
		this->dy = 0.0f;
	}

	inline Shape (const Type type)
		: Shape (type, nullptr)
	{
	}

	void apply_delta (const uint32_t n_vertices, float *x, float *y, const uint32_t stride);
	
	inline void apply_delta (float *x, float *y, const uint32_t stride)
	{
		this->apply_delta( this->get_n_vertices(), x, y, stride );
	}

	virtual uint32_t get_n_vertices () = 0;
	virtual void push_vertices (float *x, float *y, const uint32_t stride) = 0;
};

// ---------------------------------------------------

class ShapeCircle: public Shape
{
protected:
	const Opengl::CircleFactory *factory;

	OO_ENCAPSULATE(float, radius)

public:
	inline ShapeCircle (const Object *object, const float radius, const Opengl::CircleFactory *factory)
		: Shape (Type::circle, object)
	{
		this->factory = factory;
		this->radius = radius;

		dprint( "circle created r=" << this->radius << std::endl )
	}

	inline ShapeCircle (const float radius, const Opengl::CircleFactory *factory)
		: ShapeCircle (nullptr, radius, factory)
	{
	}

	inline ShapeCircle (const Opengl::CircleFactory *factory)
		: ShapeCircle (nullptr, 0.0f, factory)
	{
	}

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, const uint32_t stride) override;
};

// ---------------------------------------------------

class ShapeRect: public Shape
{
protected:
	OO_ENCAPSULATE(float, w)
	OO_ENCAPSULATE(float, h)

public:
	inline ShapeRect (const Object *object, const float w, const float h)
		: Shape (Type::rect, object)
	{
		this->w = w;
		this->h = h;

		dprint( "rect created w=" << this->w << " h=" << this->h << std::endl )
	}

	inline ShapeRect (const float w, const float h)
		: ShapeRect (nullptr, w, h)
	{
	}

	inline ShapeRect ()
		: ShapeRect (nullptr, 0.0f, 0.0f)
	{
	}

	constexpr uint32_t get_n_vertices () override
	{
		return 6;
	};
	
	void push_vertices (float *x, float *y, uint32_t const stride) override;
};

// ---------------------------------------------------

class Object
{
protected:
	OO_ENCAPSULATE(float, x)
	OO_ENCAPSULATE(float, y)
	OO_ENCAPSULATE(float, vx)
	OO_ENCAPSULATE(float, vy)
	OO_ENCAPSULATE(const World*, world)

public:
	void physics (const float dt);
	virtual void render (const float dt) = 0;
};

// ---------------------------------------------------

class Player: public Object
{
protected:
	ShapeCircle *shape;

public:
	Player ();
	~Player ();

	void render (const float dt) override;
};

// ---------------------------------------------------

} // end namespace Game

#endif