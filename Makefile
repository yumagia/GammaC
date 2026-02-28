CC = g++ -fsanitize=address -lglfw -lGL -lGLU -lGLEW -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -fopenmp
# CC = g++ -fsanitize=address 
CXXFLAGS = -std=c++17 -O3 -ggdb
LDFLAGS = $(shell pkg-config sdl2 --cflags --libs)

COMMON_DIR := src/common
BUILD_DIR := build

# GAMMA Program
GAMMA_EXEC := GAMMA
GAMMA_SRC_DIRS := src/gamma $(COMMON_DIR)

GAMMA_SRCS := $(shell find $(GAMMA_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GAMMA_OBJS := $(GAMMA_SRCS:%=$(BUILD_DIR)/%.o)
GAMMA_DEPS := $(GAMMA_OBJS:.o=.d)

GAMMA_INC_DIRS := $(shell find $(GAMMA_SRC_DIRS) -type d)
GAMMA_INC_FLAGS := $(addprefix -I,$(GAMMA_INC_DIRS))

# GBSP Program
GBSP_EXEC := GBSP
GBSP_SRC_DIRS := src/grak/gbsp $(COMMON_DIR)

GBSP_SRCS := $(shell find $(GBSP_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GBSP_OBJS := $(GBSP_SRCS:%=$(BUILD_DIR)/%.o)

GBSP_INC_DIRS := $(shell find $(GBSP_SRC_DIRS) -type d)
GBSP_INC_FLAGS := $(addprefix -I,$(GBSP_INC_DIRS))

# GRAD Program
GRAD_EXEC := GRAD
GRAD_SRC_DIRS := src/grak/grad $(COMMON_DIR)

GRAD_SRCS := $(shell find $(GRAD_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GRAD_OBJS := $(GRAD_SRCS:%=$(BUILD_DIR)/%.o)

GRAD_INC_DIRS := $(shell find $(GRAD_SRC_DIRS) -type d)
GRAD_INC_FLAGS := $(addprefix -I,$(GRAD_INC_DIRS))

# Misc Programs
VBO_GEN_EXEC := VBOGen
VBO_GEN_SRC_DIRS := src/grak/misc/vbogen $(COMMON_DIR)

VBO_GEN_SRCS := $(shell find $(VBO_GEN_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
VBO_GEN_OBJS := $(VBO_GEN_SRCS:%=$(BUILD_DIR)/%.o)

VBO_GEN_INC_DIRS := $(shell find $(VBO_GEN_SRC_DIRS) -type d)
VBO_GEN_INC_FLAGS := $(addprefix -I,$(VBO_GEN_INC_DIRS))

CPPFLAGS := $(GAMMA_INC_FLAGS) $(GBSP_INC_FLAGS) $(GRAD_INC_FLAGS) $(VBO_GEN_INC_FLAGS) -MMD -MP

all: $(GAMMA_EXEC) $(GBSP_EXEC) $(GRAD_EXEC)

tools: $(GBSP_EXEC) $(GRAD_EXEC)

misc: $(VBO_GEN_EXEC)

# GAMMA Executable
$(GAMMA_EXEC): $(GAMMA_OBJS)
	$(CC) $(GAMMA_OBJS) -o $@ $(LDFLAGS)

# GBSP Executable
$(GBSP_EXEC): $(GBSP_OBJS)
	$(CC) $(GBSP_OBJS) -o $@ $(LDFLAGS)

# GRAD Executable
$(GRAD_EXEC): $(GRAD_OBJS)
	$(CC) $(GRAD_OBJS) -o $@ $(LDFLAGS)

# Misc Executables
$(VBO_GEN_EXEC): $(VBO_GEN_OBJS)
	$(CC) $(VBO_GEN_OBJS) -o $@ $(LDFLAGS)

# Build
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean all

clean:
	$(RM) -r $(BUILD_DIR) $(GAMMA_DEPS)

MKDIR_P := mkdir -p

-include $(GAMMA_DEPS)