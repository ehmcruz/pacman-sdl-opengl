#include <iostream>
#include <string>
#include <utility>
#include <string_view>
#include <algorithm>
#include <ctype.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <my-game-lib/my-game-lib.h>

#include "game-world.h"
#include "lib.h"
#include "config.h"
#include "debug.h"

// initialize with default values
static Game::Main::InitConfig cfg = {
	.graphics_type = MyGlib::Graphics::Manager::Type::SDL,
	.window_width_px = Game::Config::default_window_width_px,
	.window_height_px = Game::Config::default_window_height_px,
	.fullscreen = false,
	.zoom = Game::Config::default_zoom,
};

static bool str_i_equals (const std::string_view& a, const std::string_view& b)
{
	/*return std::equal(a.begin(), a.end(), b.begin(), b.end(),
		[](char a, char b) {
		return tolower(a) == tolower(b);
	});*/
	return boost::iequals(a, b);
}

static void process_args (int argc, char **argv)
{
	boost::program_options::options_description cmd_line_args("Pacman -- Options");
	boost::program_options::variables_map vm;
	std::string renderer_type_strs = "Renderer type. Available renderers: ";
	const uint32_t n_types = std::to_underlying(MyGlib::Graphics::Manager::Type::Unsupported);

	for (uint32_t i = 0; i < n_types; i++) {
		renderer_type_strs += MyGlib::Graphics::Manager::get_type_str( static_cast<MyGlib::Graphics::Manager::Type>(i) );

		if (i < (n_types-1))
			renderer_type_strs += ", ";
	}

	try {
		cmd_line_args.add_options()
			( "help,h", "Help screen" )
			( "video,v",
				boost::program_options::value<std::string>()->default_value( MyGlib::Graphics::Manager::get_type_str(cfg.graphics_type) ),
				renderer_type_strs.c_str() )
			( "width",
				boost::program_options::value<uint32_t>()->default_value(cfg.window_width_px),
				"Width of the window" )
			( "height",
				boost::program_options::value<uint32_t>()->default_value(cfg.window_height_px),
				"Height of the window" )
			( "zoom",
				boost::program_options::value<float>()->default_value(cfg.zoom),
				"Zoom level (should be equal or greater than 1.0)" )
			;

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmd_line_args), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			using namespace Game;
			dprintln(cmd_line_args);
			std::exit(EXIT_FAILURE);
		}

		if (vm.count("video")) {
			bool valid_type = false;

			for (uint32_t i = 0; i < n_types; i++) {
				const auto type = static_cast<MyGlib::Graphics::Manager::Type>(i);
				const char *s = MyGlib::Graphics::Manager::get_type_str(type);
	
				if ( str_i_equals(vm["video"].as<std::string>(), std::string_view(s)) ) {
					valid_type = true;
					cfg.graphics_type = type;
					break;
				}
			}

			if (!valid_type)
				throw std::runtime_error("Bad video driver!");
		}

		if (vm.count("width")) {
			cfg.window_width_px = vm["width"].as<uint32_t>();
		}

		if (vm.count("height")) {
			cfg.window_height_px = vm["height"].as<uint32_t>();
		}

		if (vm.count("zoom")) {
			cfg.zoom = vm["zoom"].as<float>();

			if (cfg.zoom < 1.0f)
				throw std::runtime_error("The zoom must be at least 1.0");
		}
	}
	catch (const boost::program_options::error& ex) {
		throw std::runtime_error(ex.what());
	}
}

int main (const int argc, char **argv)
{
	using namespace Game;

	try {
		process_args(argc, argv);

		dprintln("Setting video renderer to ", MyGlib::Graphics::Manager::get_type_str(cfg.graphics_type));

		SDL_Init( SDL_INIT_VIDEO );

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