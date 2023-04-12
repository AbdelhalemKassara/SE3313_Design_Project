#include <iostream>
#include "SharedObject.h"
#include "Semaphore.h"

struct MyShared
{
	int threadId;
	int reportNum;
	int delayT;
};

int main(void)
{
	Semaphore semReader("reader", 1);
	Semaphore semWriter("writer", 1);

	std::cout << "I am a reader" << std::endl;
	std::cout << "Thread Number | Report Id | Time Elapsed | Delay" << std::endl;

	Shared<MyShared> sharedMem("origin");
	time_t startTime = time(0);

	while (true)
	{
		semReader.Wait();

		int timer = time(0) - startTime;
		std::cout << sharedMem->threadId << " | " << sharedMem->reportNum << " | " << timer << " | " << sharedMem->delayT << std::endl;

		semWriter.Signal();
	}
}
