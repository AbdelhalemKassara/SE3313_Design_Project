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

int main() {

  std::thread worker(DoWork, "test");
  
  std::cin.get();//waits till we press enter
  s_finished = true;

  worker.join();//block this thread and wait for the worker thread to finish

  std::cout << "finished." << std::endl;

  std::cin.get();
}