cmake_minimum_required(VERSION 3.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(pacman_sdl_opengl VERSION 1.0)

# -------------------------------------

set(MYLIB_PATH ${CMAKE_SOURCE_DIR}/../my-lib)

# -------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

# -------------------------------------

include_directories(
	"${SDL2_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIRS}"
	"${MYLIB_PATH}/include"
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/graphics"
	)

add_subdirectory(src)

# -------------------------------------

#add_executable(pacman)

#target_link_libraries(pacman PUBLIC pacman_lib)
#target_link_libraries(pacman ${SDL2_LIBRARIES})
