set(PACMAN_SOURCE_FILES
	game-object.cpp
	game-world.cpp
	graphics.cpp
	lib.cpp
	main.cpp
	graphics/sdl.cpp
)

set(PACMAN_HEADER_FILES
	config.h
	debug.h
	events.h
	game-object.h
	game-world.h
	graphics.h
	lib.h
	graphics/sdl.h
)

# -------------------------------------

add_executable(pacman ${PACMAN_SOURCE_FILES} ${PACMAN_HEADER_FILES})
target_link_libraries(pacman ${SDL2_LIBRARIES} ${Boost_LIBRARIES})
