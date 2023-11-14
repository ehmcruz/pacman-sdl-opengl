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
#include <my-lib/trigger.h>

#include "graphics.h"
#include "game-object.h"
#include "lib.h"
#include "events.h"

namespace Game
{

// ---------------------------------------------------

extern Graphics::Renderer *renderer;
extern Probability probability;

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Main
{
public:
	struct InitConfig {
		Graphics::Renderer::Type renderer_type;
		uint32_t window_width_px;
		uint32_t window_height_px;
		bool fullscreen;
		float zoom;
	};

	enum class State {
		initializing,
		playing
	};

protected:
	OO_ENCAPSULATE_PTR(World*, world)
	OO_ENCAPSULATE_SCALAR(bool, alive)
	OO_ENCAPSULATE_SCALAR_READONLY(State, state)
	OO_ENCAPSULATE_SCALAR_READONLY(InitConfig, cfg_params)

protected:
	static Main *instance;

	Main ();
	~Main ();

public:
	void load (const InitConfig& cfg);
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
	OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, w)
	OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, h)
	OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, n_walls)
	OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, pacman_start_x)
	OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, pacman_start_y)

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
	// width and height of screen
	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	OO_ENCAPSULATE_SCALAR_READONLY(float, w)
	OO_ENCAPSULATE_SCALAR_READONLY(float, h)
	OO_ENCAPSULATE_SCALAR(ClockTime, time_create) // time instant of world creation
	OO_ENCAPSULATE_SCALAR_READONLY(float, border_thickness)

	OO_ENCAPSULATE_OBJ(Player, player)
	OO_ENCAPSULATE_OBJ_READONLY(std::vector<Ghost>, ghosts)
	OO_ENCAPSULATE_OBJ_READONLY(Map, map)

	Graphics::Color wall_color;
	Events::Timer::Descriptor event_timer_wall_color_d;

protected:
	std::vector< Object* > objects;

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

	void physics (const float dt, const Uint8 *keys);
	void solve_wall_collisions ();
	void change_wall_color (Events::Timer::Event& event);
	void render_map ();
	void render_box();
	void render (const float dt);
};

// ---------------------------------------------------

void die ();

// ---------------------------------------------------

} // end namespace Game

#endif