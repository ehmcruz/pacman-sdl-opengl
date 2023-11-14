#include <iostream>
#include <string>
#include <utility>
#include <string_view>
#include <algorithm>
#include <ctype.h>

#include <SDL.h>

#include "game-world.h"
#include "graphics.h"
#include "lib.h"
#include "config.h"
#include "debug.h"

// initialize with default values
static Game::Main::InitConfig cfg = {
	.renderer_type = Graphics::Renderer::Type::SDL,
	.window_width_px = 0,
	.window_height_px = 0,
	.fullscreen = true,
	.zoom = Game::Config::default_zoom,
};

static int cpp_main (int argc, char **argv)
{
	using namespace Game;
	
	try {
		SDL_Init( SDL_INIT_VIDEO );

		dprintln("Setting video renderer to ", Graphics::Renderer::get_type_str(cfg.renderer_type));

		Game::Main::allocate();

		Game::Main::get()->load(cfg);
		Game::Main::get()->run();
		Game::Main::get()->cleanup();

		Game::Main::deallocate();
	}
	catch (const std::exception& e) {
		dprintln("Something bad happened!", '\n', e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

extern "C"
{
	int main (int argc, char **argv)
	{
		return cpp_main(argc, argv);
	}
}