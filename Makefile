CXX = g++
CXXFLAGS = -Wall -Wno-deprecated-declarations -std=c++17 -Isrc/include

TARGET = bin/main.exe
SOURCES = src/Main.cpp src/ImageIO.cpp src/Quadtree.cpp src/ImageCompressor.cpp src/ErrorMeasurement.cpp src/SaveGif.cpp

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: all
ifeq ($(OS),Windows_NT)
	cls && $(TARGET)
else
	clear && ./$(TARGET)
endif

clean:
	rm -f $(TARGET)