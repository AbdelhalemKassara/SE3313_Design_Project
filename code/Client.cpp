#include "socket.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include "SharedObject.h"
#include <mutex>

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

  Socket* socket = new Socket("127.0.0.1", 2000);
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

void viewOtherUsersPage(Client* client) {
  while(true) {
    system("clear");
    std::cout << "Users you can chat with:" << std::endl;
    ProcessedPacket serverPac = sendRequestToServer("getUsers", {" "});
    std::cout << serverPac.content << std::endl << std::endl;


    //getting the user that we will start chatting with
    std::string userName;
    std::getline(std::cin, userName);

    if(userName != (*client).userName) {
      serverPac = sendRequestToServer("userExists", {userName});
      
      if(serverPac.requesetType == "success") {
        (*client).otherUserName = userName;
        return;
      } else {
        system("clear");
        std::cout << serverPac.content << " Press any key to continue." << std::endl;
        std::cin.get();
      }
    } else {
      system("clear");
      std::cout << "You can't chat with yourself, press any key to continue." << std::endl;
      std::cin.get();
    }


  }

}

void handleUserInput(std::string userName, std::string otherUserName, bool* quit) {
  Shared<std::string> userMessage("clientMessage");

  char pressed = 'a';//this will get written over later

  while(true) {
    while(pressed != '\n') {//gets each charater the user enters and adds it to the message variable
      pressed = getchar();
      *userMessage += pressed;
    }

    pressed = 'a';//this will get written over later
    if(*userMessage == "=quit") {
      *quit = true;
      *userMessage = "";
      return;
    } else {
      sendRequestToServer("addMessage", {userName, otherUserName, *userMessage});
      *userMessage = "";
    }
  }
}

bool chatWithUser(Client* client) {
  Shared<std::string> userMessage("clientMessage", true);

  bool* quit = new bool(false);

  std::thread* thread = new std::thread(handleUserInput, (*client).userName, (*client).otherUserName, quit);

  (*thread).detach();

  while(!*quit) {
    system("clear");
    std::cout << "enter =quit to go back." << std::endl;


    ProcessedPacket serverPac = sendRequestToServer("getChat", {(*client).userName, (*client).otherUserName});
    std::cout << serverPac.content << std::endl << std::endl;
    std::cout << *userMessage;//eempty string for some reason?
   

   sleep(1);
  }

  while(true) {
    if((*thread).joinable()) {
      (*thread).join();
      break;
    }
  }

  //add code for deleteing pointers and force quitting the thread (use bool variable pointer)
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

  while(true) {
    viewOtherUsersPage(client);
    chatWithUser(client);
  }

  return 1;
}
