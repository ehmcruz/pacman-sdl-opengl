#include <iostream>
#include <string>
#include <utility>
#include <string_view>
#include <algorithm>
#include <ctype.h>
#include <boost/algorithm/string.hpp>

#include <boost/program_options.hpp>

#include "game-world.h"
#include "graphics.h"
#include "lib.h"

static Graphics::Renderer::Type renderer_type = Graphics::Renderer::Type::SDL; // default

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
	const uint32_t n_types = std::to_underlying(Graphics::Renderer::Type::Unsupported);

	for (uint32_t i = 0; i < n_types; i++) {
		renderer_type_strs += Graphics::Renderer::get_type_str( static_cast<Graphics::Renderer::Type>(i) );

		if (i < (n_types-1))
			renderer_type_strs += ", ";
	}

	try {
		cmd_line_args.add_options()
			( "help,h", "Help screen" )
			( "video,v",
				boost::program_options::value<std::string>()->default_value( Graphics::Renderer::get_type_str(renderer_type) ),
				renderer_type_strs.c_str() )
			;

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmd_line_args), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			std::cout << cmd_line_args << std::endl;
			exit(0);
		}

		if (vm.count("video")) {
			bool valid_type = false;

			for (uint32_t i = 0; i < n_types; i++) {
				const auto type = static_cast<Graphics::Renderer::Type>(i);
				const char *s = Graphics::Renderer::get_type_str(type);
	
				if ( str_i_equals(vm["video"].as<std::string>(), std::string_view(s)) ) {
					valid_type = true;
					renderer_type = type;
					break;
				}
			}

			if (!valid_type)
				throw std::runtime_error("Bad video driver!");
		}
	}
	catch (const boost::program_options::error& ex) {
		std::cout << ex.what() << std::endl;
		exit(1);
	}
}

int main (int argc, char **argv)
{
	try {
		process_args(argc, argv);

		std::cout << "Setting video renderer to " << Graphics::Renderer::get_type_str(renderer_type) << std::endl;

		Game::Main::allocate();

		Game::Main::get()->load(renderer_type);
		Game::Main::get()->run();
		Game::Main::get()->cleanup();

		Game::Main::deallocate();
	}
	catch(std::exception& e) {
		std::cout << "Something bad happened!" << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}