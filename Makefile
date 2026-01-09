# Simple Makefile for 40Hz Auditory Stimulation Generator
# For macOS with SDL2 installed via Homebrew

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs)

# Static linking libraries for macOS
SDL2_STATIC_LIB = /usr/local/opt/sdl2/lib/libSDL2.a
SDL2_STATIC_MAIN = /usr/local/opt/sdl2/lib/libSDL2main.a
MACOS_FRAMEWORKS = -framework CoreAudio -framework AudioToolbox -framework CoreFoundation \
                   -framework CoreGraphics -framework CoreVideo -framework Cocoa \
                   -framework IOKit -framework ForceFeedback -framework Carbon \
                   -framework CoreServices -framework ApplicationServices \
                   -framework Metal -framework GameController -framework CoreHaptics \
                   -liconv -weak_framework QuartzCore

TARGET = pnas_sound
TARGET_STATIC = pnas_sound_static
SRC = main.cpp

.PHONY: all clean run static

all: $(TARGET)

# Dynamic linking (requires SDL2 installed)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -o $@ $< $(SDL2_LIBS)

# Static linking (standalone executable)
static: $(TARGET_STATIC)

$(TARGET_STATIC): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -o $@ $< $(SDL2_STATIC_MAIN) $(SDL2_STATIC_LIB) $(MACOS_FRAMEWORKS)

clean:
	rm -f $(TARGET) $(TARGET_STATIC)

run: $(TARGET)
	./$(TARGET)

run-static: $(TARGET_STATIC)
	./$(TARGET_STATIC)

# Install SDL2 on macOS (requires Homebrew)
install-deps:
	brew install sdl2
