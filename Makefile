all:
	g++ -c ./sources/*.cpp
	g++ -c ./glad/glad.c
	g++ ./*.o -o main -lglfw3 -O2

windows:
	x86_64-w64-mingw32-g++ -c ./sources/*.cpp
	x86_64-w64-mingw32-g++ -c ./glad/glad.c
	x86_64-w64-mingw32-g++ ./*.o -o main -lglfw3 -lopengl32 -lwinmm -lgdi32 -static -O2

clean:
	rm -f ./*.o