#include "socket.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include "SharedObject.h"

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

//this is responsible for sending all requests to the server
//it will handle all serverside issues
ProcessedPacket sendRequestToServer(std::string requestType, std::vector<std::string> content) {
  try {
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

  } catch (...) {
    std::cout << "There was an issue with the request to the server" << std::endl;
  }
  return {"", ""};
}

//this is the welcome page
void mainPage() {
  system("clear");//clear the screen
	std::cout << "You can exit the program by entering 'exit'.\n\nWelcome To our Chat application:\nAre you an existing user? (y/n)?" << std::endl;  
}

//this is the login page where we will keep prompting the user to enter in the username
//until they enter a valid username or want to go back
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

//this displays the account creation page and won't leave unitl 
//the user enters in a valid username or wants to go back
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

    if(serverPac.requesetType == "success") {//if the account was created successfully
      (*client).userName = userName;
      return true;
    } else { //display the issue with the username
      std::cout << serverPac.content << std::endl;
    }
  }
}

//this is a general error page to display that something has gone wrong
void errorPage() {
  system("clear");
  std::cout << "There was an issue processing your request, enter any key to continue." << std::endl;
  std::cin.get();
  
}

//this will display all the registered users
//and won't leave this page until the user has entered an a user that they want to talk to.
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

void handleUserInput(std::string userName, std::string otherUserName, bool* quit, std::string* userMessage, bool* back) {

  char pressed;//this will get written over later

  while(pressed != '\n') {//gets each charater the user enters and adds it to the message variable
    pressed = getchar();
    if(pressed != '\n') {
      (*userMessage) += pressed;
    }
  }

  pressed = 'a';//this will get written over later

  if(*userMessage == "=quit") {
    *quit = true;
    (*userMessage).clear();
    return;
  } else if(*userMessage == "=back") {
    *back = true;
    (*userMessage).clear();
    return;
  } else {
    sendRequestToServer("addMessage", {userName, otherUserName, *userMessage});
    (*userMessage).clear();
  }

}

bool chatWithUser(Client* client) {
  std::string* userMessage = new std::string("");

  bool* quit = new bool(false);
  bool* back = new bool(false);

  while(!*back && !*quit) {
    std::thread* thread = new std::thread(handleUserInput, (*client).userName, (*client).otherUserName, quit, userMessage, back);

    system("clear");

    std::cout << "enter =quit exit the program or enter =back to go back." << std::endl;

    ProcessedPacket serverPac = sendRequestToServer("getChat", {(*client).userName, (*client).otherUserName});
    std::cout << serverPac.content << std::endl << std::endl;

    (*thread).join();
    
  }
  bool output(*quit);
  delete quit;
  delete back;
  delete userMessage;
  
  return output;
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
  
  try {
    registration(client);

    bool exit = false;
    while(!exit) {
      viewOtherUsersPage(client);
      exit = chatWithUser(client);
    }

  } catch (...)
    {
      std::cout << "There was an issue with the program." << std::endl;
    }

  return 1;
}
