CC = g++
CFLAGS = `pkg-config --cflags --libs libconfig++ libmicrohttpd libcurl`

# this makes the binary
all:
	$(CC) -g3 -o bin/torgate src/*.cpp src/*/*.cpp $(CFLAGS)
