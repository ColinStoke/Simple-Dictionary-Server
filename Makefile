FLAGS := -g -Wall
OBJECTS := server1.o server2.o server3.o 

all: client

client: client.cpp
	g++ $(FLAGS) client.cpp -o client


clean:
	rm -f client server1 server2 server3