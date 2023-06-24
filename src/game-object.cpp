#include <utility>

#include "game-world.h"
#include "game-object.h"
#include "lib.h"

void Game::Object::physics (const float dt, const Uint8 *keys)
{
	this->x += this->vx * dt;
	this->y += this->vy * dt;
}

void Game::Shape::apply_delta (const uint32_t n_vertices, float *x, float *y, const uint32_t stride)
{
	uint32_t j = 0;

	for (uint32_t i=0; i<n_vertices; i++) {
		x[j] += this->dx;
		y[j] += this->dy;
		j += stride;
	}
}

uint32_t Game::ShapeCircle::get_n_vertices ()
{
	return this->factory->get_n_vertices();
}

void Game::ShapeCircle::push_vertices (float *x, float *y, const uint32_t stride)
{
	const uint32_t n = this->get_n_vertices();

	this->factory->fill_vertex_buffer(this->radius, x, y, stride);
	this->apply_delta(n, x, y, stride);
}

void Game::ShapeRect::push_vertices (float *x, float *y, const uint32_t stride)
{
	const float half_w = this->w * 0.5f;
	const float half_h = this->h * 0.5f;
	uint32_t i = 0;

	// let's draw clockwise

	// first triangle
	x[i] = -half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = half_h;
	i += stride;
	x[i] = -half_w;
	y[i] = half_h;

	i += stride;

	// second triangle
	x[i] = -half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = -half_h;
	i += stride;
	x[i] = half_w;
	y[i] = half_h;

	this->apply_delta(fast_get_n_vertices(), x, y, stride);
}

Game::Player::Player (World *world_)
	: Object(world_)
	, shape( this, Config::pacman_radius, Main::get()->get_opengl_circle_factory_low_def() )
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->vx = 0.0f;
	this->vy = 0.0f;
	this->direction = Direction::Stopped;
	this->target_direction = Direction::Stopped;

	this->color = Opengl::Color { .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };

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
	const float cell_center_x = get_cell_center(this->x);
	const float cell_center_y = get_cell_center(this->y);
	const float dist_x = std::abs(cell_center_x - this->x);
	const float dist_y = std::abs(cell_center_y - this->y);
	const int32_t xi = static_cast<uint32_t>( this->get_x() );
	const int32_t yi = static_cast<uint32_t>( this->get_y() );
	const Map& map = this->world->get_map();

	switch (this->target_direction) {
		using enum Direction;

		case Left:
			if (dist_y < Config::pacman_turn_threshold && map(yi, xi-1) != Map::Cell::Wall) {
				this->y = cell_center_y; // teleport to center of cell
				this->vx = -Config::pacman_speed;
				this->vy = 0.0f;
				this->direction = Left;
			}
		break;

		case Right:
			if (dist_y < Config::pacman_turn_threshold && map(yi, xi+1) != Map::Cell::Wall) {
				this->y = cell_center_y; // teleport to center of cell
				this->vx = Config::pacman_speed;
				this->vy = 0.0f;
				this->direction = Right;
			}
		break;

		case Up:
			if (dist_x < Config::pacman_turn_threshold && map(yi-1, xi) != Map::Cell::Wall) {
				this->x = cell_center_x; // teleport to center of cell
				this->vx = 0.0f;
				this->vy = -Config::pacman_speed;
				this->direction = Up;
			}
		break;

		case Down:
			if (dist_x < Config::pacman_turn_threshold && map(yi+1, xi) != Map::Cell::Wall) {
				this->x = cell_center_x; // teleport to center of cell
				this->vx = 0.0f;
				this->vy = Config::pacman_speed;
				this->direction = Down;
			}
		break;
	}

	this->Object::physics(dt, keys);
}

void Game::Player::render (const float dt)
{
	Opengl::ProgramTriangle::Vertex *vertices;
	Opengl::ProgramTriangle *program;
	uint32_t n_vertices;

	n_vertices = this->shape.get_n_vertices();
	dprint( "player allocating space for " << n_vertices << " vertices in vertex_buffer" << std::endl )

	program = Main::get()->get_opengl_program_triangle();
	vertices = program->alloc_vertices(n_vertices);

	this->shape.push_vertices( &(vertices->x), &(vertices->y), Opengl::ProgramTriangle::get_stride() );

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].offset_x = this->x;
		vertices[i].offset_y = this->y;
		vertices[i].r = this->color.r;
		vertices[i].g = this->color.g;
		vertices[i].b = this->color.b;
		vertices[i].a = this->color.a;
	}
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
	: Object(world_)
	, shape( this, Config::ghost_radius, Main::get()->get_opengl_circle_factory_low_def() )
{
	static Opengl::Color ghosts_colors[] = {
		{ .r = 0.5f, .g = 0.1f, .b = 0.0f, .a = 1.0f },
		{ .r = 0.0f, .g = 0.5f, .b = 0.0f, .a = 1.0f },
	};
	static uint32_t color_i = 0;

	this->x = 0.0f;
	this->y = 0.0f;
	this->vx = 0.0f;
	this->vy = 0.0f;
	this->direction = Direction::Stopped;
	this->time_last_turn = this->world->get_time_create();
	this->time_between_turns = ClockDuration(Config::ghost_time_between_turns);

	if (color_i >= (sizeof(ghosts_colors) / sizeof(Opengl::Color)))
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
	const float cell_center_x = get_cell_center(this->x);
	const float cell_center_y = get_cell_center(this->y);
	const float dist_x = std::abs(cell_center_x - this->x);
	const float dist_y = std::abs(cell_center_y - this->y);
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
					this->x = cell_center_x; // teleport to center of cell
					this->y = cell_center_y; // teleport to center of cell
					this->vx = -Config::pacman_speed;
					this->vy = 0.0f;
					this->direction = Left;
				break;

				case Right:
					this->x = cell_center_x; // teleport to center of cell
					this->y = cell_center_y; // teleport to center of cell
					this->vx = Config::pacman_speed;
					this->vy = 0.0f;
					this->direction = Right;
				break;

				case Up:
					this->x = cell_center_x; // teleport to center of cell
					this->y = cell_center_y; // teleport to center of cell
					this->vx = 0.0f;
					this->vy = -Config::pacman_speed;
					this->direction = Up;
				break;

				case Down:
					this->x = cell_center_x; // teleport to center of cell
					this->y = cell_center_y; // teleport to center of cell
					this->vx = 0.0f;
					this->vy = Config::pacman_speed;
					this->direction = Down;
				break;
			}
		}
	}

	this->Object::physics(dt, keys);
}

void Game::Ghost::render (const float dt)
{
	Opengl::ProgramTriangle::Vertex *vertices;
	Opengl::ProgramTriangle *program;
	uint32_t n_vertices;

	n_vertices = this->shape.get_n_vertices();
	dprint( "ghost allocating space for " << n_vertices << " vertices in vertex_buffer" << std::endl )

	program = Main::get()->get_opengl_program_triangle();
	vertices = program->alloc_vertices(n_vertices);

	this->shape.push_vertices( &(vertices->x), &(vertices->y), Opengl::ProgramTriangle::get_stride() );

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].offset_x = this->x;
		vertices[i].offset_y = this->y;
		vertices[i].r = this->color.r;
		vertices[i].g = this->color.g;
		vertices[i].b = this->color.b;
		vertices[i].a = this->color.a;
	}
}