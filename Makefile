MAIN = src/main.cpp
SRC = 

build: 
	g++ -g $(SRC) $(MAIN) -o app  -std=gnu++11 -I/opt/homebrew/include -L/opt/homebrew/lib `libpng-config --cflags` -lpng