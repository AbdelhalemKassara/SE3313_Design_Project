#include "socketserver.h"
#include <iostream>
#include <thread>
#include "thread.h"
#include <vector>

using namespace Sync;


void dealWithClientRequest(Socket* socket) {
  ByteArray clientData;
  int val = (*socket).Read(clientData);

  //prints the clients request
  std::cout << clientData.ToString() << std::endl;//write the client request to the console 

  //response to the client
  ByteArray out("server response");
  (*socket).Write(out);
  (*socket).Close();

}

class ReqThread : public Thread {
  private:
  SocketServer *server;
  std::vector<std::thread*> threads;

  public:
  ReqThread(SocketServer *server) {
    (*this).server = server;
  }

  ~ReqThread() {
    //wait until all client handler threads are done
    for(int i = 0; i < threads.size(); i++) {
      (*threads[i]).join();//waits until the client threads have been handled
    }  
  }

  virtual long ThreadMain(void) {
    while(true) {
      try {
        Socket soc = (*server).Accept(); //wait until a client connects
        Socket *tempSoc = new Socket(soc);
        
        std::thread* worker = new std::thread(dealWithClientRequest, tempSoc);
        threads.push_back(worker);

        (*worker).detach();//let the thread work independently

      } catch (std::string s) {

      } catch (TerminationException e) {
        std::cout << "Server has been terminated." << std::endl;
      }
    }
    return 0;
  }

};

int main(void)
{
  std::vector<std::thread*> threads;

  SocketServer server(2001);
  bool exit = false;

  while(!exit) {//keep creating a new thread for each client that connects
    Socket soc = server.Accept(); //wait until a client connects
    Socket *tempSoc = new Socket(soc);
    std::thread* worker = new std::thread(dealWithClientRequest, tempSoc);
    threads.push_back(worker);

    (*worker).detach();//let the thread work independently
  }

  //wait until all threads are done
  for(int i = 0; i < threads.size(); i++) {
    (*threads[i]).join();//waits until the client threads have been handled
  }

  server.Shutdown();
}
 
ByteArray createPacket(std::string requestType, std::string content) {
  return ByteArray(requestType + "-" + content);
}