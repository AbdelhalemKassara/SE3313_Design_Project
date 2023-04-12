all: Client Server

Server: server.cpp
	g++ server.cpp -lpthread -o server.out

Client: client.cpp
	g++ client.cpp -lpthread -o client.out
