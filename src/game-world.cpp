#include <iostream>
#include <iomanip>
#include <chrono>
#include <limits>

#include "game-world.h"

Game::Main* Game::Main::instance = nullptr;

Game::Map::Map ()
{
	this->w = 8;
	this->h = 8;
	this->map = new Mylib::Matrix<Cell>(this->h, this->w);
	auto& m = *(this->map);
	
	std::string map_string = "00000000"
	                         "0p     0"
	                         "0    0 0"
	                         "0    0 0"
	                         "0      0"
	                         "0 0000 0"
	                         "0      0"
	                         "00000000";

	ASSERT(map_string.length() == (this->w * this->h))

	this->n_walls = 0;
	this->pacman_start_x = std::numeric_limits<uint32_t>::max();

	uint32_t k = 0;
	for (uint32_t y=0; y<this->h; y++) {
		for (uint32_t x=0; x<this->w; x++) {
			switch (map_string[k]) {
				case ' ':
					m(y, x) = Cell::empty;
				break;
				
				case '0':
					m(y, x) = Cell::wall;
					this->n_walls++;
				break;
				
				case 'p':
					m(y, x) = Cell::pacman_start;
					this->pacman_start_x = x;
					this->pacman_start_y = y;
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
	if (this->map != nullptr)
		delete this->map;
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

	SDL_Init( SDL_INIT_EVERYTHING );

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	this->screen_width_px = 600;
	this->screen_height_px = 600;

	this->sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->screen_width_px, this->screen_height_px, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	this->sdl_gl_context = SDL_GL_CreateContext(this->sdl_window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0, 0, this->screen_width_px, this->screen_height_px);

	this->opengl_circle_factory_low_def = new Opengl::Circle_factory(Config::opengl_low_def_triangles);
	//this->opengl_circle_factory_high_def = new Opengl::Circle_factory(Config::opengl_high_def_triangles);

	dprint( "loaded opengl stuff" << std::endl )

	dprint( "chorono resolution " << (static_cast<float>(std::chrono::high_resolution_clock::period::num) / static_cast<float>(std::chrono::high_resolution_clock::period::den)) << std::endl );

	this->load_opengl_programs();

	this->world = nullptr;
	this->world = new World();

	dprint( "loaded world" << std::endl )

	this->alive = true;
}

void Game::Main::load_opengl_programs ()
{
	this->opengl_program_triangle = new Opengl::Program_triangle;

	dprint( "loaded opengl triangle program" << std::endl )

	this->opengl_program_triangle->use_program();
	
	this->opengl_program_triangle->bind_vertex_array();
	this->opengl_program_triangle->bind_vertex_buffer();

	this->opengl_program_triangle->setup_vertex_array();

	dprint( "generated and binded opengl world vertex array/buffer" << std::endl )
}

void Game::Main::run ()
{
	SDL_Event event;
	const Uint8 *keys;
	std::chrono::steady_clock::time_point tbegin, tend;
	float real_dt, virtual_dt;
	const float target_fps = 60.0f;
	const float target_dt = 1.0f / target_fps;

	this->state = State::playing;

	keys = SDL_GetKeyboardState(nullptr);

	real_dt = 0.0f;
	virtual_dt = 0.0f;

	while (this->alive) {
		tbegin = std::chrono::steady_clock::now();

		virtual_dt = (real_dt > target_dt) ? target_dt : real_dt;

		dprint( "start new frame render real_dt=" << real_dt << " virtual_dt=" << virtual_dt << std::endl )

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

		glClear( GL_COLOR_BUFFER_BIT );

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

		SDL_GL_SwapWindow(this->sdl_window);

		do {
			tend = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsed_ = std::chrono::duration_cast<std::chrono::duration<float>>(tend - tbegin);
			real_dt = elapsed_.count();
		} while (real_dt < target_dt);
	}
}

void Game::Main::cleanup ()
{
	SDL_GL_DeleteContext(this->sdl_gl_context);
	SDL_DestroyWindow(this->sdl_window);
	SDL_Quit();
}

Game::World::World ()
{
	this->w = static_cast<float>( this->map.get_w() );
	this->h = static_cast<float>( this->map.get_h() );

	this->projection_matrix.setup( Opengl::Projection_matrix::Args{
		.left = 0.0f,
		.right = this->w,
		.top = 0.0f,
		.bottom = this->h,
		.znear = 0.0f,
		.zfar = 100.0f
		} );

	Main::get()->get_opengl_program_triangle()->upload_projection_matrix(this->projection_matrix);

	this->player = new Player;
	this->add_object(player);

	this->player->set_x( static_cast<float>( this->map.get_pacman_start_x() ) + 0.5f );
	this->player->set_y( static_cast<float>( this->map.get_pacman_start_y() ) + 0.5f );
}

Game::World::~World ()
{
	delete this->player;
}

void Game::World::event_keydown (SDL_Keycode key)
{
	switch (key) {
		case SDLK_LEFT:
			this->player->set_vx(-Config::pacman_speed);
			this->player->set_vy(0.0f);
		break;

		case SDLK_RIGHT:
			this->player->set_vx(Config::pacman_speed);
			this->player->set_vy(0.0f);
		break;

		case SDLK_UP:
			this->player->set_vx(0.0f);
			this->player->set_vy(-Config::pacman_speed);
		break;

		case SDLK_DOWN:
			this->player->set_vx(0.0f);
			this->player->set_vy(Config::pacman_speed);
		break;
	}
}

void Game::World::physics (float dt, const Uint8 *keys)
{
	for (Object *obj: this->objects) {
		obj->physics(dt);
	}
}

void Game::World::render_map ()
{
	Opengl::Program_triangle::Vertex *vertices;
	Opengl::Program_triangle *program = Main::get()->get_opengl_program_triangle();

	Shape_rect rect(Config::map_tile_size, Config::map_tile_size);
	const uint32_t n_rects = this->map.get_n_walls();
	const uint32_t total_n_vertices = rect.fast_get_n_vertices() * n_rects;

	dprint( "map allocating space for " << total_n_vertices << " vertices in vertex_buffer" << std::endl )

	vertices = program->alloc_vertices(total_n_vertices);
	Opengl::Program_triangle::Vertex *rect_vertices = vertices;

	for (uint32_t y=0; y<this->map.get_h(); y++) {
		for (uint32_t x=0; x<this->map.get_w(); x++) {
			switch (this->map(y, x)) {
				case Map::Cell::wall:
					rect.set_dx( static_cast<float>(x) + 0.5f );
					rect.set_dy( static_cast<float>(y) + 0.5f );
					rect.push_vertices( &(rect_vertices->x), &(rect_vertices->y), program->get_stride() );
					rect_vertices += rect.fast_get_n_vertices();
				break;
			}
		}
	}

	for (uint32_t i=0; i<total_n_vertices; i++) {
		vertices[i].offset_x = 0.0f;
		vertices[i].offset_y = 0.0f;
		vertices[i].r = 0.0f;
		vertices[i].g = 0.0f;
		vertices[i].b = 1.0f;
		vertices[i].a = 1.0f;

	#if 0
		dprint( "map vertex[" << i << "] x = " << vertices[i].x << "  y =" << vertices[i].y
		        << " offset_x = " << vertices[i].offset_x
				<< " offset_y = " << vertices[i].offset_y << std::endl )
	#endif
	}
}

void Game::World::render (float dt)
{
	Main::get()->get_opengl_program_triangle()->clear();

	this->render_map();

	for (Object *obj: this->objects) {
		obj->render(dt);
	}

	//this->Programriangle->debug(); exit(1);

	Main::get()->get_opengl_program_triangle()->upload_vertex_buffer();
	Main::get()->get_opengl_program_triangle()->draw();
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