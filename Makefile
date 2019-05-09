CC = g++
CFLAGS = `pkg-config --cflags --libs libconfig++ libmicrohttpd libcurl`

# this makes the binary
all:
	$(CC) -o /usr/bin/torgate src/*.cpp src/*/*.cpp $(CFLAGS)

# this makes a debuggable binary
debug:
	$(CC) -g3 -o bin/torgate src/*.cpp src/*/*.cpp $(CFLAGS)