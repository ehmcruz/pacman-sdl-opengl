#ifndef __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__
#define __PACMAN_SDL_OPENGL_CONFIG_HEADER_H__

#include <my-lib/std.h>

namespace Game
{
namespace Config
{

// ---------------------------------------------------

inline constexpr bool sleep_to_save_cpu = true;

inline constexpr bool busy_wait_to_ensure_fps = true;

inline constexpr uint32_t default_window_width_px = 700;

inline constexpr uint32_t default_window_height_px = 700;

inline constexpr float default_zoom = 1.0f;

inline constexpr float border_thickness_screen_fraction = 0.05f;

inline constexpr float pacman_radius = 0.45f;

inline constexpr float pacman_speed = 2.0f;

inline constexpr float ghost_radius = 0.3f;

inline constexpr float ghost_time_between_turns = 0.5f; // in seconds

inline constexpr float map_tile_color_change_time = 2.0f; // in seconds

inline constexpr float ghost_color_change_time = 0.2f; // in seconds

inline constexpr float map_tile_size = 1.0f;

inline constexpr float target_fps = 60.0f;

// if fps gets lower than min_fps, we slow down the simulation
inline constexpr float min_fps = 30.0f;

inline constexpr float target_dt = 1.0f / target_fps;

inline constexpr float max_dt = 1.0f / min_fps;

inline constexpr float sleep_threshold = target_dt * 0.9f;

inline constexpr float pacman_max_delta_per_cycle = pacman_speed * max_dt;

inline constexpr float pacman_turn_threshold = pacman_max_delta_per_cycle * 1.0f;

// ---------------------------------------------------

} // end namespace Config
} // end namespace Game

#endif