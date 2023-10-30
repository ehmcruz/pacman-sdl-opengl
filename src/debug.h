#ifndef __PACMAN_SDL_OPENGL_DEBUG_HEADER_H__
#define __PACMAN_SDL_OPENGL_DEBUG_HEADER_H__

#include <iostream>

#include <my-lib/std.h>

namespace Game
{

// ---------------------------------------------------

#define DEBUG

#ifdef DEBUG
	template <typename... Types>
	void dprint (Types&&... vars)
	{
		Mylib::print_stream(std::cout, vars...);
	}

	template <typename... Types>
	void dprintln (Types&&... vars)
	{
		Mylib::print_stream(std::cout, vars..., '\n');
	}
#else
	#define dprint(...)
	#define dprintln(...)
#endif

// ---------------------------------------------------

} // end namespace Game

#endif