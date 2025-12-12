CC = g++ -fsanitize=address
CXXFLAGS = -std=c++17 -O3 -ggdb
LDFLAGS = $(shell pkg-config sdl2 --cflags --libs)

COMMON_DIR := src/common

# GAMMA Program
GAMMA_EXEC := GAMMA
GAMMA_BUILD_DIR := gamma-build
GAMMA_SRC_DIRS := src/gamma $(COMMON_DIR)

GAMMA_SRCS := $(shell find $(GAMMA_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GAMMA_OBJS := $(GAMMA_SRCS:%=$(GAMMA_BUILD_DIR)/%.o)

GAMMA_INC_DIRS := $(shell find $(GAMMA_SRC_DIRS) -type d)
GAMMA_INC_FLAGS := $(addprefix -I,$(GAMMA_INC_DIRS))

GAMMA_CPPFLAGS := $(GAMMA_INC_FLAGS) -MMD -MP

# GBSP Program
GBSP_EXEC := GBSP
GBSP_BUILD_DIR := gbsp-build
GBSP_SRC_DIRS := src/grak/gbsp $(COMMON_DIR)

GBSP_SRCS := $(shell find $(GBSP_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GBSP_OBJS := $(GBSP_SRCS:%=$(GBSP_BUILD_DIR)/%.o)

GBSP_INC_DIRS := $(shell find $(GBSP_SRC_DIRS) -type d)
GBSP_INC_FLAGS := $(addprefix -I,$(GBSP_INC_DIRS))

GBSP_CPPFLAGS := $(GBSP_INC_FLAGS) -MMD -MP

# GRAD Program
GRAD_EXEC := GRAD
GRAD_BUILD_DIR := grad-build
GRAD_SRC_DIRS := src/grak/grad $(COMMON_DIR)

GRAD_SRCS := $(shell find $(GRAD_SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
GRAD_OBJS := $(GRAD_SRCS:%=$(GRAD_BUILD_DIR)/%.o)

GRAD_INC_DIRS := $(shell find $(GRAD_SRC_DIRS) -type d)
GRAD_INC_FLAGS := $(addprefix -I,$(GRAD_INC_DIRS))

GRAD_CPPFLAGS := $(GRAD_INC_FLAGS) -MMD -MP

all: $(GAMMA_EXEC) $(GBSP_EXEC) $(GRAD_EXEC)

# GAMMA Executable
$(GAMMA_EXEC): $(GAMMA_OBJS)
	$(CC) $(GAMMA_OBJS) -o $@ $(LDFLAGS)

$(GAMMA_BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(GAMMA_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(GAMMA_BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(GAMMA_CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# GBSP Executable
$(GBSP_EXEC): $(GBSP_OBJS)
	$(CC) $(GBSP_OBJS) -o $@ $(LDFLAGS)

$(GBSP_BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(GBSP_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(GBSP_BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(GBSP_CPPFLAGS) $(CXXFLAGS) -c $< -o $@


# GRAD Executable
$(GRAD_EXEC): $(GRAD_OBJS)
	$(CC) $(GRAD_OBJS) -o $@ $(LDFLAGS)

$(GRAD_BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(GRAD_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(GRAD_BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(GRAD_CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean all

clean:
	$(RM) -r $(GBSP_BUILD_DIR) $(GRAD_BUILD_DIR)

MKDIR_P := mkdir -p