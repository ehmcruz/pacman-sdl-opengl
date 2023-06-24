#ifndef __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__
#define __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__

#include <my-lib/std.h>

namespace Game
{
namespace Config
{

// ---------------------------------------------------

inline constexpr uint32_t opengl_low_def_triangles = 64;

inline constexpr uint32_t opengl_high_def_triangles = 1024;

inline constexpr float pacman_radius = 0.45f;

inline constexpr float pacman_speed = 2.0f;

inline constexpr float map_tile_size = 1.0f;

// ---------------------------------------------------

} // end namespace Config
} // end namespace Game

#endif