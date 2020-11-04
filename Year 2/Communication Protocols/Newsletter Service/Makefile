CC = g++
CFLAGS = -Wall -g

all: server subscriber

# Compileaza server.c
server: server.cpp helpers.cpp
	${CC} -Wall $^ -o server

# Compileaza subscriber.c
subscriber: subscriber.cpp helpers.cpp
	${CC} -Wall $^ -o subscriber

.PHONY: clean run_server run_client

clean:
	rm -f server subscriber
