MAIN = src/main.cpp
OUTPUT_PATH = ./out/app 
SRC = src/camera/camera_rig.cpp src/utilities/util.cpp
FLAGS = -g -Wall -Wextra -std=gnu++11 -I/opt/homebrew/include -L/opt/homebrew/lib `libpng-config --cflags` -lpng

build: $(MAIN)
	clear
	g++ $(FLAGS) -o $(OUTPUT_PATH) $(MAIN) $(SRC)

test: build
	time $(OUTPUT_PATH) 2>> out/timing_results.txt

dbg: build
	dbg ./out/app