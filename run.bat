cd generator

gcc generate.c -o generate.exe

cd ..

generator\generate.exe

gcc lib/*/*.c *.c -g -I ./include/ -lopengl32 -lgdi32 -lpthread -o main.exe && main.exe
