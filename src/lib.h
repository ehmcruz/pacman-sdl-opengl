#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#include <chrono>
#include <random>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/macros.h>

namespace Game
{

// ---------------------------------------------------

using Clock = std::chrono::steady_clock;
using ClockTime = Clock::time_point;
using ClockDuration = std::chrono::duration<float>;

class Probability
{
private:
	OO_ENCAPSULATE_REFERENCE_READONLY(std::mt19937_64, rgenerator);

public:
	Probability ();
};

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