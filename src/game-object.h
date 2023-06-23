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
	OO_ENCAPSULATE(Object*, object)

	// distance from the center of the shape to the center of the object
	OO_ENCAPSULATE(float, dx)
	OO_ENCAPSULATE(float, dy)

public:
	inline Shape (Type type, Object *object)
	{
		this->type = type;
		this->object = object;
		this->dx = 0.0f;
		this->dy = 0.0f;
	}

	inline Shape (Type type)
		: Shape (type, nullptr)
	{
	}

	void apply_delta (uint32_t n_vertices, float *x, float *y, uint32_t stride);
	
	inline void apply_delta (float *x, float *y, uint32_t stride)
	{
		this->apply_delta( this->get_n_vertices(), x, y, stride );
	}

	virtual uint32_t get_n_vertices () = 0;
	virtual void push_vertices (float *x, float *y, uint32_t stride) = 0;
};

// ---------------------------------------------------

class Shape_circle: public Shape
{
protected:
	Opengl::Circle_factory *factory;

	OO_ENCAPSULATE(float, radius)

public:
	inline Shape_circle (Object *object, float radius, Opengl::Circle_factory *factory)
		: Shape (Type::circle, object)
	{
		this->factory = factory;
		this->radius = radius;

		dprint( "circle created r=" << this->radius << std::endl )
	}

	inline Shape_circle (float radius, Opengl::Circle_factory *factory)
		: Shape_circle (nullptr, radius, factory)
	{
	}

	inline Shape_circle (Opengl::Circle_factory *factory)
		: Shape_circle (nullptr, 0.0f, factory)
	{
	}

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, uint32_t stride) override;
};

// ---------------------------------------------------

class Shape_rect: public Shape
{
protected:
	OO_ENCAPSULATE(float, w)
	OO_ENCAPSULATE(float, h)

public:
	inline Shape_rect (Object *object, float w, float h)
		: Shape (Type::rect, object)
	{
		this->w = w;
		this->h = h;

		dprint( "rect created w=" << this->w << " h=" << this->h << std::endl )
	}

	inline Shape_rect (float w, float h)
		: Shape_rect (nullptr, w, h)
	{
	}

	inline Shape_rect ()
		: Shape_rect (nullptr, 0.0f, 0.0f)
	{
	}

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, uint32_t stride) override;

	constexpr uint32_t fast_get_n_vertices ()
	{
		return 6; // 2 triangles
	}
};

// ---------------------------------------------------

class Object
{
protected:
	OO_ENCAPSULATE(float, x)
	OO_ENCAPSULATE(float, y)
	OO_ENCAPSULATE(float, vx)
	OO_ENCAPSULATE(float, vy)
	OO_ENCAPSULATE(World*, game_world)

public:
	void physics (float dt);
	virtual void render (float dt) = 0;
};

// ---------------------------------------------------

class Player: public Object
{
protected:
	Shape_circle *shape;

public:
	Player ();
	~Player ();

	void render (float dt) override;
};

// ---------------------------------------------------

} // end namespace Game

#endif