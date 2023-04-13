#include "socketserver.h"
#include <iostream>
#include <thread>
#include "thread.h"
#include <vector>
#include "Database.h"
#include "Semaphore.h"
#include "SharedObject.h"
#include "Blockable.h"
#include <cstring>  

using namespace Sync;

struct ProcessedPacket {
  std::string requesetType;
  std::string content;
};
struct DataBaseContainer {
  Database* db;
};
ByteArray createPacket(std::string requestType, std::string content) {
  return ByteArray(requestType + "-" + content);
}

ProcessedPacket processPacket(ByteArray b) {
  std::string pacStr = b.ToString();
  ProcessedPacket procPac = {"", ""};
  
  for(int i = 0; i < pacStr.length(); i++) {
    if(pacStr.at(i) == '-') {
      procPac.requesetType = pacStr.substr(0, i);
      
      if(i+1 < pacStr.length()) {
        procPac.content = pacStr.substr(i+1, pacStr.length());
      }
    }
  }

  return procPac;
}
ProcessedPacket splitTwo(std::string pacStr) {
  ProcessedPacket procPac = {"", ""};
  
  for(int i = 0; i < pacStr.length(); i++) {
    if(pacStr.at(i) == '-') {
      procPac.requesetType = pacStr.substr(0, i);
      
      if(i+1 < pacStr.length()) {
        procPac.content = pacStr.substr(i+1, pacStr.length());
      }
    }
  }

  return procPac;
}



//checks if a string has a dash inside of it
bool hasDash(std::string content) {
    for(int i = 0; i < content.length(); i++) {
      if(content.at(i) == '-') {
        return true;
      }
    }
  return false;
}
struct TwoUNAndVal {
  std::string un1;
  std::string un2;
  std::string other;
};

//splits un1-un2-anythingElse
//splits un1-un2-anythingElse
TwoUNAndVal splitTwoUn(std::string str) {
  int count = 0;
  int previousIndex = 0;
  TwoUNAndVal out = {"", "", ""};
  int i = 0;
  for(; i < str.length(); i++) {
    if(str.at(i) == '-') {
      if(count == 0) {
        out.un1 = str.substr(previousIndex, i-previousIndex);
        previousIndex = i+1;
      } else if (count == 1) {
        out.un2 = str.substr(previousIndex, i-previousIndex);
        previousIndex = i+1;
      }
      count ++; 
    }
  }
  out.other = str.substr(previousIndex, i-previousIndex);
  return out;
}

ProcessedPacket performRequest(ProcessedPacket pack, Database* db) {
  std::string req = pack.requesetType;
  std::cout << req << std::endl;

  if(req == "addUser") {
    //format of packet from client
    //username
    bool containsDash = hasDash(pack.content);
    if(containsDash) {
      return {"error" , "This username is invalid, your username can't contain a -."};
    }

    bool exists = db->addUser(pack.content);

    if(exists) {//the user already exists
      return {"error", "This user already exists."};
    } else {
      return {"success", "The account has been added"};
    }
  } else if (req == "addMessage"){
    /*
      format of packet from client
      senderUN-reciverUN-message
    */
    TwoUNAndVal messageInfo = splitTwoUn(pack.content);

    db->addMessage(messageInfo.un1, messageInfo.un2, messageInfo.other);
    std::string chat = db->getFullChat(messageInfo.un1, messageInfo.un2);

    return {"success", chat};
  } else if (req == "createChat") {
    //format of packet from client
    //senderUN-reciverUN
    ProcessedPacket p = splitTwo(pack.content);
    std::string user1 = p.requesetType;
    std::string user2 = p.content;

    db->getChatMessages(user1, user2);//this will automatically create a new chat if it doesn't exist
    return {"success", "A group Chat has been created"};
  } else if (req == "login") {
    //format of packet from client
    //senderUN
    bool exists = db->doesUserExist(pack.content);

    if(exists) {
      return {"success", "You have logged in."};
    } else {
      return {"error", "This user doesn't exist"};
    }
  } else if (req == "getUsers") {
    //format of the packet:
    //userName
    
    std::string output = db->getUsers();//this currently is getting all users  
    return {"success", output};
  }
  else {
    return {"error", "This request is invaild."};
  }
}

//this is the client request handler
void dealWithClientRequest(Socket* socket) {

  ByteArray clientData;
  int val = (*socket).Read(clientData);

  if(val < 0) {
    std::cout << "There was an issue" << std::endl;
    return;
  } if(val == 0) {
    std::cout << "Socket from the client side has been closed" << std::endl;
    return;
  }

  //process the client's request
  ProcessedPacket packet = processPacket(clientData.ToString());

  Shared<DataBaseContainer> sharedMem("origin");
  //semaphore stuff here
  ProcessedPacket returnPacket = performRequest(packet, sharedMem->db);


  //response to the client
  ByteArray out(createPacket(returnPacket.requesetType, returnPacket.content));
  (*socket).Write(out);
  (*socket).Close();

}

//this thread creates other threads to handle the client requests
class ReqThread : public Thread {
  private:
  SocketServer *server;
  std::vector<std::thread*>* threads;

  public:
  ReqThread(SocketServer *server, std::vector<std::thread*> *threads) {
    (*this).server = server;
    (*this).threads = threads;
  }
  

  virtual long ThreadMain(void) {
    Shared<DataBaseContainer> sharedMem("origin");
    sharedMem->db = new Database();//add a new instance of the database to the shared memory
    
    while(true) {
      try {
        Socket soc = (*server).Accept(); //wait until a client connects
        Socket *tempSoc = new Socket(soc);
        
        std::thread* worker = new std::thread(dealWithClientRequest, tempSoc);
        (*threads).push_back(worker);

        (*worker).detach();//let the thread work independently

      } catch (std::string s) {
        std::cout << s << std::endl;
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
  SocketServer server(2002);
  ReqThread* requestThread = new ReqThread(&server, &threads);

  FlexWait cinWaiter(1, stdin);
  cinWaiter.Wait();
  std::cin.get();


  //wait until all client handler threads are done
  for(int i = 0; i < threads.size(); i++) {
    if((*threads[i]).joinable()) {
      (*threads[i]).join();//waits until the client threads have been handled
      delete threads[i];
      i--;
    }
  }  

  server.Shutdown();
}
