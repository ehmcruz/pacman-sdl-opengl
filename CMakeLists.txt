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

# To build without OpenGL support:
# cmake .. -DSUPPORT_OPENGL=OFF
option(SUPPORT_OPENGL "Include support for OpenGL" ON)

# -------------------------------------

#set(TARGET_PLATFORM "UNKNOWN")

#if(WIN32)
#	set(TARGET_PLATFORM "WINDOWS")
#endif()

#if(UNIX AND NOT APPLE)
	# for Linux, BSD, Solaris, Minix
#	set(TARGET_PLATFORM "LINUX")
#endif()

#message(STATUS "Target platform: ${TARGET_PLATFORM}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# -------------------------------------

set(MYLIB_PATH ${CMAKE_SOURCE_DIR}/../my-lib)
set(MYGAMELIB_PATH ${CMAKE_SOURCE_DIR}/../my-game-lib)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	find_package(SDL2 REQUIRED)
	find_package(Boost COMPONENTS program_options REQUIRED)

	# SDL2 mixer
	set(SDL2_MIXER_LINK_FLAGS "-lSDL2_mixer")
	set(SDL2_MIXER_LIBRARIES "${SDL2_MIXER_LINK_FLAGS}")

	# SDL2 image
	set(SDL2_IMAGE_LINK_FLAGS "-lSDL2_image")
	set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_LINK_FLAGS}")
endif()

if (MSVC)
	set(WINDOWS_SDL_DEV_LIBS "C:\\my-msvc-libs\\SDL2-2.28.5")

	list(APPEND CMAKE_PREFIX_PATH "${WINDOWS_SDL_DEV_LIBS}\\cmake")
	list(APPEND CMAKE_PREFIX_PATH "C:\\my-msvc-libs\\boost_1_83_0")
	#list(APPEND CMAKE_PREFIX_PATH "C:\\my-msvc-libs\\boost_1_83_0\\lib64-msvc-14.3\\cmake\\boost_program_options-1.83.0")

	set(my_Boost_LIBRARIES "C:\\my-msvc-libs\\boost_1_83_0\\lib64-msvc-14.3\\libboost_program_options-vc143-mt-gd-x64-1_83.lib")

	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(Boost COMPONENTS program_options REQUIRED)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
	set(SDL2_INCLUDE_DIRS /home/ehmcruz/Android/SDL/include)
	set(SDL2_LINK_DIR /home/ehmcruz/Android/SDL/lib/arm64-v8a)

	add_library(SDL2 SHARED IMPORTED)
	set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION ${SDL2_LINK_DIR}/libSDL2.so)

	add_library(SDL2_MIXER SHARED IMPORTED)
	set_target_properties(SDL2_MIXER PROPERTIES IMPORTED_LOCATION ${SDL2_LINK_DIR}/libSDL2_mixer.so)
endif()

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	if (MSVC)
		set(my_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Debug)
	else()
		set(my_OUTPUT_DIR ${CMAKE_BINARY_DIR})
	endif()
endif()

#message(STATUS "aki ${Boost_LIBRARIES}")

if (SUPPORT_OPENGL)
	find_package(OpenGL REQUIRED)

	if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
		find_package(GLEW REQUIRED)
	endif()
endif()

# -------------------------------------

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	file(COPY ${MYGAMELIB_PATH}/shaders DESTINATION ${my_OUTPUT_DIR})

	if (MSVC)
		file(COPY "${WINDOWS_SDL_DEV_LIBS}\\lib\\x64\\SDL2.dll" DESTINATION ${my_OUTPUT_DIR})
	endif()
endif()

# -------------------------------------

#include(GNUInstallDirs)

# https://stackoverflow.com/questions/56397962/cmake-gtest-fatal-error-missing-stdlib-h-when-cross-compiling
# It is known that using -isystem for standard compiler include directories breaks #include_next directive which is widely used for standard C++ header files.
# See e.g. this question: -isystem on a system include directory causes errors.

include_directories(
	"${SDL2_INCLUDE_DIRS}"
	"${MYLIB_PATH}/include"
	"${MYGAMELIB_PATH}/include"
	"${CMAKE_SOURCE_DIR}/src"
	)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	include_directories("${Boost_INCLUDE_DIRS}")
endif()

set(MYGAMELIB_SOURCE_FILES
	"${MYGAMELIB_PATH}/src/events.cpp"
	"${MYGAMELIB_PATH}/src/graphics.cpp"
	"${MYGAMELIB_PATH}/src/my-game-lib.cpp"
	"${MYGAMELIB_PATH}/src/texture-atlas.cpp"
	"${MYGAMELIB_PATH}/src/sdl/sdl.cpp"
	"${MYGAMELIB_PATH}/src/sdl/sdl-audio.cpp"
	"${MYGAMELIB_PATH}/src/sdl/sdl-graphics.cpp"
)

if (SUPPORT_OPENGL)
	include_directories(
		"${OPENGL_INCLUDE_DIR}"
		)

	if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
		include_directories(
			"${GLEW_INCLUDE_DIRS}"
			)
	endif()

	set(MYGAMELIB_SOURCE_FILES ${MYGAMELIB_SOURCE_FILES}
		"${MYGAMELIB_PATH}/src/opengl/program.cpp"
		"${MYGAMELIB_PATH}/src/opengl/renderer.cpp"
		)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
	set(MYGAMELIB_SOURCE_FILES ${MYGAMELIB_SOURCE_FILES}
		"${MYGAMELIB_PATH}/src/opengl/opengl.cpp")
endif()

# -------------------------------------

add_compile_definitions(MYGLIB_FP_TYPE=float)
add_compile_definitions(MYGLIB_SUPPORT_SDL=1)

if (SUPPORT_OPENGL)
	add_compile_definitions(MYGLIB_SUPPORT_OPENGL=1)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
	add_compile_definitions(MYGLIB_SUPPORT_OPENGL=1)
endif()

# -------------------------------------

add_subdirectory(src)

# -------------------------------------

#add_executable(pacman)

#target_link_libraries(pacman PUBLIC pacman_lib)
#target_link_libraries(pacman ${SDL2_LIBRARIES})
