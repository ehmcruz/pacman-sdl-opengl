#ifndef __PACMAN_SD_EVENTS_HEADER_H__
#define __PACMAN_SD_EVENTS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <ostream>

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/trigger.h>
#include <my-lib/timer.h>

#include "lib.h"

namespace Game
{
namespace Events
{

// ---------------------------------------------------

using Keyboard = Mylib::Trigger::EventHandler<SDL_Keycode>;
using Timer = Mylib::Trigger::Timer<Game::ClockTime>;
using Mylib::Trigger::DatalessEventHandler;

// ---------------------------------------------------

struct MoveData {
	#define _MYLIB_ENUM_CLASS_DIRECTION_VALUES_ \
		_MYLIB_ENUM_CLASS_DIRECTION_VALUE_(Left) \
		_MYLIB_ENUM_CLASS_DIRECTION_VALUE_(Right) \
		_MYLIB_ENUM_CLASS_DIRECTION_VALUE_(Up) \
		_MYLIB_ENUM_CLASS_DIRECTION_VALUE_(Down) \
		_MYLIB_ENUM_CLASS_DIRECTION_VALUE_(Stopped)   // must be the last one

	enum class Direction : uint8_t {
		#define _MYLIB_ENUM_CLASS_DIRECTION_VALUE_(V) V,
		_MYLIB_ENUM_CLASS_DIRECTION_VALUES_
		#undef _MYLIB_ENUM_CLASS_DIRECTION_VALUE_
	};

	Direction direction;
};

using Move = Mylib::Trigger::EventHandler<MoveData>;

const char* enum_class_to_str (const Move::Direction value);

inline std::ostream& operator << (std::ostream& out, const Move::Direction value)
{
	out << enum_class_to_str(value);
	return out;
}

// ---------------------------------------------------

extern Keyboard keydown;
extern Timer timer;
extern Move move;
extern DatalessEventHandler quit;

// ---------------------------------------------------

void process_events ();

// ---------------------------------------------------

} // end namespace Events
} // end namespace Game

#endif