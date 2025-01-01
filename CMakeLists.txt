CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2
INCLUDES = -Iinclude
LIBS = -L. -loutline -lboost_system -lboost_json -lboost_url -lssl -lcrypto -lpthread

SRC_DIR = src
OBJ_DIR = obj

CLIENT_SRC = $(shell find $(SRC_DIR) -name "*.cpp")

CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CLIENT_SRC))

all: liboutline.a example

liboutline.a: $(CLIENT_OBJ)
	ar rcs liboutline.a $(CLIENT_OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

example: examples/basic/main.cpp liboutline.a
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o example examples/basic/main.cpp liboutline.a $(LIBS)

run: example
	./example

clean:
	rm -rf liboutline.a example obj

.PHONY: all clean run
