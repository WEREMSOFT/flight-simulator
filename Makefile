SRC := $(shell find src -name *.cpp)
SRC_C := $(shell find libs -name *.c) $(shell find src -name *.c) 
SRC_H := $(shell find src -name *.hpp) $(shell find src -name *.h)
OBJ := $(patsubst %.cpp,%.opp,$(SRC))
OBJ_C := $(patsubst %.c,%.o,$(SRC_C))
CC := clang
INCLUDES := -Ilibs/include -Ilibs/GLAD/include
FLAGS := -g3 -O0 -std=c++17 -Wall -Wno-missing-braces -Wno-unknown-warning-option $(INCLUDES)
FLAGS_RELEASE := -g0 -O4 -std=c++17 -Wall -Wno-missing-braces -Wno-unknown-warning-option $(INCLUDES)
FLAGS_C := -g3 -O0 -Wall -Wno-missing-braces -Wno-unknown-warning-option $(INCLUDES)
FLAGS_C_RELEASE := -g0 -O4 -Wno-missing-braces $(INCLUDES)
LIBS := -lstdc++ -lm -lglfw -ldl -lGL
TARGET := bin/main.bin


all: $(OBJ) $(OBJ_C) copy_assets $(SRC_H)
#@echo $(OBJ) $(OBJ_C) $(SRC_C) $(SRC_H)
	@$(CC) $(FLAGS) $(OBJ) $(OBJ_C) -o $(TARGET) $(LIBS)

%.cpp: %.hpp
	@echo "$@ updated"
	@touch $@

%.c: %.h
	@echo "$@ updated"
	@touch $@

%.opp: %.cpp
	$(CC) $(FLAGS) -c $^ -o $@

%.o: %.c
	$(CC) $(FLAGS_C) -c $^ -o $@


run_main: all
	$(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJ)
	rm -rf $(OBJ_C)

copy_assets:
	@cp -r assets bin
