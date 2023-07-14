#ifndef __PACMAN_SD_EVENTS_HEADER_H__
#define __PACMAN_SD_EVENTS_HEADER_H__S

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/trigger.h>

namespace Game
{

// ---------------------------------------------------

namespace Events
{
	using Keyboard = Mylib::Trigger::EventHandler<SDL_Keycode>;

	extern Keyboard key_down;
};

// ---------------------------------------------------

} // end namespace Game

#endif