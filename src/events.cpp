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

Move move;
DataLessEvent quit;
Keyboard keydown;
WallCollision wall_collision;

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

static void process_keydown (SDL_KeyboardEvent& event)
{
	keydown.publish(event.keysym.sym);

	switch (event.keysym.sym) {
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
	}
}

// ---------------------------------------------------

#ifdef __ANDROID__
	struct FingerEvent {
		bool free = true;
		SDL_TouchID touch_id;
		SDL_FingerID finger_id;
		uint64_t global_id;
		Vector norm_down_pos;
		Vector norm_last_pos;
	};

	static std::vector<FingerEvent> finger_events;
	static uint64_t global_touch_id = 0;
#endif

// ---------------------------------------------------

#ifdef __ANDROID__
	static void finger_event_check_trigger (const FingerEvent& fe)
	{
		const float dx = fe.norm_last_pos.x - fe.norm_down_pos.x;
		const float dy = fe.norm_last_pos.y - fe.norm_down_pos.y;
		constexpr float norm_threshold = 0.2f;

		const uint32_t window_width_px = renderer->get_window_width_px();
		const uint32_t window_height_px = renderer->get_window_height_px();

		const float dx_px = dx * static_cast<float>(window_width_px);
		const float dy_px = dy * static_cast<float>(window_height_px);

		const float threshold = norm_threshold * static_cast<float>(std::min(window_width_px, window_height_px));

		if (std::abs(dx_px) > threshold) {
			if (dx_px < 0.0f)
				move.publish(MoveData { .direction = MoveData::Direction::Left });
			else
				move.publish(MoveData { .direction = MoveData::Direction::Right });
		}
		else if (std::abs(dy_px) > threshold) {
			if (dy_px < 0.0f)
				move.publish(MoveData { .direction = MoveData::Direction::Up });
			else
				move.publish(MoveData { .direction = MoveData::Direction::Down });
		}
	}

	static FingerEvent& find_finger_event (const SDL_TouchID touch_id, const SDL_FingerID finger_id)
	{
		for (auto& event : finger_events) {
			if (event.touch_id == touch_id && event.finger_id == finger_id)
				return event;
		}

		mylib_assert_exception_msg(false, "finger event not found: touch_id=", touch_id, " finger_id=", finger_id)
	}

	static void process_fingerdown (const SDL_TouchFingerEvent& event)
	{
		FingerEvent *fe = nullptr;

		// check for a free slot
		for (auto& event : finger_events) {
			if (event.free) {
				fe = &event;
				break;
			}
		}

		if (fe == nullptr) // no free slot, allocate one
			fe = &finger_events.emplace_back();
		
		fe->free = false;
		fe->touch_id = event.touchId;
		fe->finger_id = event.fingerId;
		fe->global_id = global_touch_id++;
		fe->norm_down_pos = Vector(event.x, event.y);
		fe->norm_last_pos = fe->norm_down_pos;
	}

	static void process_fingermotion (const SDL_TouchFingerEvent& event_)
	{

	}

	static void process_fingerup (const SDL_TouchFingerEvent& event_)
	{
		FingerEvent& fe = find_finger_event(event_.touchId, event_.fingerId);

		fe.norm_last_pos = Vector(event_.x, event_.y);
		finger_event_check_trigger(fe);

		fe.free = true;
	}
#endif

// ---------------------------------------------------

void process_events ()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				quit.publish({});
			break;
			
			case SDL_KEYDOWN:
				process_keydown(event.key);
			break;

		#ifdef __ANDROID__
			case SDL_FINGERMOTION:
				process_fingermotion(event.tfinger);
			break;

			case SDL_FINGERDOWN:
				process_fingerdown(event.tfinger);
			break;

			case SDL_FINGERUP:
				process_fingerup(event.tfinger);
			break;
		#endif
		}
	}
}

// ---------------------------------------------------

} // end namespace Events
} // end namespace Game