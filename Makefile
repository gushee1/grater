#Makefile for game engine created with the help of UMGPT

CXX = clang++
CXXFLAGS = -std=c++17 -O3
LDFLAGS = -Llib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2main -lSDL2_ttf -llua5.4
SRCDIR = ./game_engine/src
BOX2D_SRC_DIR = ./game_engine/third-party/box2d-2.4.1/src
INCLUDES = -I./game_engine/third-party/glm-0.9.9.8/glm \
		   -I./game_engine/third-party/glm-0.9.9.8 \
		   -I./game_engine/third-party/rapidjson-1.1.0 \
		   -I./game_engine/third-party/SDL2-2.28.5-VC/include \
		   -I./game_engine/third-party/SDL2_image-2.6.3-VC/include \
		   -I./game_engine/third-party/SDL2_mixer-2.6.3-VC/include \
		   -I./game_engine/third-party/SDL2_ttf-2.20.2-VC/include \
		   -I./game_engine/third-party/lua-5.4.6 \
		   -I./game_engine/third-party/LuaBridge-2.8 \
		   -I./game_engine/third-party/box2d-2.4.1/include \
		   -I./game_engine/third-party/box2d-2.4.1/src \
		   -I./game_engine/third-party
OUTPUT = game_engine_linux

BOX2D_COLLISION_SRCS = $(wildcard $(BOX2D_SRC_DIR)/collision/*.cpp)
BOX2D_DYNAMICS_SRCS = $(wildcard $(BOX2D_SRC_DIR)/dynamics/*.cpp)
BOX2D_COMMON_SRCS = $(wildcard $(BOX2D_SRC_DIR)/common/*.cpp)
BOX2D_ROPE_SRCS = $(wildcard $(BOX2D_SRC_DIR)/rope/*.cpp)
BOX2D_SRCS = $(BOX2D_COLLISION_SRCS) $(BOX2D_DYNAMICS_SRCS) $(BOX2D_COMMON_SRCS) $(BOX2D_ROPE_SRCS)

ENGINE_SRCS = $(wildcard $(SRCDIR)/*.cpp)

SRCS = $(BOX2D_SRCS) $(ENGINE_SRCS)
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTPUT)
