all : Client Server

Client: Client.o thread.o Blockable.o
	g++ -o Client Client.o thread.o Blockable.o -pthread -l rt

Server: Server.o thread.o Blockable.o
	g++ -o Server Server.o -pthread -l rt
	
Client.o : Client.cpp thread.h Blockable.h SharedObject.h Semaphore.h
	g++ -c Client.cpp -std=c++11

thread.o : thread.cpp thread.h Blockable.h
	g++ -c thread.cpp -std=c++11

Blockable.o : Blockable.cpp Blockable.h
	g++ -c Blockable.cpp -std=c++11
	
Server.o : Server.cpp SharedObject.h Semaphore.h
	g++ -c Server.cpp -std=c++11 
