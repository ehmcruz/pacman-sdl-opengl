#ifndef __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>
#include <list>
#include <string>
#include <type_traits>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>
#include <my-lib/event.h>

#include "game-object.h"
#include "lib.h"
#include "events.h"

namespace Game
{

// ---------------------------------------------------

inline MyGlib::Graphics::Manager *renderer = nullptr;
inline MyGlib::Event::Manager *event_manager = nullptr;
inline Probability probability;

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Main
{
public:
	struct InitConfig {
		MyGlib::Graphics::Manager::Type graphics_type;
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
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)
	MYLIB_OO_ENCAPSULATE_SCALAR(bool, alive)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(State, state)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(InitConfig, cfg_params)

	MyGlib::Event::Quit::Descriptor event_quit_d;
	MyGlib::Lib *lib;

protected:
	static inline Main *instance = nullptr;

	Main ();
	~Main ();

public:
	void load (const InitConfig& cfg);
	void run ();
	void cleanup ();
	void event_quit (const MyGlib::Event::Quit::Type);

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
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, w)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, h)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, n_walls)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, pacman_start_x)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(uint32_t, pacman_start_y)

public:
	Map ();
	~Map ();

	inline Cell get (const int row, const int col) const
	{
		return this->map[row, col];
	}

	inline Cell operator[] (const int row, const int col) const
	{
		return this->map[row, col];
	}

	inline Cell& operator[] (const int row, const int col)
	{
		return this->map[row, col];
	}
};

// ---------------------------------------------------

class World
{
protected:
	// width and height of screen
	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(float, w)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(float, h)
	MYLIB_OO_ENCAPSULATE_SCALAR(ClockTime, time_create) // time instant of world creation
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(float, border_thickness)

	MYLIB_OO_ENCAPSULATE_OBJ(Player, player)
	MYLIB_OO_ENCAPSULATE_OBJ_READONLY(std::list<Ghost>, ghosts)
	MYLIB_OO_ENCAPSULATE_OBJ_READONLY(Map, map)

	Color wall_color;
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