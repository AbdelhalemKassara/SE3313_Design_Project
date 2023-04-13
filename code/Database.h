#ifndef DATABASE_H
#define DATABASE_H


#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

struct Message {
	std::string user;
	std::string message;
};

struct Chat {
	std::string user1;
	std::string user2;
	std::vector<Message> messages;
};

class Database {
	private:
	std::vector<std::string> users;
	std::vector<Chat*> chats;


	public:
	Database() {
	}

  ~Database() {

  }


	bool addUser(std::string user) {
    if(!doesUserExist(user)) {
      users.push_back(user);
      return false;
    }

    return true;
	}

	std::vector<Message>* getChatMessages(std::string user1, std::string user2) {
    if(doesChatExist(user1, user2)) {//returns existing messages
      return &(*getChatStruct(user1, user2)).messages;
    } else {//creates a new chat if it doesn't exist
        //currently we are not dealing with the case if the users don't exist or are the same
        //we should probably deal with this by throwing an exception
        bool result = createChat(user1, user2);
        return &(*getChatStruct(user1, user2)).messages;
    }
	}

	bool addMessage(std::string user, int chatIndex, std::string message) {
		//adds a message to a chat if it exists (returns false if it doesn't)
    std::vector<Message> chatMessages = (*chats[chatIndex]).messages;
	}

  bool addMessage(std::string senderUser, std::string receiverUser, std::string message) {
    std::vector<Message>* chatMessages = getChatMessages(senderUser, receiverUser);
    
    Message messageObj = {senderUser, message};
    (*chatMessages).push_back(messageObj);
	}

  std::string getFullChat(std::string user1, std::string user2) {
    std::string output = "";
    std::vector<Message> messages = (*getChatStruct(user1, user2)).messages;

    for(int i = 0; i < messages.size(); i++) {
      output += messages[i].user + " : " + messages[i].message;
    }

    return output;
  }

  //delete these, these are just for testing
  std::string getUsers() {
    std::string out = "";
    for(int i = 0; i < users.size(); i++) {
      out += users[i] + "\n";
    }
    
    return out;
  }
	
	bool doesUserExist(std::string user) {
		for(int i = 0; i < users.size(); i++) {
			if(users[i] == user) {
				return true;
			}
		}

		return false;
	}

  private:

	bool createChat(std::string user1, std::string user2) {
		if(user1 != user2 && doesUserExist(user1) && doesUserExist(user2)) {
      Chat* chat = new Chat{user1, user2, {}};

			chats.push_back(chat);
			return true;
		}

		return false;
	}

	bool doesChatExist(std::string user1, std::string user2) {
		for(int i = 0; i < chats.size(); i++) {
			if((*chats[i]).user1 == user1 && (*chats[i]).user2 == user2 ||
				(*chats[i]).user1 == user2 && (*chats[i]).user2 == user1) {
					return true;
				}
		}

		return false;
	}

	bool doesChatExist(int chatIndex) {
		return chatIndex < chats.size();
	}

  Chat* getChatStruct(std::string user1, std::string user2) {
		for(int i = 0; i < chats.size(); i++) {
			if((*chats[i]).user1 == user1 && (*chats[i]).user2 == user2 ||
				(*chats[i]).user1 == user2 && (*chats[i]).user2 == user1) {
					return chats[i];
				}
		}    
  }

};
#endif // DATABASE_H
