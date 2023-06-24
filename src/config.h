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

inline constexpr float target_fps = 60.0f;

// if fps gets lower than min_fps, we slow down the simulation
inline constexpr float min_fps = 30.0f;

// here we put derived config

inline constexpr float target_dt = 1.0f / target_fps;

inline constexpr float max_dt = 1.0f / min_fps;

// ---------------------------------------------------

} // end namespace Config

} // end namespace Game

#endif