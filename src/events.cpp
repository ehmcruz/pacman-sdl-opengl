#include <array>
#include <vector>

#include "debug.h"
#include "events.h"
#include "lib.h"
#include "game-object.h"
#include "game-world.h"


namespace Game
{
namespace Events
{

// ---------------------------------------------------

const char* enum_class_to_str (const MoveData::Direction value)
{
	static constexpr auto strs = std::to_array<const char*>({
		#define _MYLIB_ENUM_CLASS_DIRECTION_VALUE_(V) #V,
		_MYLIB_ENUM_CLASS_DIRECTION_VALUES_
		#undef _MYLIB_ENUM_CLASS_DIRECTION_VALUE_
	});

	mylib_assert_exception_msg(std::to_underlying(value) < strs.size(), "invalid enum class value ", std::to_underlying(value))

	return strs[ std::to_underlying(value) ];
}

// ---------------------------------------------------

static void key_down_callback (const KeyDown::Type& event)
{
	switch (event.key_code) {
		case SDLK_LEFT:
			move.publish(MoveData { .direction = MoveData::Direction::Left });
		break;

		case SDLK_RIGHT:
			move.publish(MoveData { .direction = MoveData::Direction::Right });
		break;

		case SDLK_UP:
			move.publish(MoveData { .direction = MoveData::Direction::Up });
		break;

		case SDLK_DOWN:
			move.publish(MoveData { .direction = MoveData::Direction::Down });
		break;

		case SDLK_ESCAPE:
			event_manager->quit().publish( {} );
		break;
	}
}

// ---------------------------------------------------

static void touch_screen_move_callback (const TouchScreenMove::Type& event)
{
	switch (event.direction) {
		using enum TouchScreenMove::Type::Direction;

		case Left:
			move.publish(MoveData { .direction = MoveData::Direction::Left });
			break;
		
		case Right:
			move.publish(MoveData { .direction = MoveData::Direction::Right });
			break;
		
		case Up:
			move.publish(MoveData { .direction = MoveData::Direction::Up });
			break;
		
		case Down:
			move.publish(MoveData { .direction = MoveData::Direction::Down });
			break;
	}
}

// ---------------------------------------------------

void setup_events ()
{
	event_manager->key_down().subscribe( Mylib::Event::make_callback_function<KeyDown::Type>(&key_down_callback) );
	event_manager->touch_screen_move().subscribe( Mylib::Event::make_callback_function<TouchScreenMove::Type>(&touch_screen_move_callback) );
}

// ---------------------------------------------------

} // end namespace Events
} // end namespace Game