#include "Framework.h"
#include <iostream>
#include <string>
#include <thread>

int main()
{
	//Capture();
	//std::thread t1(pyRPM);
	std::thread t1(createRPM);
	std::thread t2(PlayStatic);
	
	t2.join();
	t1.join();
	//t3.join();
	return 0;
}