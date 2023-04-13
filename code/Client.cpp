#include "socket.h"
#include <iostream>
#include <thread>

using namespace Sync;

struct Client {
  std::string userName;
  std::string otherUserName;
};

struct ProcessedPacket {
  std::string requesetType;
  std::string content;
};


/*
format of the string is requestType-content

requestType can't contain a - 
content can contain anything
*/
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



int main()
{
  Client* client = new Client{"", ""};
  
	Socket socket("127.0.0.1", 2002);
  socket.Open();


  socket.Write(createPacket("getUsers", "asdfasdf"));

  //registration(client);

  //server response
  ByteArray serverResponse;
  int conStatus = socket.Read(serverResponse);

  if(conStatus == 0) {//there is no connection with the server
    std::cout << serverResponse.ToString() << std::endl;
    std::cout << "we have disconnected from the server" << std::endl;
    return 1;
  } else if (conStatus < 0) {
    std::cout << "there was an issue processing your request, connection status < 0" << std::endl;
  } else {
    std::cout << serverResponse.ToString() << std::endl;
  }

  socket.Close();
  return 1;
}


void mainPage() {
  system("clear");//clear the screen
	std::cout << "Welcome To our Chat application:\nAre you an existing user? (y/n)" << std::endl;  
}

void loginUserPage(Client* client) {
  system("clear");

}

void createUserPage() {
  system("clear");
}

void errorPage() {
  system("clear");
  std::cout << "There was an issue processing your request." << std::endl;
  
}

void registration(Client* client) {
  std::string userInput;//this string will contain the user's input
  
  while(true) {
    mainPage();//displays the main page
    std::getline(std::cin, userInput);//get user response

    if(userInput == "y" || userInput == "Y" || userInput == "yes" || userInput == "Yes") {
      loginUserPage(client);
      //check if the user already exists on the database
      break;
    } else if(userInput == "n" || userInput == "N" || userInput == "No" || userInput == "no"){
      createUserPage();
      //check if the user already exists on the database
      break;
    } else {
      errorPage();
      //std::this_thread::sleep_for(1s);
    }
  }
}


