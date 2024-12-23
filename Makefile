all: build
	./simulation

build: main.c
	gcc `pkg-config --cflags raylib` -o simulation main.c `pkg-config --libs raylib`

clean:
	rm simulation
