all:
	clang ./src/main.c -o build -I/usr/include/ -L/usr/lib/ -lglut -lGL -lGLEW -lGLU -std=gnu17 -lm
	chmod u+x ./build && ./build

install:
	sudo apt install freeglut3-dev libglew-dev clang --no-install-recommends -y

clean:
	rm ./build
