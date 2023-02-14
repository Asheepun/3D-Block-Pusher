g++ lib/engine/*.cpp lib/glad/glx.c lib/glad/gl.c *.cpp -g -I ./include/ -ldl -lm -lGL -lX11 -lXfixes -lpthread -o main && ./main
