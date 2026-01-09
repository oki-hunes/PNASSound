# Simple Makefile for 40Hz Auditory Stimulation Generator
# For macOS with SDL2 installed via Homebrew

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs)

TARGET = pnas_sound
SRC = main.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -o $@ $< $(SDL2_LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Install SDL2 on macOS (requires Homebrew)
install-deps:
	brew install sdl2
