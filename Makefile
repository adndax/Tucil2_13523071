CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -Isrc/include

SRC_DIR = src
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SOURCES))
TARGET = $(BIN_DIR)/main

all: $(TARGET)
	@echo "Build complete."
	@echo "Run with: ./$(TARGET)"

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lfreeimage

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)/*.o $(TARGET)

.PHONY: all clean