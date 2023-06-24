CPP = g++
BIN = pacman.exe

# get my-lib:
# https://github.com/ehmcruz/my-lib
MYLIB = ../my-lib

# Windows
CPPFLAGS = -std=c++23 `sdl2-config --cflags` -I$(MYLIB)/include -g -mconsole #-O2
LDFLAGS = -std=c++23 `sdl2-config --libs` -mconsole -lglew32 -lopengl32 -lm

# Linux
#CPPFLAGS = -std=c++20 `sdl2-config --cflags` -I$(MYLIB)/include -g #-O2
#LDFLAGS = -std=c++20 `sdl2-config --libs` -lGL -lGLEW -lm

# ----------------------------------

OBJS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))

HEADERS = $(wildcard src/*.h) $(wildcard $(MYLIB)/include/my-lib/*.h)

# ----------------------------------

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(BIN): $(OBJS)
	$(CPP) -o $(BIN) $(OBJS) $(LDFLAGS)

all: $(BIN)
	@echo "Everything compiled! yes!"

clean:
	- rm -rf $(BIN) $(OBJS)