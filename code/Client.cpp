#include "socket.h"
#include <iostream>

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
ByteArray createPacket(std::string requestType, std::vector<std::string> content) {
  std::string output = requestType;
  for(int i = 0; i < content.size(); i++) {
    output += "-" + content[i];
  }

  return ByteArray(output);
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

ProcessedPacket sendRequestToServer(std::string requestType, std::vector<std::string> content) {

  ByteArray request(createPacket(requestType, content));

  Socket* socket = new Socket("127.0.0.1", 2002);
  (*socket).Open();

  (*socket).Write(request);//send the request to the server

  //server response
  ByteArray serverResponse;
  int conStatus = (*socket).Read(serverResponse);

  (*socket).Close();

  if(conStatus == 0) {//there is no connection with the server
    system("clear");
    std::cout << "we have disconnected from the server" << std::endl;
  } else if (conStatus < 0) {
    std::cout << "there was an issue processing your request, connection status < 0" << std::endl;
  } else {
    return processPacket(serverResponse);
  }

  return {"", ""};
}


//pages
void mainPage() {
  system("clear");//clear the screen
	std::cout << "You can exit the program by entering 'exit'.\n\nWelcome To our Chat application:\nAre you an existing user? (y/n)?" << std::endl;  
}

bool loginUserPage(Client* client) {
  system("clear");
  std::cout << "(enter 'back' to go back)\n\nPlease enter your username:" << std::endl;
  
  while(true) {
  ///get the user name
  std::string userName;
  std::getline(std::cin, userName);
  if(userName == "back") {//go back to previous page
    return false;
  }

  ProcessedPacket serverPac = sendRequestToServer( "login", {userName});//send request to the server
    
  if(serverPac.requesetType == "success") {
    (*client).userName = userName;
    return true;
  } else {
    std::cout << serverPac.content << std::endl;
  }
  
 }
}

bool createUserPage(Client* client) {
  system("clear");
  std::cout << "(enter 'back' to go back)\n\nPlease enter a username that you would like to have:" << std::endl;

  while(true) {
    std::string userName;
    std::getline(std::cin, userName);

    if(userName == "back") {//go back to the previous page
      return false;
    }

    ProcessedPacket serverPac = sendRequestToServer("addUser", {userName});//send request to the server

    if(serverPac.requesetType == "success") {
      (*client).userName = userName;
      return true;
    } else {
      std::cout << serverPac.content << std::endl;
    }
  }
}

void errorPage() {
  system("clear");
  std::cout << "There was an issue processing your request, enter any key to continue." << std::endl;
  std::cin.get();
  
}

void registration(Client* client) {
  system("clear");
  std::string userInput;//this string will contain the user's input
  
  bool exit = false;
  while(!exit) {
    mainPage();//displays the main page
    std::getline(std::cin, userInput);//get user response

    if(userInput == "y" || userInput == "Y" || userInput == "yes" || userInput == "Yes") {
      exit = loginUserPage(client);
    } else if(userInput == "n" || userInput == "N" || userInput == "No" || userInput == "no"){
      exit = createUserPage(client);
    } else if (userInput == "exit") {
      return;//exit the program
    } else {
      errorPage();
    }
  }
}

int main()
{
  Client* client = new Client{"", ""};
  
  registration(client);
  
  return 1;
}
