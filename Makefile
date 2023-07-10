CPP = g++

BIN_LINUX = pacman
BIN_WINDOWS = pacman.exe

# get my-lib:
# https://github.com/ehmcruz/my-lib
MYLIB = ../my-lib

# Windows
CPPFLAGS_WINDOWS = -std=c++23 `sdl2-config --cflags` -I$(MYLIB)/include -g -mconsole #-O2
LDFLAGS_WINDOWS = -std=c++23 `sdl2-config --libs` -mconsole -lglew32 -lopengl32 -lm

# Linux
CPPFLAGS_LINUX = -std=c++23 `sdl2-config --cflags` -I$(MYLIB)/include -g #-O2
LDFLAGS_LINUX = -std=c++23 `sdl2-config --libs` -lGL -lGLEW -lm

# ----------------------------------

ifdef CONFIG_TARGET_LINUX
	BIN=$(BIN_LINUX)
	CPPFLAGS=$(CPPFLAGS_LINUX)
	LDFLAGS=$(LDFLAGS_LINUX)
endif

ifdef CONFIG_TARGET_WINDOWS
	BIN=$(BIN_WINDOWS)
	CPPFLAGS=$(CPPFLAGS_WINDOWS)
	LDFLAGS=$(LDFLAGS_WINDOWS)
endif

# ----------------------------------

MYLIB_SRCS = ext/math-matrix.cpp

ext/math-matrix.cpp: $(MYLIB)/math-matrix.cpp
	cp $(MYLIB)/math-matrix.cpp ext/math-matrix.cpp

copy_my_lib_srcs:
	mkdir -p ext
	cp $(MYLIB)/math-matrix.cpp ext/math-matrix.cpp

# ----------------------------------

SRCS := $(wildcard src/*.cpp) $(MYLIB_SRCS) src/graphics/sdl.cpp

OBJS := $(patsubst %.cpp,%.o,$(SRCS))

HEADERS = $(wildcard src/*.h) $(wildcard src/graphics/*.h) $(wildcard $(MYLIB)/include/my-lib/*.h)

# ----------------------------------

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(BIN): $(OBJS)
	$(CPP) -o $(BIN) $(OBJS) $(LDFLAGS)

all: $(BIN)
	@echo "Everything compiled! yes!"

clean:
	- rm -rf $(BIN) $(OBJS)