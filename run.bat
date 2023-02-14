g++ lib/engine/*.cpp lib/glad/wgl.c lib/glad/gl.c *.cpp -g -I ./include/ -lopengl32 -lgdi32 -lpthread -o main.exe && main.exe
