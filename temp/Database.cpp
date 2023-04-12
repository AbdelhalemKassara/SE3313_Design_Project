#include <iostream>
#include <vector>

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
	std::vector<std::string>* users;
	std::vector<Chat>* chats;

	bool doesUserExist(std::string user) {
		for(int i = 0; i < (*users).size(); i++) {
			if((*users)[i] == user) {
				return true;
			}
		}

		return false;
	}

	bool createChat(std::string user1, std::string user2) {
		if(user1 != user2 && doesUserExist(user1) && doesUserExist(user2)) {
			(*chats).push_back({user1, user2, {}});
			return true;
		}

		return false;
	}

	bool doesChatExist(std::string user1, std::string user2) {
		for(int i = 0; i < (*chats).size(); i++) {
			if((*chats)[i].user1 == user1 && (*chats)[i].user2 == user2 ||
				(*chats)[i].user1 == user2 && (*chats)[i].user2 == user1) {
					return true;
				}
		}

		return false;
	}

	bool doesChatExist(int chatIndex) {
		return chatIndex < (*chats).size();
	}

  Chat getChatStruct(std::string user1, std::string user2) {
		for(int i = 0; i < (*chats).size(); i++) {
			if((*chats)[i].user1 == user1 && (*chats)[i].user2 == user2 ||
				(*chats)[i].user1 == user2 && (*chats)[i].user2 == user1) {
					return (*chats)[i];
				}
		}    
  }

	public:
	Database() {
		users = {};
		(*chats) = {};
	}
  
  ~Database() {

  }

	bool addUser(std::string user) {
    if(!doesUserExist(user)) {
      (*users).push_back(user);
      return true;
    }

    return false;
	}

	std::vector<Message> getChat(std::string user1, std::string user2) {
    if(doesChatExist(user1, user2)) {//returns existing messages
      return getChatStruct(user1, user2).messages;
    } else {//creates a new chat if it doesn't exist
        //currently we are not dealing with the case if the users don't exist or are the same
        //we should probably deal with this by throwing an exception
        bool result = createChat(user1, user2);
        return getChatStruct(user1, user2).messages;
    }
	}

	bool addMessage(std::string user, int chatIndex, std::string message) {
		//adds a message to a chat if it exists (returns false if it doesn't)
    std::vector<Message> chatMessages = (*chats)[chatIndex].messages;
	}
  bool addMessage(std::string user1, std::string user2, std::string message) {
    std::vector<Message> chatMessages = getChat(user1, user2);

		
	}
  //delete these, these are just for testing
  std::string getUsers() {
    std::string out = "";
    for(int i = 0; i < (*users).size(); i++) {
      out += (*users)[i] + ", ";
    }
    
    return out;
  }

};

int main() {
  Database* database = new Database();
  (*database).addUser("testUser1");
  (*database).addUser("testUser2");
  (*database).getChat("testUser1", "testUser2"); //creates the chat

  return 1;
}