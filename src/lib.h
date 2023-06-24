#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#include <cmath>

#include <my-lib/std.h>

namespace Game
{

// ---------------------------------------------------

inline float get_cell_center (float pos)
{
	return std::floor(pos) + 0.5f;
}

inline float get_cell_center (uint32_t pos)
{
	return get_cell_center( static_cast<float>(pos) );
}

// ---------------------------------------------------

} // end namespace Game

#endif