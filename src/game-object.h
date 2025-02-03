#ifndef __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <string>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include <my-game-lib/my-game-lib.h>

#include "config.h"
#include "lib.h"
#include "events.h"


namespace Game
{

// ---------------------------------------------------

using MyGlib::Graphics::Rect2D;
using MyGlib::Graphics::Circle2D;

// ---------------------------------------------------

class World;
class Object;

// ---------------------------------------------------

class Object
{
public:
	using Direction = Events::MoveData::Direction;

protected:
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, pos)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, vel)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(std::string, name)
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)
	MYLIB_OO_ENCAPSULATE_SCALAR(Direction, direction)

public:
	inline Object (World *world_)
		: world(world_)
	{
	}

	virtual ~Object () = default;

	inline float get_x () const
	{
		return this->pos.x;
	}

	inline float get_y () const
	{
		return this->pos.y;
	}

	inline void set_x (const float x)
	{
		this->pos.x = x;
	}

	inline void set_y (const float y)
	{
		this->pos.y = y;
	}

	inline float get_vx () const
	{
		return this->vel.x;
	}

	inline float get_vy () const
	{
		return this->vel.y;
	}

	inline void set_vx (const float vx)
	{
		this->vel.x = vx;
	}

	inline void set_vy (const float vy)
	{
		this->vel.y = vy;
	}

	virtual void physics (const float dt, const Uint8 *keys);
	virtual void render (const float dt) = 0;
};

// ---------------------------------------------------

class Player : public Object
{
protected:
	Circle2D shape;
	Direction target_direction;
	Color color;
	//Graphics::Color base_color;
	Events::Move::Descriptor event_move_d;

public:
	Player (World *world_);
	~Player ();

	void physics (const float dt, const Uint8 *keys) override final;
	void render (const float dt) override final;

	void event_move (const Events::Move::Type& move_data);

	void update_color ();
};

// ---------------------------------------------------

class Ghost : public Object
{
protected:
	Circle2D shape;
	Color color;
	ClockTime time_last_turn;
	ClockDuration time_between_turns;
	Events::WallCollision::Descriptor event_wall_collision_d;

	struct WallCollisionFilter
	{
		Object& myself;

		inline bool operator() (const Events::WallCollision::Type& event)
		{
			//dprintln("test-coll " << event.coll_obj->get_name() << "   myself is " << this->myself->get_name())
			return (&this->myself == &event.coll_obj);
		}
	};

public:
	Ghost (World *world_);
	~Ghost ();

	void collided_with_wall (const Events::WallCollision::Type& event, const WallCollisionFilter& filter);
	void physics (const float dt, const Uint8 *keys) override final;
	void render (const float dt) override final;
};

// ---------------------------------------------------

} // end namespace Game

#endif