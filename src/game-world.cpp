#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>

#include "debug.h"
#include "game-world.h"
#include "game-object.h"
#include "lib.h"


Game::Main *Game::Main::instance = nullptr;
Graphics::Renderer *Game::renderer = nullptr;
Game::Probability Game::probability;

Game::Events::Timer Game::Events::timer( Clock::now() );
Game::Events::Keyboard Game::Events::key_down;
Game::Events::WallCollision Game::Events::wall_collision;

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

	mylib_assert_exception(map_string.length() == (this->w * this->h))

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
					mylib_assert_exception(0)
			}

			k++;
		}
	}

	mylib_assert_exception(this->pacman_start_x != std::numeric_limits<uint32_t>::max())
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
	mylib_assert_exception(instance == nullptr)

	instance = new Main;
}

void Game::Main::deallocate ()
{
	mylib_assert_exception(instance != nullptr)

	delete instance;
}

void Game::Main::load (const InitConfig& cfg)
{
	std::cout << std::setprecision(4);
	std::cout << std::fixed;

	this->state = State::initializing;
	this->cfg_params = cfg;

	SDL_Init( SDL_INIT_VIDEO );

	renderer = Graphics::init(cfg.renderer_type, cfg.window_width_px, cfg.window_height_px);

	dprintln("chorono resolution ", (static_cast<float>(Clock::period::num) / static_cast<float>(Clock::period::den)));

	this->world = nullptr;
	this->world = new World();

	dprintln("loaded world");

	this->alive = true;
}

void Game::Main::cleanup ()
{
	Graphics::quit(renderer, this->cfg_params.renderer_type);
	SDL_Quit();
}

void Game::Main::run ()
{
	SDL_Event event;
	const Uint8 *keys;
	float real_dt, virtual_dt, required_dt, sleep_dt, busy_wait_dt, fps;

	this->state = State::playing;

	keys = SDL_GetKeyboardState(nullptr);

	real_dt = 0.0f;
	virtual_dt = 0.0f;
	required_dt = 0.0f;
	sleep_dt = 0.0f;
	busy_wait_dt = 0.0f;
	fps = 0.0f;

	while (this->alive) {
		const ClockTime tbegin = Clock::now();
		ClockTime tend;
		ClockDuration elapsed;

		renderer->wait_next_frame();

		Events::timer.trigger_events(tbegin);

		virtual_dt = (real_dt > Config::max_dt) ? Config::max_dt : real_dt;

		//dprintln( "start new frame render target_dt=" << Config::target_dt << " required_dt=" << required_dt << " real_dt=" << real_dt << " sleep_dt=" << sleep_dt << " busy_wait_dt=" << busy_wait_dt << " virtual_dt=" << virtual_dt << " max_dt=" << Config::max_dt << " target_dt=" << Config::target_dt << " fps=" << fps )

		while ( SDL_PollEvent( &event ) ) {
			switch (event.type) {
				case SDL_QUIT:
					this->alive = false;
				break;
				
				case SDL_KEYDOWN:
					Events::key_down.publish(event.key.keysym.sym);
				break;
			}
		}

		switch (this->state) {
			case State::playing:
				this->world->physics(virtual_dt, keys);
				this->world->render(virtual_dt);
			break;
			
			default:
				mylib_assert_exception(0)
		}

		renderer->render();

		const ClockTime trequired = Clock::now();
		elapsed = trequired - tbegin;
		required_dt = ClockDuration_to_float(elapsed);

		if constexpr (Config::sleep_to_save_cpu) {
			if (required_dt < Config::sleep_threshold) {
				sleep_dt = Config::sleep_threshold - required_dt; // target sleep time
				const uint32_t delay = static_cast<uint32_t>(sleep_dt * 1000.0f);
				//dprintln( "sleeping for " << delay << "ms..." )
				SDL_Delay(delay);
			}
		}
		
		const ClockTime tbefore_busy_wait = Clock::now();
		elapsed = tbefore_busy_wait - trequired;
		sleep_dt = ClockDuration_to_float(elapsed); // check exactly time sleeping

		do {
			tend = Clock::now();
			elapsed = tend - tbegin;
			real_dt = ClockDuration_to_float(elapsed);

			if constexpr (!Config::busy_wait_to_ensure_fps)
				break;
		} while (real_dt < Config::target_dt);

		elapsed = tend - tbefore_busy_wait;
		busy_wait_dt = ClockDuration_to_float(elapsed);

		fps = 1.0f / real_dt;
	}
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

	this->wall_color = Graphics::Color { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };
	
	this->event_timer_wall_color_d = Events::timer.schedule_event(Clock::now() + float_to_ClockDuration(Config::map_tile_color_change_time), Mylib::Trigger::make_callback_object<Events::Timer::Event>(*this, &World::change_wall_color));
}

Game::World::~World ()
{
	Events::timer.unschedule_event(this->event_timer_wall_color_d);
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
		const Vector cell_center = get_cell_center(obj->get_value_pos());
		const int32_t xi = static_cast<uint32_t>( obj->get_x() );
		const int32_t yi = static_cast<uint32_t>( obj->get_y() );

		if (obj->get_x() < cell_center.x && this->map(yi, xi-1) == Map::Cell::Wall) {
			obj->set_x(cell_center.x);
			obj->set_vx(0.0f);
			Events::wall_collision.publish( Events::WallCollisionData { .coll_obj = *obj, .direction = Object::Direction::Left } );
			obj->set_direction(Object::Direction::Stopped);
		}
		else if (obj->get_x() > cell_center.x && this->map(yi, xi+1) == Map::Cell::Wall) {
			obj->set_x(cell_center.x);
			obj->set_vx(0.0f);
			Events::wall_collision.publish( Events::WallCollisionData { .coll_obj = *obj, .direction = Object::Direction::Right } );
			obj->set_direction(Object::Direction::Stopped);
		}

		if (obj->get_y() < cell_center.y && this->map(yi-1, xi) == Map::Cell::Wall) {
			obj->set_y(cell_center.y);
			obj->set_vy(0.0f);
			Events::wall_collision.publish( Events::WallCollisionData { .coll_obj = *obj, .direction = Object::Direction::Up } );
			obj->set_direction(Object::Direction::Stopped);
		}
		else if (obj->get_y() > cell_center.y && this->map(yi+1, xi) == Map::Cell::Wall) {
			obj->set_y(cell_center.y);
			obj->set_vy(0.0f);
			Events::wall_collision.publish( Events::WallCollisionData { .coll_obj = *obj, .direction = Object::Direction::Down } );
			obj->set_direction(Object::Direction::Stopped);
		}
	}
}

void Game::World::change_wall_color (Events::Timer::Event& event)
{
	//dprintln("Changing wall color")

	std::uniform_real_distribution<float> d (0.0f, 1.0f);
	auto& r = probability.get_ref_rgenerator();

	this->wall_color = Graphics::Color {
		.r = d(r),
		.g = d(r),
		.b = d(r),
		.a = 1.0f
		};

	event.re_schedule = true;
	event.time = Clock::now() + float_to_ClockDuration(Config::map_tile_color_change_time);
}

void Game::World::render_map ()
{
	const Graphics::ShapeRect rect(Config::map_tile_size, Config::map_tile_size);
	const uint32_t n_rects = this->map.get_n_walls();
	Vector offset;

	for (uint32_t y=0; y<this->map.get_h(); y++) {
		for (uint32_t x=0; x<this->map.get_w(); x++) {
			switch (this->map(y, x)) {
				case Map::Cell::Wall:
					offset.set(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
					renderer->draw_rect(rect, offset, this->wall_color);
				break;
			}
		}
	}
}

void Game::World::render_box()
{
	Graphics::ShapeRect rect;
	Vector offset;
	float w, h;
	const Graphics::Color color = { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
	const Vector ws = renderer->get_normalized_window_size();
	
	w = this->border_thickness;
	h = ws.y;
	offset.set(w*0.5f, ws.y*0.5f);
	rect = Graphics::ShapeRect(w, h);
	renderer->draw_rect(rect, offset, color);

	offset.set(ws.x - w*0.5f, ws.y*0.5f);
	renderer->draw_rect(rect, offset, color);

	w = ws.x;
	h = this->border_thickness;
	offset.set(ws.x*0.5f, h*0.5f);
	rect = Graphics::ShapeRect(w, h);
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
		.force_camera_inside_world = true,
		.world_camera_focus = player.get_value_pos(),
		.world_screen_width = this->w * (1.0f / Main::get()->get_cfg_params().zoom)
		} );
	
	/*renderer->setup_projection_matrix( Graphics::ProjectionMatrixArgs {
		.clip_init_norm = Vector(0.0f, 0.0f),
		.clip_end_norm = Vector(ws.x, ws.y),
		.world_init = Vector(0.0f, 0.0f),
		.world_end = Vector(this->w, this->h),
		.force_camera_inside_world = true,
		.world_camera_focus = player.get_pos(),
		.world_screen_width = this->w * (1.0f / Main::get()->get_cfg_params().zoom)
		} );*/

	this->render_map();

	for (Object *obj: this->objects) {
		obj->render(dt);
	}

#if 0
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
