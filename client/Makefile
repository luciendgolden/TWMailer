CC = g++
CFLAGS = -std=c++11 -c -g -Wall
OBJECTS = request/request.o myclient.o

all: client

request/request.o: request/request.h
	$(CC) $(CFLAGS) -o request/request.o request/request.h

myclient.o: myclient.cpp
	${CC} ${CFLAGS} myclient.cpp -o myclient.o

client: $(OBJECTS)
	${CC} $(OBJECTS) -o client


clean:
	rm -f $(OBJECTS) client

# g++ options:
# -g        debugging information
# -Wall     show all warnings
# -c        compile only, no linking
# -o        specify output
