#ifndef __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdint.h>

#include "config.h"
#include "lib.h"
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
		circle
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

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, uint32_t stride) override;
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