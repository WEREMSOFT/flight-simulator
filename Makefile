SRC := $(shell find src -name *.cpp)
SRC_C := $(shell find libs -name *.c)
OBJ := $(patsubst %.cpp,%.opp,$(SRC))
OBJ_C := $(patsubst %.c,%.o,$(SRC_C))
CC := clang
INCLUDES := -Ilibs/include -Ilibs/GLAD/include
FLAGS := -g -O0 -std=c++17 $(INCLUDES)
FLAGS_C := -g -O0 $(INCLUDES)
LIBS := -lstdc++ -lm -lglfw -ldl -lGL
TARGET := bin/main.bin


all: $(OBJ) $(OBJ_C) copy_assets
	@echo $(OBJ) $(OBJ_C) $(SRC_C)
	$(CC) $(FLAGS) $(OBJ) $(OBJ_C) -o $(TARGET) $(LIBS)

%.opp: %.cpp
	$(CC) -c $(FLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(FLAGS_C) $^ -o $@


run_main: all
	$(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJ)
	rm -rf $(OBJ_C)

copy_assets:
	cp -r assets bin