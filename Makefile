MAIN = src/main.cpp
OUTPUT_PATH = ./out/app 
SRC = src/camera/camera_rig.cpp src/utilities/util.cpp

COM = g++
COM_FLAGS = -g -Wall -Wextra -std=c++17
INCLUDE = -I/sammc/metal-cpp -isystem /opt/homebrew/include -L/opt/homebrew/lib `libpng-config --cflags` -lpng -framework Metal -framework Foundation -framework QuartzCore

build: $(MAIN)
	$(COM) $(COM_FLAGS) $(INCLUDE) -o $(OUTPUT_PATH) $(MAIN) $(SRC)

test: build
	time $(OUTPUT_PATH) 2>> out/timing_results.txt

dbg: build
	dbg ./out/app

clean:
	rm -r ./out/*

build_test:
	$(COM) $(COM_FLAGS) src/utilities/util.cpp test/test_linear_algebra.cpp -o test/test_linear_algebra