CC = g++
DEBUG__FLAGS = -fsanitize=address,leak,undefined -Wall -g
I_FLAG = -I Include/

all: build

build: main.o Differentiator.o LogsLib.o TextLib.o Tree.o TexDump.o
	$(CC) main.o Differentiator.o LogsLib.o TextLib.o Tree.o TexDump.o  -o diff

debug: main.o Differentiator.o LogsLib.o TextLib.o Tree.o TexDump.o
	$(CC) main.o Differentiator.o LogsLib.o TextLib.o Tree.o TexDump.o -o diff $(DEBUG__FLAGS)

main.o: main.cpp
	$(CC) -c main.cpp $(I_FLAG)
Differentiator.o: Tree/Differentiator.cpp
	$(CC) -c Tree/Differentiator.cpp $(I_FLAG)
LogsLib.o: Logger/LogsLib.cpp
	$(CC) -c Logger/LogsLib.cpp $(I_FLAG)
TextLib.o: TextLib/TextLib.cpp
	$(CC) -c TextLib/TextLib.cpp $(I_FLAG)
Tree.o: Tree/Tree.cpp
	$(CC) -c Tree/Tree.cpp $(I_FLAG)
TexDump.o: Logger/TexDump.cpp
	$(CC) -c Logger/TexDump.cpp $(I_FLAG)

clean:
	rm -rf *.o diff
