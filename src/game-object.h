#ifndef __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include "config.h"
#include "lib.h"
#include "graphics.h"


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
		Circle,
		Rect
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

	inline Shape (const Type type_)
		: Shape (type_, nullptr)
	{
	}
};

// ---------------------------------------------------

class ShapeCircle: public Shape
{
protected:
	OO_ENCAPSULATE(float, radius)

public:
	inline ShapeCircle (const Object *object_, const float radius_)
		: Shape (Type::Circle, object_),
		  radius(radius_)
	{
		dprint( "circle created r=" << this->radius << std::endl )
	}

	inline ShapeCircle (const float radius_)
		: ShapeCircle (nullptr, radius_)
	{
	}

	inline ShapeCircle ()
		: ShapeCircle (nullptr, 0.0f)
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
	inline ShapeRect (const Object *object_, const float w_, const float h_)
		: Shape (Type::Rect, object_),
		  w(w_), h(h_)
	{
		dprint( "rect created w=" << this->w << " h=" << this->h << std::endl )
	}

	inline ShapeRect (const float w_, const float h_)
		: ShapeRect (nullptr, w_, h_)
	{
	}

	inline ShapeRect ()
		: ShapeRect (nullptr, 0.0f, 0.0f)
	{
	}
};

// ---------------------------------------------------

class Object
{
public:
	enum class Direction {
		Left,
		Right,
		Up,
		Down,
		Stopped // must be the last one
	};

protected:
	OO_ENCAPSULATE(float, x)
	OO_ENCAPSULATE(float, y)
	OO_ENCAPSULATE(float, vx)
	OO_ENCAPSULATE(float, vy)
	OO_ENCAPSULATE(World*, world)
	OO_ENCAPSULATE_READONLY(Direction, direction)

public:
	inline Object (World *world_)
		: world(world_)
	{
	}

	virtual void collided_with_wall (Direction direction) = 0;
	virtual void physics (const float dt, const Uint8 *keys);
	virtual void render (const float dt) = 0;
};

// ---------------------------------------------------

class Player: public Object
{
protected:
	ShapeCircle shape;
	Direction target_direction;
	Graphics::Color color;

public:
	Player (World *world_);
	~Player ();

	void collided_with_wall (Direction direction) override;
	void physics (const float dt, const Uint8 *keys) override;
	void render (const float dt) override;

	void event_keydown (const SDL_Keycode key);
};

// ---------------------------------------------------

class Ghost: public Object
{
protected:
	ShapeCircle shape;
	Graphics::Color color;
	ClockTime time_last_turn;
	ClockDuration time_between_turns;

public:
	Ghost (World *world_);
	~Ghost ();

	void collided_with_wall (Direction direction) override;
	void physics (const float dt, const Uint8 *keys) override;
	void render (const float dt) override;
};

// ---------------------------------------------------

} // end namespace Game

#endif