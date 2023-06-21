#ifndef __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__
#define __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__

#include <stdint.h>

namespace Game
{
namespace Config
{

// ---------------------------------------------------

inline const uint32_t opengl_low_def_triangles = 64;

inline const uint32_t opengl_high_def_triangles = 1024;

inline const float pacman_radius = 0.45f;

inline const float pacman_speed = 2.0f;

inline const float map_tile_size = 1.0f;

// ---------------------------------------------------

} // end namespace Config
} // end namespace Game

#endif