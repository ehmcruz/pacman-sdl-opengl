cmake_minimum_required(VERSION 3.8)

# cmake --build .
# make VERBOSE=1

project(pacman_sdl_opengl VERSION 1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_VERBOSE ON)
#set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

# -------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# -------------------------------------

option(SUPPORT_OPENGL "Include support for OpenGL" OFF)

# -------------------------------------

set(MYLIB_PATH ${CMAKE_SOURCE_DIR}/../my-lib)

if (MSVC)
	set(WINDOWS_SDL_DEV_LIBS "C:\\my-msvc-libs\\SDL2-2.28.5")

	list(APPEND CMAKE_PREFIX_PATH "${WINDOWS_SDL_DEV_LIBS}\\cmake")
	list(APPEND CMAKE_PREFIX_PATH "C:\\my-msvc-libs\\boost_1_83_0")
	#list(APPEND CMAKE_PREFIX_PATH "C:\\my-msvc-libs\\boost_1_83_0\\lib64-msvc-14.3\\cmake\\boost_program_options-1.83.0")

	set(my_Boost_LIBRARIES "C:\\my-msvc-libs\\boost_1_83_0\\lib64-msvc-14.3\\libboost_program_options-vc143-mt-gd-x64-1_83.lib")
endif()

find_package(SDL2 REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
	set(my_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Debug)
else()
	set(my_OUTPUT_DIR ${CMAKE_BINARY_DIR})
endif()

#message(STATUS "aki ${Boost_LIBRARIES}")

if (SUPPORT_OPENGL)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)

	add_compile_definitions(PACMAN_SUPPORT_OPENGL=1)
endif()

# -------------------------------------

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${my_OUTPUT_DIR})

if (MSVC)
	file(COPY "${WINDOWS_SDL_DEV_LIBS}\\lib\\x64\\SDL2.dll" DESTINATION ${my_OUTPUT_DIR})
endif()

# -------------------------------------

#include(GNUInstallDirs)

# https://stackoverflow.com/questions/56397962/cmake-gtest-fatal-error-missing-stdlib-h-when-cross-compiling
# It is known that using -isystem for standard compiler include directories breaks #include_next directive which is widely used for standard C++ header files.
# See e.g. this question: -isystem on a system include directory causes errors.

include_directories(
	"${SDL2_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIRS}"
	"${MYLIB_PATH}/include"
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/graphics"
	)

if (SUPPORT_OPENGL)
	include_directories(
		"${OPENGL_INCLUDE_DIR}"
		"${GLEW_INCLUDE_DIRS}"
		)
endif()

add_subdirectory(src)

# -------------------------------------

#add_executable(pacman)

#target_link_libraries(pacman PUBLIC pacman_lib)
#target_link_libraries(pacman ${SDL2_LIBRARIES})
