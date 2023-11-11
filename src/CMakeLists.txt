set(PACMAN_SOURCE_FILES
	game-object.cpp
	game-world.cpp
	graphics.cpp
	lib.cpp
	main.cpp
	graphics/sdl.cpp
)

if (SUPPORT_OPENGL)
	set(PACMAN_SOURCE_FILES ${PACMAN_SOURCE_FILES}
		graphics/opengl.cpp)
endif()

# -------------------------------------

#add_executable(pacman ${PACMAN_SOURCE_FILES} ${PACMAN_HEADER_FILES})
add_executable(pacman ${PACMAN_SOURCE_FILES})

#set_target_properties(
#	pacman PROPERTIES
#	NO_SYSTEM_FROM_IMPORTED true) # remove -isystem from system libs and use -I to include everything

target_link_libraries(pacman ${SDL2_LIBRARIES} ${Boost_LIBRARIES})

if (SUPPORT_OPENGL)
	target_link_libraries(pacman ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif()
