.PHONY: clean

SRC_DIR = src
EXE_NAME = GRAK
CFLAGS = -O2 -ggdb

LDFLAGS = -lglfw

build: $(SRC_DIR)/main.cpp $(SRC_DIR)/render/glad/glad.c $(SRC_DIR)/render/Application.cpp $(SRC_DIR)/mapping/Bsp.cpp $(SRC_DIR)/mapping/MeshLoader.cpp $(SRC_DIR)/Math.cpp
	g++ $(CFLAGS) -o $(EXE_NAME) $(SRC_DIR)/main.cpp $(SRC_DIR)/render/glad/glad.c $(SRC_DIR)/render/Application.cpp $(SRC_DIR)/mapping/Bsp.cpp $(SRC_DIR)/mapping/MeshLoader.cpp $(SRC_DIR)/Math.cpp -I$(SRC_DIR) $(LDFLAGS)

clean:
	-rm $(EXE_NAME)