#include <utility>
#include <array>
#include <limits>
#include <algorithm>

#include <cmath>

#include "debug.h"
#include "game-world.h"
#include "game-object.h"
#include "lib.h"


void Game::Object::physics (const float dt, const Uint8 *keys)
{
	this->pos += this->vel * dt;
}

Game::Player::Player (World *world_)
	: Object(world_),
	  shape(Config::pacman_radius)
{
	this->name = "Player";
	this->pos = Vector(0.0f, 0.0f);
	this->vel = Vector(0.0f, 0.0f);
	this->direction = Direction::Stopped;
	this->target_direction = Direction::Stopped;

	//this->base_color = Graphics::Color { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
	//this->color = this->base_color;
	this->color = Color(0.0f, 1.0f, 0.0f, 1.0f);

	this->event_move_d = Events::move.subscribe( Mylib::Event::make_callback_object<Events::Move::Type>(*this, &Player::event_move) );

	dprintln("player created");
}

Game::Player::~Player ()
{
//	Events::keydown.unsubscribe(this->event_keydown_d);
	Events::move.unsubscribe(this->event_move_d);
}

void Game::Player::physics (const float dt, const Uint8 *keys)
{
	const Vector cell_center = get_cell_center(this->pos);
	const Vector dist = cell_center - this->pos;
	const float dist_x = std::abs(dist.x);
	const float dist_y = std::abs(dist.y);
	const int32_t xi = static_cast<uint32_t>( this->get_x() );
	const int32_t yi = static_cast<uint32_t>( this->get_y() );
	const Map& map = this->world->get_ref_map();

	switch (this->target_direction) {
		using enum Direction;

		case Left:
			if (dist_y < Config::pacman_turn_threshold && map[yi, xi-1] != Map::Cell::Wall) {
				this->pos.y = cell_center.y; // teleport to center of cell
				this->vel.x = -Config::pacman_speed;
				this->vel.y = 0.0f;
				this->direction = Left;
			}
		break;

		case Right:
			if (dist_y < Config::pacman_turn_threshold && map[yi, xi+1] != Map::Cell::Wall) {
				this->pos.y = cell_center.y; // teleport to center of cell
				this->vel.x = Config::pacman_speed;
				this->vel.y = 0.0f;
				this->direction = Right;
			}
		break;

		case Up:
			if (dist_x < Config::pacman_turn_threshold && map[yi-1, xi] != Map::Cell::Wall) {
				this->pos.x = cell_center.x; // teleport to center of cell
				this->vel.x = 0.0f;
				this->vel.y = -Config::pacman_speed;
				this->direction = Up;
			}
		break;

		case Down:
			if (dist_x < Config::pacman_turn_threshold && map[yi+1, xi] != Map::Cell::Wall) {
				this->pos.x = cell_center.x; // teleport to center of cell
				this->vel.x = 0.0f;
				this->vel.y = Config::pacman_speed;
				this->direction = Down;
			}
		break;

		case Stopped: break; // clear warnings
	}

	this->Object::physics(dt, keys);
}

void Game::Player::render (const float dt)
{
	this->update_color();
	renderer->draw_circle2D(this->shape, this->pos, this->color);
}

void Game::Player::event_move (const Events::Move::Type& move_data)
{
	this->target_direction = move_data.direction;
}

void Game::Player::update_color ()
{
	float min_distance = std::numeric_limits<float>::max();
	const float w = this->world->get_w();
	const float h = this->world->get_h();
	const float max_world_distance = std::sqrt(w*w + h*h);

	//dprintln("min_distance: " << min_distance)

	for (const Ghost& ghost : this->world->get_ref_ghosts()) {
		const float distance = Mylib::Math::distance(this->get_value_pos(), ghost.get_value_pos());

		if (distance < min_distance)
			min_distance = distance;
	}

	const float dist_ratio = min_distance / max_world_distance;

	//this->color = this->base_color;
	this->color.r = 1.0f - dist_ratio;
	//this->color.r *= 2.0f;
	this->color.g = dist_ratio;
	this->color.b = dist_ratio;

	this->color.r = std::clamp(this->color.r, 0.0f, 1.0f);
	this->color.g = std::clamp(this->color.g, 0.0f, 1.0f);
	this->color.b = std::clamp(this->color.b, 0.0f, 1.0f);

//	dprintln("min_distance: " << min_distance << "  max_world_distance: " << max_world_distance << "  color.r: " << this->color.r)
}

Game::Ghost::Ghost (World *world_)
	: Object(world_),
	  shape(Config::ghost_radius)
{
	static uint32_t ghost_i = 0;

	this->name = "Ghost_" + std::to_string(ghost_i++);
	this->pos = Vector(0.0f, 0.0f);
	this->vel = Vector(0.0f, 0.0f);
	this->direction = Direction::Stopped;
	this->time_last_turn = this->world->get_time_create();
	this->time_between_turns = float_to_ClockDuration(Config::ghost_time_between_turns);

	this->color = Color(0.0f, 0.0f, 0.0f, 1.0f);

	this->event_wall_collision_d = Events::wall_collision.subscribe( Mylib::Event::make_callback_object<Events::WallCollision::Type>(*this, &Ghost::collided_with_wall) );

	/*
		Let's change ghost colors randomly using a coroutine lambda.
	*/
	auto coro = [] (Ghost& ghost) -> Mylib::Coroutine {
		dprintln("Ghost ", ghost.name, " coroutine started");

		constexpr ClockDuration wait_time = float_to_ClockDuration(Config::ghost_color_change_time);

		std::uniform_real_distribution<float> d (0.0f, 1.0f);
		auto& r = probability.get_ref_rgenerator();

		while (true) {
			co_await Events::timer.coroutine_wait(wait_time);

			ghost.color = Color(d(r), d(r), d(r), 1.0f);
		}
	}(*this);

	Mylib::initialize_coroutine(coro);

	dprintln("ghost created");
}

Game::Ghost::~Ghost ()
{
	Events::wall_collision.unsubscribe(this->event_wall_collision_d);
}

void Game::Ghost::collided_with_wall (const Events::WallCollision::Type& event)
{
	if (&event.coll_obj != this)
		return;

	this->time_last_turn = this->world->get_time_create();

//	dprintln("------- Ghost " << this->name << " collided with wall")
}

void Game::Ghost::physics (const float dt, const Uint8 *keys)
{
	const Vector cell_center = get_cell_center(this->pos);
	const Vector dist = cell_center - this->pos;
	const float dist_x = std::abs(dist.x);
	const float dist_y = std::abs(dist.y);
	const int32_t xi = static_cast<uint32_t>( this->get_x() );
	const int32_t yi = static_cast<uint32_t>( this->get_y() );
	const Map& map = this->world->get_ref_map();

	// check if we are in an intersection
	if (dist_x < Config::pacman_turn_threshold && dist_y < Config::pacman_turn_threshold) {
		if (Clock::now() > (this->time_last_turn + this->time_between_turns)) {
			this->time_last_turn = Clock::now();

			// let's check in which adjacent tiles we have walls

			std::array<Direction, 4> possibilities; // max of 4 possible directions
			uint32_t n_possibilities = 0;

			if (map[xi-1, yi] == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Left;
			if (map[xi+1, yi] == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Right;
			if (map[xi, yi-1] == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Up;
			if (map[xi, yi+1] == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Down;

			//dprint("Ghost n_possibilities = " << n_possibilities << " (")

			//for (uint32_t i=0; i<n_possibilities; i++)
			//	dprint( get_direction_str(possibilities[i]) << ", " )
			
			//dprint( ")" << std::endl)

			if (n_possibilities == 0) // ghost is locked in a jail
				return;

			// let's randomize a direction among the possible directions

			uint32_t dice_range = n_possibilities - 1;
			
			// used to reduce the probability of constantly changing direction when moving
			if (this->direction != Direction::Stopped)
				dice_range += 3;
			
			//dprintln("Ghost dice_range = " << dice_range)

			std::uniform_int_distribution<uint32_t> distribution (0, dice_range);
			const uint32_t dice = distribution(probability.get_ref_rgenerator());
			//dprintln("Ghost dice = " << dice)
			const Direction target_direction = (dice < n_possibilities)
			                                 ? static_cast<Direction>( possibilities[dice] )
											 : Direction::Stopped;

			//dprintln("Ghost target_direction = " << get_direction_str(target_direction))

			switch (target_direction) {
				using enum Direction;

				case Left:
					this->pos = cell_center; // teleport to center of cell
					this->vel.x = -Config::pacman_speed;
					this->vel.y = 0.0f;
					this->direction = Left;
				break;

				case Right:
					this->pos = cell_center; // teleport to center of cell
					this->vel.x = Config::pacman_speed;
					this->vel.y = 0.0f;
					this->direction = Right;
				break;

				case Up:
					this->pos = cell_center; // teleport to center of cell
					this->vel.x = 0.0f;
					this->vel.y = -Config::pacman_speed;
					this->direction = Up;
				break;

				case Down:
					this->pos = cell_center; // teleport to center of cell
					this->vel.x = 0.0f;
					this->vel.y = Config::pacman_speed;
					this->direction = Down;
				break;

				case Stopped: break; // clear warnings
			}
		}
	}

	this->Object::physics(dt, keys);
}

void Game::Ghost::render (const float dt)
{
	renderer->draw_circle2D(this->shape, this->pos, this->color);
}