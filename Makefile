simulation: main.c
	gcc `pkg-config --cflags raylib` -o $@ $^ `pkg-config --libs raylib`

clean:
	rm simulation
