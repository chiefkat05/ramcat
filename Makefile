all:
	g++ -c ./sources/*.cpp ./sources/*.c
	g++ -c ./glad/glad.c
	g++ ./*.o -o main -lglfw3 -lfreetype -O2

windows:
	x86_64-w64-mingw32-g++ -c ./sources/*.cpp ./sources/*.c
	x86_64-w64-mingw32-g++ -c ./glad/glad.c
	x86_64-w64-mingw32-g++ ./*.o -o main -lglfw3 -lopengl32 -lwinmm -lgdi32 -lfreetype -static -O2

clean:
	rm -f ./*.o