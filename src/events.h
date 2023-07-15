#ifndef __PACMAN_SD_EVENTS_HEADER_H__
#define __PACMAN_SD_EVENTS_HEADER_H__S

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/trigger.h>
#include <my-lib/timer.h>

#include "lib.h"

namespace Game
{

// ---------------------------------------------------

namespace Events
{
	using Keyboard = Mylib::Trigger::EventHandler<SDL_Keycode>;
	using Timer = Mylib::Trigger::Timer<Game::ClockTime>;

	extern Keyboard key_down;
	extern Timer timer;
};

// ---------------------------------------------------

} // end namespace Game

#endif