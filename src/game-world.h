#ifndef __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>
#include <string>
#include <type_traits>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include "graphics.h"
#include "game-object.h"
#include "lib.h"

namespace Game
{

// ---------------------------------------------------

extern Graphics::Renderer *renderer;

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Main
{
public:
	enum class State {
		initializing,
		playing
	};

protected:
	OO_ENCAPSULATE(World*, world)
	OO_ENCAPSULATE(bool, alive)
	OO_ENCAPSULATE_READONLY(State, state)
	OO_ENCAPSULATE_REFERENCE_READONLY(Probability, probability)

protected:
	static Main *instance;

	Main ();
	~Main ();

public:
	void load ();
	void graphics_init ();
	void run ();
	void cleanup ();

	static inline Main* get ()
	{
		return instance;
	}

	static void allocate ();
	static void deallocate ();
};

// ---------------------------------------------------

class Map
{
public:
	enum class Cell {
		Empty,
		Wall,
		Pacman_start,
		Ghost_start
	};

protected:
	Mylib::Matrix<Cell> map;
	OO_ENCAPSULATE_READONLY(uint32_t, w)
	OO_ENCAPSULATE_READONLY(uint32_t, h)
	OO_ENCAPSULATE_READONLY(uint32_t, n_walls)
	OO_ENCAPSULATE_READONLY(uint32_t, pacman_start_x)
	OO_ENCAPSULATE_READONLY(uint32_t, pacman_start_y)

public:
	Map ();
	~Map ();

	inline Cell get (const int row, const int col) const
	{
		return this->map(row, col);
	}

	inline Cell operator() (const int row, const int col) const
	{
		return this->get(row, col);
	}
};

// ---------------------------------------------------

class World
{
protected:
	Graphics::ProjectionMatrix projection_matrix;

	// width and height of screen
	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	OO_ENCAPSULATE_READONLY(float, w)
	OO_ENCAPSULATE_READONLY(float, h)
	OO_ENCAPSULATE(ClockTime, time_create) // time instant of world creation

	OO_ENCAPSULATE_REFERENCE_READONLY(Player, player)
	OO_ENCAPSULATE_REFERENCE_READONLY(Map, map)

protected:
	std::vector< Object* > objects;
	std::vector<Ghost> ghosts;

public:
	World ();
	~World ();

	inline void add_object (Object *obj)
	{
		this->objects.push_back(obj);
	}

	inline void add_object (Object& obj)
	{
		this->objects.push_back(&obj);
	}

	inline void event_keydown (const SDL_Keycode key)
	{
		this->player.event_keydown(key);
	}

	void physics (const float dt, const Uint8 *keys);
	void solve_collisions ();
	void render_map ();
	void render (const float dt);
};

// ---------------------------------------------------

} // end namespace Game

#endif