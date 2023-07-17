#include <utility>
#include <array>
#include <limits>
#include <algorithm>

#include <cmath>

#include "game-world.h"
#include "game-object.h"
#include "lib.h"

const char* Game::Object::get_direction_str (Direction d)
{
	static const char *strs[] = {
		"Left",
		"Right",
		"Up",
		"Down",
		"Stopped"
	};

	const auto i = std::to_underlying(d);

	ASSERT(i <= std::to_underlying(Direction::Stopped))

	return strs[i];
}

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
	this->shape.user_data = this;

	//this->base_color = Graphics::Color { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
	//this->color = this->base_color;
	this->color = Graphics::Color { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };

	this->event_keydown_d = Events::key_down.subscribe( Mylib::Trigger::make_callback_object<Events::Keyboard::Type>(*this, &Player::event_keydown) );

	dprint( "player created" << std::endl )
}

Game::Player::~Player ()
{
	Events::key_down.unsubscribe(this->event_keydown_d);
}

void Game::Player::physics (const float dt, const Uint8 *keys)
{
	const Vector cell_center = get_cell_center(this->pos);
	const Vector dist = cell_center - this->pos;
	const float dist_x = std::abs(dist.x);
	const float dist_y = std::abs(dist.y);
	const int32_t xi = static_cast<uint32_t>( this->get_x() );
	const int32_t yi = static_cast<uint32_t>( this->get_y() );
	const Map& map = this->world->get_map();

	switch (this->target_direction) {
		using enum Direction;

		case Left:
			if (dist_y < Config::pacman_turn_threshold && map(yi, xi-1) != Map::Cell::Wall) {
				this->pos.y = cell_center.y; // teleport to center of cell
				this->vel.x = -Config::pacman_speed;
				this->vel.y = 0.0f;
				this->direction = Left;
			}
		break;

		case Right:
			if (dist_y < Config::pacman_turn_threshold && map(yi, xi+1) != Map::Cell::Wall) {
				this->pos.y = cell_center.y; // teleport to center of cell
				this->vel.x = Config::pacman_speed;
				this->vel.y = 0.0f;
				this->direction = Right;
			}
		break;

		case Up:
			if (dist_x < Config::pacman_turn_threshold && map(yi-1, xi) != Map::Cell::Wall) {
				this->pos.x = cell_center.x; // teleport to center of cell
				this->vel.x = 0.0f;
				this->vel.y = -Config::pacman_speed;
				this->direction = Up;
			}
		break;

		case Down:
			if (dist_x < Config::pacman_turn_threshold && map(yi+1, xi) != Map::Cell::Wall) {
				this->pos.x = cell_center.x; // teleport to center of cell
				this->vel.x = 0.0f;
				this->vel.y = Config::pacman_speed;
				this->direction = Down;
			}
		break;
	}

	this->Object::physics(dt, keys);
}

void Game::Player::render (const float dt)
{
	this->update_color();
	renderer->draw_circle(this->shape, this->pos, this->color);
}

void Game::Player::event_keydown (const Events::Keyboard::Type& key)
{
	switch (key) {
		case SDLK_LEFT:
			this->target_direction = Direction::Left;
		break;

		case SDLK_RIGHT:
			this->target_direction = Direction::Right;
		break;

		case SDLK_UP:
			this->target_direction = Direction::Up;
		break;

		case SDLK_DOWN:
			this->target_direction = Direction::Down;
		break;
	}
}

void Game::Player::update_color ()
{
	float min_distance = std::numeric_limits<float>::max();
	const float w = this->world->get_w();
	const float h = this->world->get_h();
	const float max_world_distance = std::sqrt(w*w + h*h);

	//dprintln("min_distance: " << min_distance)

	for (const Ghost& ghost : this->world->get_ghosts()) {
		const float distance = Mylib::Math::distance(this->get_pos(), ghost.get_pos());

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
	static Graphics::Color ghosts_colors[] = {
		{ .r = 0.5f, .g = 0.1f, .b = 0.0f, .a = 1.0f },
		{ .r = 0.0f, .g = 0.5f, .b = 0.0f, .a = 1.0f },
	};
	static uint32_t ghost_i = 0;

	this->name = "Ghost_" + std::to_string(ghost_i);
	this->pos = Vector(0.0f, 0.0f);
	this->vel = Vector(0.0f, 0.0f);
	this->direction = Direction::Stopped;
	this->time_last_turn = this->world->get_time_create();
	this->time_between_turns = ClockDuration(Config::ghost_time_between_turns);
	this->shape.user_data = this;

	this->color = ghosts_colors[ ghost_i % (sizeof(ghosts_colors) / sizeof(Graphics::Color)) ];
	ghost_i++;

	WallCollisionFilter wall_collision_filter = WallCollisionFilter { .myself = *this };

	this->event_wall_collision_d = Events::wall_collision.subscribe( Mylib::Trigger::make_filter_callback_object<Events::WallCollision::Type>(wall_collision_filter, *this, &Ghost::collided_with_wall) );

	dprint( "ghost created" << std::endl )
}

Game::Ghost::~Ghost ()
{
	Events::wall_collision.unsubscribe(this->event_wall_collision_d);
}

void Game::Ghost::collided_with_wall (const Events::WallCollision::Type& event, const WallCollisionFilter& filter)
{
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
	const Map& map = this->world->get_map();

	// check if we are in an intersection
	if (dist_x < Config::pacman_turn_threshold && dist_y < Config::pacman_turn_threshold) {
		if (Clock::now() > (this->time_last_turn + this->time_between_turns)) {
			this->time_last_turn = Clock::now();

			// let's check in which adjacent tiles we have walls

			std::array<Direction, 4> possibilities; // max of 4 possible directions
			uint32_t n_possibilities = 0;

			if (map(xi-1, yi) == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Left;
			if (map(xi+1, yi) == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Right;
			if (map(xi, yi-1) == Map::Cell::Empty)
				possibilities[n_possibilities++] = Direction::Up;
			if (map(xi, yi+1) == Map::Cell::Empty)
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
			const uint32_t dice = distribution(probability.get_rgenerator());
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
			}
		}
	}

	this->Object::physics(dt, keys);
}

void Game::Ghost::render (const float dt)
{
	renderer->draw_circle(this->shape, this->pos, this->color);
}