#include <iostream>
#include <thread>

static bool s_finished = false;

using namespace std::literals::chrono_literals;

void DoWork(std::string message) {
  while(!s_finished) {
    std::cout << message << std::endl;
    std::this_thread::sleep_for(1s);
  }
}

struct TwoUNAndVal {
  std::string un1;
  std::string un2;
  std::string other;
};

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



int main() {

  std::thread worker(DoWork, "test");
  
  TwoUNAndVal out = splitTwoUn("user1-user2-asdfasdfasdf");
  std::cout << out.un1 << " : " << out.un2 << " : " << out.other << std::endl;
  

  std::cin.get();//waits till we press enter
  s_finished = true;

  worker.join();//block this thread and wait for the worker thread to finish

  std::cout << "finished." << std::endl;

  std::cin.get();
}

