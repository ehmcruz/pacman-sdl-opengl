#include <utility>

#include "game-world.h"
#include "game-object.h"
#include "lib.h"

void Game::Object::physics (const float dt, const Uint8 *keys)
{
	this->pos.x += this->vel.x * dt;
	this->pos.y += this->vel.y * dt;
}

Game::Player::Player (World *world_)
	: Object(world_),
	  shape(this, Config::pacman_radius)
{
	this->pos = Vector(0.0f, 0.0f);
	this->vel = Vector(0.0f, 0.0f);
	this->direction = Direction::Stopped;
	this->target_direction = Direction::Stopped;

	this->color = Graphics::Color { .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };

	dprint( "player created" << std::endl )
}

Game::Player::~Player ()
{
}

void Game::Player::collided_with_wall (Direction direction)
{

}

void Game::Player::physics (const float dt, const Uint8 *keys)
{
	const Vector cell_center = get_cell_center(this->pos);
	const float dist_x = std::abs(cell_center.x - this->get_x() );
	const float dist_y = std::abs(cell_center.y - this->get_y() );
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
	renderer->draw_circle(this->shape, this->pos, this->color);
}

void Game::Player::event_keydown (const SDL_Keycode key)
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

Game::Ghost::Ghost (World *world_)
	: Object(world_),
	  shape(this, Config::ghost_radius)
{
	static Graphics::Color ghosts_colors[] = {
		{ .r = 0.5f, .g = 0.1f, .b = 0.0f, .a = 1.0f },
		{ .r = 0.0f, .g = 0.5f, .b = 0.0f, .a = 1.0f },
	};
	static uint32_t color_i = 0;

	this->pos = Vector(0.0f, 0.0f);
	this->vel = Vector(0.0f, 0.0f);
	this->direction = Direction::Stopped;
	this->time_last_turn = this->world->get_time_create();
	this->time_between_turns = ClockDuration(Config::ghost_time_between_turns);

	if (color_i >= (sizeof(ghosts_colors) / sizeof(Graphics::Color)))
		color_i = 0;
	this->color = ghosts_colors[color_i++];

	dprint( "ghost created" << std::endl )
}

Game::Ghost::~Ghost ()
{
}

void Game::Ghost::collided_with_wall (Direction direction)
{
	this->time_last_turn = this->world->get_time_create();
}

void Game::Ghost::physics (const float dt, const Uint8 *keys)
{
	const Vector cell_center = get_cell_center(this->pos);
	const float dist_x = std::abs(cell_center.x - this->get_x() );
	const float dist_y = std::abs(cell_center.y - this->get_y() );
	const int32_t xi = static_cast<uint32_t>( this->get_x() );
	const int32_t yi = static_cast<uint32_t>( this->get_y() );
	const Map& map = this->world->get_map();

	// check if we are in an intersection
	if (dist_x < Config::pacman_turn_threshold && dist_y < Config::pacman_turn_threshold) {
		if (Clock::now() > (this->time_last_turn + this->time_between_turns)) {
			this->time_last_turn = Clock::now();

			// let's randomize a direction
			std::uniform_int_distribution<uint32_t> distribution (0, 5);
			const uint32_t direction_ = distribution(Main::get()->get_probability().get_rgenerator());
			const Direction target_direction = (direction_ < std::to_underlying(Direction::Stopped))
			                                 ? static_cast<Direction>(direction_)
											 : Direction::Stopped;

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