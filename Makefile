MAIN = src/main.cpp
OUTPUT_PATH = ./out/app 

build: $(MAIN)
	clear
	g++ -g $(SRC) $(MAIN) -o $(OUTPUT_PATH)  -std=gnu++11 -I/opt/homebrew/include -L/opt/homebrew/lib `libpng-config --cflags` -lpng

test: build
	time $(OUTPUT_PATH) 2>> out/timing_results.txt

dbg: build
	dbg ./out/app