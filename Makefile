all:
	g++ -c -g ./sources/*.cpp ./sources/*.c
	g++ -c ./glad/glad.c
	g++ ./*.o -o main -lglfw3 -lfreetype -O2

windows:
	x86_64-w64-mingw32-g++ -c ./sources/*.cpp ./sources/*.c
	x86_64-w64-mingw32-g++ -c ./glad/glad.c
	x86_64-w64-mingw32-g++ ./*.o -o main -static -lglfw3 -lopengl32 -lwinmm -lfreetype -lgdi32 -O2

clean:
	rm -f ./*.o