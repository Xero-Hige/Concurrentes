CC = gcc
CFLAGS = -Wall -Wextra -g -pedantic
CPPFLAGS = -Wall -Wextra -g -pedantic -lstdc++ -lm -std=c++11 -O3

OBJECTS = 

all: exec

exec: clase4.cpp $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) clase4.cpp -o exec

clean:
	rm -r *.o *~ exec

clean-o:
	rm -r *.o

run: exec
	./exec
