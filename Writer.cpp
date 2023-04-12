#include "thread.h"
#include <thread>
#include <unistd.h>
#include <iostream>
#include "Blockable.h"
#include <time.h>
#include "SharedObject.h"
#include <stack>
#include "Semaphore.h"

Semaphore semReader("reader", 1, true);
Semaphore semWriter("writer", 1, true);

struct MyShared
{
	int threadId;
	int reportNum;
	int delayT;
};

class WriterThread : public Thread
{
public:
	int threadId;
	bool continueLoop = true;
	int delay;
	int reportNum = 0;

	WriterThread(int in, int delayTime) : Thread(delayTime * 1000)
	{
		this->threadId = in;
		this->delay = delayTime;
	}

	virtual long ThreadMain(void) override
	{

		// declare shared memory var so this thread can access it
		Shared<MyShared> sharedMem("origin");
		time_t startTime = time(0);

		while (continueLoop)
		{
			semWriter.Wait();

			sleep(delay);
			this->reportNum++;

			sharedMem->reportNum = reportNum;
			sharedMem->threadId = threadId;
			sharedMem->delayT = delay;

			semReader.Signal();
		}
		return 1;
	}
};

int main()
{
	std::cout << "I am a Writer" << std::endl;

	Shared<MyShared> shared("origin", true); // This is the owner of sharedMamory

	int threadNum = 0;
	std::stack<WriterThread *> threadsStack; // we will store the threads in this stack
	WriterThread *thread;

	std::string delayStr;
	std::string newThreadStr;

	while (true)
	{
		std::cout << "Would you like to create a writer thread? (y/n)" << std::endl;
		std::getline(std::cin, newThreadStr);

		if (newThreadStr == "y")
		{
			std::cout << "What is the delay time for this thread?" << std::endl;
			std::getline(std::cin, delayStr);

			threadNum++;
			int delayInt = std::stoi(delayStr);
			thread = new WriterThread(threadNum, delayInt);
			threadsStack.push(thread);
		}
		else if (newThreadStr == "n")
		{
			break;
		}
	}

	// we aren't creating threads anymore when we reach this point so we will delete them.
	for (int i = 0; i < threadsStack.size(); i++)
	{
		thread = threadsStack.top();
		thread->continueLoop = false;
		delete thread;
		threadsStack.pop();
	}
}
