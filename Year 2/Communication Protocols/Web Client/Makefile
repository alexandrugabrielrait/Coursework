CC=g++

client: client.cpp requests.cpp helpers.cpp buffer.c commands.cpp parser.cpp
	$(CC) -o client client.cpp requests.cpp helpers.cpp buffer.c commands.cpp parser.cpp -Wall -g

run: client
	./client

clean:
	rm -f *.o client