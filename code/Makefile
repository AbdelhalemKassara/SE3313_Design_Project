all : Client Server

Client: Client.o Blockable.o socket.o
	g++ -o Client.out Client.o Blockable.o socket.o -pthread -l rt

Server: Server.o Blockable.o socketserver.o socket.o thread.o
	g++ -o Server.out Server.o Blockable.o socketserver.o socket.o thread.o -pthread -l rt
	
Client.o : Client.cpp Blockable.h SharedObject.h Semaphore.h Database.h socket.h
	g++ -c Client.cpp -std=c++11

Blockable.o : Blockable.cpp Blockable.h
	g++ -c Blockable.cpp -std=c++11
	
Server.o : Server.cpp SharedObject.h Semaphore.h thread.h socketserver.h
	g++ -c Server.cpp -std=c++11 

thread.o : thread.cpp thread.h
	g++ -g -c thread.cpp -std=c++11

socket.o : socket.cpp socket.h
	g++ -g -c socket.cpp -std=c++11

socketserver.o : socketserver.cpp socket.h socketserver.h
	g++ -g -c socketserver.cpp -std=c++11
