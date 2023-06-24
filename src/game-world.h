#ifndef __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>
#include <string>
#include <type_traits>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include "opengl.h"
#include "game-object.h"

namespace Game
{

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Map
{
public:
	enum class Cell {
		empty,
		wall,
		pacman_start
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

	inline Cell operator() (const int row, const int col)
	{
		return this->map(row, col);
	}
};

// ---------------------------------------------------

class Main
{
public:
	enum class State {
		initializing,
		playing
	};

protected:
	OO_ENCAPSULATE(SDL_Window*, sdl_window)
	OO_ENCAPSULATE(SDL_GLContext, sdl_gl_context)
	OO_ENCAPSULATE(uint32_t, screen_width_px)
	OO_ENCAPSULATE(uint32_t, screen_height_px)
	OO_ENCAPSULATE(World*, world)
	OO_ENCAPSULATE(bool, alive)
	OO_ENCAPSULATE_READONLY(State, state)
	OO_ENCAPSULATE_READONLY(Opengl::CircleFactory*, opengl_circle_factory_low_def)
	OO_ENCAPSULATE_READONLY(Opengl::CircleFactory*, opengl_circle_factory_high_def)
	OO_ENCAPSULATE_READONLY(Opengl::ProgramTriangle*, opengl_program_triangle)

protected:
	static Main *instance;

	Main ();
	~Main ();

public:
	void load ();
	void load_opengl_programs ();
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

class World
{
protected:
	Opengl::ProjectionMatrix projection_matrix;

	// width and height of screen
	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	OO_ENCAPSULATE_READONLY(float, w)
	OO_ENCAPSULATE_READONLY(float, h)
	//OO_ENCAPSULATE(float, world_to_opengl_conversion)

	OO_ENCAPSULATE_REFERENCE_READONLY(Player*, player)

protected:
	std::vector< Object* > objects;
	Map map;

public:
	World ();
	~World ();

	void bind_vertex_buffer ();

	inline void add_object (Object *obj)
	{
		obj->set_world(this);
		this->objects.push_back(obj);
	}

	void event_keydown (const SDL_Keycode key);
	void physics (const float dt, const Uint8 *keys);
	void render_map ();
	void render (const float dt);
};

// ---------------------------------------------------

} // end namespace Game

#endif