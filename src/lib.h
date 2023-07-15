#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#include <chrono>
#include <random>

#include <cmath>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/math-vector.h>

namespace Game
{

// ---------------------------------------------------

using Clock = std::chrono::steady_clock;
using ClockDuration = std::chrono::duration<float>;
//using ClockTime = Clock::time_point;
using ClockTime = std::chrono::time_point<Clock, ClockDuration>;

using Vector = Mylib::Math::Vector2d;

// ---------------------------------------------------

inline int32_t round_to_nearest (const float v)
{
	return static_cast<int32_t>(v + 0.5f);
}

// ---------------------------------------------------

class Probability
{
private:
	OO_ENCAPSULATE_REFERENCE_READONLY(std::mt19937_64, rgenerator);

public:
	Probability ();
};

// ---------------------------------------------------

inline Vector get_cell_center (const Vector& pos)
{
	return Vector(std::floor(pos.x) + 0.5f, std::floor(pos.y) + 0.5f);
}

inline Vector get_cell_center (const Vector&& pos)
{
	return Vector(std::floor(pos.x) + 0.5f, std::floor(pos.y) + 0.5f);
}

inline float get_cell_center (const float pos)
{
	return std::floor(pos) + 0.5f;
}

inline float get_cell_center (const uint32_t pos)
{
	return get_cell_center( static_cast<float>(pos) + 0.5f );
}

// ---------------------------------------------------

} // end namespace Game

#endif