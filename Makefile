all:
	g++ ./sources/main.cpp ./sources/sprite.cpp  ./sources/graphics.cpp ./glad/glad.c -o main -lglfw3 -O2

windows:
	x86_64-w64-mingw32-g++ ./sources/main.cpp ./sources/sprite.cpp  ./sources/graphics.cpp ./glad/glad.c -o main -lglfw3 -lopengl32 -lwinmm -lgdi32 -static -O2