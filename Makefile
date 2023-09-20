CPP = g++

BIN_LINUX = pacman
BIN_WINDOWS = pacman.exe

# get my-lib:
# https://github.com/ehmcruz/my-lib
MYLIB = ../my-lib

# Windows
CPPFLAGS_WINDOWS = -Wall -DCONFIG_TARGET_WINDOWS=1 -std=c++23 `sdl2-config --cflags` -I$(MYLIB)/include -g -mconsole #-O2
LDFLAGS_WINDOWS = -std=c++23 `sdl2-config --libs` -mconsole -lm -lboost_program_options-mt

# Linux
CPPFLAGS_LINUX = -Wall -DCONFIG_TARGET_LINUX=1 -std=c++23 `sdl2-config --cflags` -I$(MYLIB)/include -g #-O2
LDFLAGS_LINUX = -std=c++23 `sdl2-config --libs` -lm -lboost_program_options

# ----------------------------------

ifdef CONFIG_TARGET_LINUX
	BIN=$(BIN_LINUX)
	CPPFLAGS=$(CPPFLAGS_LINUX)
	LDFLAGS=$(LDFLAGS_LINUX)

	ifdef PACMAN_SUPPORT_OPENGL
		CPPFLAGS += -DPACMAN_SUPPORT_OPENGL=1
		LDFLAGS += -lGL -lGLEW
	endif
endif

ifdef CONFIG_TARGET_WINDOWS
	BIN=$(BIN_WINDOWS)
	CPPFLAGS=$(CPPFLAGS_WINDOWS)
	LDFLAGS=$(LDFLAGS_WINDOWS)

	ifdef PACMAN_SUPPORT_OPENGL
		CPPFLAGS += -DPACMAN_SUPPORT_OPENGL=1
		LDFLAGS += -lglew32 -lopengl32
	endif
endif

# ----------------------------------

# need to add a rule for each .o/.cpp at the bottom
MYLIB_OBJS = ext/math.o

SRCS := $(wildcard src/*.cpp) src/graphics/sdl.cpp

ifdef PACMAN_SUPPORT_OPENGL
	SRCS += src/graphics/opengl.cpp
endif

OBJS := $(patsubst %.cpp,%.o,$(SRCS)) $(MYLIB_OBJS)

HEADERS = $(wildcard src/*.h) $(wildcard src/graphics/*.h) $(wildcard $(MYLIB)/include/my-lib/*.h)

# ----------------------------------

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(BIN): $(OBJS)
	$(CPP) -o $(BIN) $(OBJS) $(LDFLAGS)

all: $(BIN)
	@echo "Everything compiled! yes!"

# ----------------------------------

ext/math.o: $(MYLIB)/src/math.cpp $(HEADERS)
	mkdir -p ext
	$(CPP) $(CPPFLAGS) -c -o ext/math.o $(MYLIB)/src/math.cpp

# ----------------------------------

clean:
	- rm -rf $(BIN) $(OBJS)