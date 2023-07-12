#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>

#include "game-world.h"
#include "lib.h"
#include "graphics/sdl.h"


Game::Main *Game::Main::instance = nullptr;
Graphics::Renderer *Game::renderer = nullptr;
Game::Probability Game::probability;


Game::Map::Map ()
{
	this->w = 8;
	this->h = 8;
	this->map = Mylib::Matrix<Cell>(this->h, this->w);
	auto& m = this->map;
	
	std::string map_string = "00000000"
	                         "0p  g  0"
	                         "0    0 0"
	                         "0    0 0"
	                         "0  g   0"
	                         "0 0000 0"
	                         "0   g  0"
	                         "00000000";

	ASSERT(map_string.length() == (this->w * this->h))

	this->n_walls = 0;
	this->pacman_start_x = std::numeric_limits<uint32_t>::max();

	uint32_t k = 0;
	for (uint32_t y=0; y<this->h; y++) {
		for (uint32_t x=0; x<this->w; x++) {
			switch (map_string[k]) {
				case ' ':
					m(y, x) = Cell::Empty;
				break;
				
				case '0':
					m(y, x) = Cell::Wall;
					this->n_walls++;
				break;
				
				case 'p':
					m(y, x) = Cell::Pacman_start;
					this->pacman_start_x = x;
					this->pacman_start_y = y;
				break;

				case 'g':
					m(y, x) = Cell::Ghost_start;
				break;
				
				default:
					ASSERT(0)
			}

			k++;
		}
	}

	ASSERT(this->pacman_start_x != std::numeric_limits<uint32_t>::max())
}

Game::Map::~Map ()
{
}

Game::Main::Main ()
{
}

Game::Main::~Main ()
{
}

void Game::Main::allocate ()
{
	ASSERT(instance == nullptr)

	instance = new Main;
}

void Game::Main::deallocate ()
{
	ASSERT(instance != nullptr)

	delete instance;
}

void Game::Main::load ()
{
	std::cout << std::setprecision(4);
	std::cout << std::fixed;

	this->state = State::initializing;

	SDL_Init( SDL_INIT_VIDEO );

	this->graphics_init();

	dprint( "chorono resolution " << (static_cast<float>(Clock::period::num) / static_cast<float>(Clock::period::den)) << std::endl );

	this->world = nullptr;
	this->world = new World();

	dprint( "loaded world" << std::endl )

	this->alive = true;
}

void Game::Main::graphics_init ()
{
	renderer = new Graphics::SDL::Renderer(Config::screen_width_px, Config::screen_height_px);
}

void Game::Main::run ()
{
	SDL_Event event;
	const Uint8 *keys;
	ClockTime tbegin, tend;
	ClockDuration elapsed;
	float real_dt, virtual_dt, required_dt, sleep_dt;

	this->state = State::playing;

	keys = SDL_GetKeyboardState(nullptr);

	real_dt = 0.0f;
	virtual_dt = 0.0f;
	required_dt = 0.0f;
	sleep_dt = 0.0f;

	while (this->alive) {
		tbegin = Clock::now();

		renderer->wait_next_frame();

		virtual_dt = (real_dt > Config::max_dt) ? Config::max_dt : real_dt;

		//dprintln( "start new frame render required_dt=" << required_dt << " real_dt=" << real_dt << " sleep_dt=" << sleep_dt << " virtual_dt=" << virtual_dt << " max_dt=" << Config::max_dt )

		while ( SDL_PollEvent( &event ) ) {
			switch (event.type) {
				case SDL_QUIT:
					this->alive = false;
				break;
				
				case SDL_KEYDOWN:
					switch (this->state) {
						case State::playing:
							this->world->event_keydown(event.key.keysym.sym);
						break;
					}
			}
		}

		//glBufferData( GL_ARRAY_BUFFER, sizeof(Vertex) * circle_factory.get_n_vertices(), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

		//glBindVertexArray( vao );
		//glDrawArrays( GL_TRIANGLES, 0, circle_factory.get_n_vertices() );

		switch (this->state) {
			case State::playing:
				this->world->physics(virtual_dt, keys);
				this->world->render(virtual_dt);
			break;
			
			default:
				ASSERT(0)
		}

		renderer->render();

		tend = Clock::now();
		elapsed = tend - tbegin;
		required_dt = elapsed.count();

		if (required_dt < Config::sleep_threshold) {
			sleep_dt = Config::sleep_threshold - required_dt;
			uint32_t delay = static_cast<uint32_t>(sleep_dt * 1000.0f);
			//dprintln( "sleeping for " << delay << "ms..." )
			SDL_Delay(delay);
		}
		else
			sleep_dt = 0.0f;

		do {
			tend = Clock::now();
			elapsed = tend - tbegin;
			real_dt = elapsed.count();
		} while (real_dt < Config::target_dt);
	}
}

void Game::Main::cleanup ()
{
	delete renderer;
	
	SDL_Quit();
}

Game::World::World ()
	: time_create( Clock::now() )
	, player(this)
{
	this->w = static_cast<float>( this->map.get_w() );
	this->h = static_cast<float>( this->map.get_h() );

	this->border_thickness = Config::border_thickness_screen_fraction;

	// avoid vector re-allocations
	this->objects.reserve(100);
	this->ghosts.reserve(10);

	this->add_object(player);

	this->player.set_pos( Vector(
		get_cell_center(this->map.get_pacman_start_x()),
		get_cell_center(this->map.get_pacman_start_y())
		));

	// create ghosts

	for (uint32_t y=0; y<this->map.get_h(); y++) {
		for (uint32_t x=0; x<this->map.get_w(); x++) {
			switch (this->map(y, x)) {
				case Map::Cell::Ghost_start:
					Ghost& ghost = this->ghosts.emplace_back(this);
					ghost.set_pos(Vector( get_cell_center(x), get_cell_center(y) ));
					this->add_object(ghost);
				break;
			}
		}
	}
}

Game::World::~World ()
{
}

void Game::World::physics (const float dt, const Uint8 *keys)
{
//	dprintln( "distance between player and ghost[0]: " << Mylib::Math::distance(this->player.get_pos(), this->ghosts[0].get_pos()) )

	for (Object *obj: this->objects) {
		obj->physics(dt, keys);
	}

	this->solve_wall_collisions();
}

void Game::World::solve_wall_collisions ()
{
	for (Object *obj: this->objects) {
		const Vector cell_center = get_cell_center(obj->get_pos());
		const int32_t xi = static_cast<uint32_t>( obj->get_x() );
		const int32_t yi = static_cast<uint32_t>( obj->get_y() );

		if (obj->get_x() < cell_center.x && this->map(yi, xi-1) == Map::Cell::Wall) {
			obj->set_x(cell_center.x);
			obj->set_vx(0.0f);
			obj->collided_with_wall(Object::Direction::Left);
			obj->set_direction(Object::Direction::Stopped);
		}
		else if (obj->get_x() > cell_center.x && this->map(yi, xi+1) == Map::Cell::Wall) {
			obj->set_x(cell_center.x);
			obj->set_vx(0.0f);
			obj->collided_with_wall(Object::Direction::Right);
			obj->set_direction(Object::Direction::Stopped);
		}

		if (obj->get_y() < cell_center.y && this->map(yi-1, xi) == Map::Cell::Wall) {
			obj->set_y(cell_center.y);
			obj->set_vy(0.0f);
			obj->collided_with_wall(Object::Direction::Up);
			obj->set_direction(Object::Direction::Stopped);
		}
		else if (obj->get_y() > cell_center.y && this->map(yi+1, xi) == Map::Cell::Wall) {
			obj->set_y(cell_center.y);
			obj->set_vy(0.0f);
			obj->collided_with_wall(Object::Direction::Down);
			obj->set_direction(Object::Direction::Stopped);
		}
	}
}

void Game::World::render_map ()
{
	const ShapeRect rect(Config::map_tile_size, Config::map_tile_size);
	const uint32_t n_rects = this->map.get_n_walls();
	const Graphics::Color color = { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };
	Vector offset;

	for (uint32_t y=0; y<this->map.get_h(); y++) {
		for (uint32_t x=0; x<this->map.get_w(); x++) {
			switch (this->map(y, x)) {
				case Map::Cell::Wall:
					offset.set(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
					renderer->draw_rect(rect, offset, color);
				break;
			}
		}
	}
}

void Game::World::render_box()
{
	ShapeRect rect;
	Vector offset;
	float w, h;
	const Graphics::Color color = { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
	const Vector ws = renderer->get_normalized_window_size();
	
	w = this->border_thickness;
	h = ws.y;
	offset.set(w*0.5f, ws.y*0.5f);
	rect = ShapeRect(w, h);
	renderer->draw_rect(rect, offset, color);

	offset.set(ws.x - w*0.5f, ws.y*0.5f);
	renderer->draw_rect(rect, offset, color);

	w = ws.x;
	h = this->border_thickness;
	offset.set(ws.x*0.5f, h*0.5f);
	rect = ShapeRect(w, h);
	renderer->draw_rect(rect, offset, color);

	offset.set(ws.x*0.5f, ws.y - h*0.5f);
	renderer->draw_rect(rect, offset, color);
}

void Game::World::render (const float dt)
{
	const Vector ws = renderer->get_normalized_window_size();

	renderer->setup_projection_matrix( Graphics::ProjectionMatrixArgs {
		.clip_init_norm = Vector(this->border_thickness, this->border_thickness),
		.clip_end_norm = Vector(ws.x - this->border_thickness, ws.y - this->border_thickness),
		.world_init = Vector(0.0f, 0.0f),
		.world_end = Vector(this->w, this->h),
		.world_camera_focus = player.get_pos(),
		.world_screen_width = this->w
		} );

	this->render_map();

	for (Object *obj: this->objects) {
		obj->render(dt);
	}

#if 1
	renderer->setup_projection_matrix( Graphics::ProjectionMatrixArgs {
		.clip_init_norm = Vector(0.0f, 0.0f),
		.clip_end_norm = Vector(ws.x, ws.y),
		.world_init = Vector(0.0f, 0.0f),
		.world_end = Vector(ws.x, ws.y),
		.world_camera_focus = Vector(ws.x*0.5f, ws.y*0.5f),
		.world_screen_width = ws.x
		} );
	
	this->render_box();
#endif
}

int main (int argc, char **argv)
{
	Game::Main::allocate();
	
	Game::Main::get()->load();
	Game::Main::get()->run();
	Game::Main::get()->cleanup();

	Game::Main::deallocate();

	return 0;
}