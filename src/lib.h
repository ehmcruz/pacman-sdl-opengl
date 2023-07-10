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
using ClockTime = Clock::time_point;
using ClockDuration = std::chrono::duration<float>;

using Vector = Mylib::Math::Vector<2>;

// ---------------------------------------------------

class Probability
{
private:
	OO_ENCAPSULATE_REFERENCE_READONLY(std::mt19937_64, rgenerator);

public:
	Probability ();
};

// ---------------------------------------------------

inline Vector get_cell_center (Vector& pos)
{
	return Vector(std::floor(pos.x) + 0.5f, std::floor(pos.y) + 0.5f);
}

inline Vector get_cell_center (Vector&& pos)
{
	return Vector(std::floor(pos.x) + 0.5f, std::floor(pos.y) + 0.5f);
}

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