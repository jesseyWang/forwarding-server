#include <iostream>
#include "pressureClient.h"

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cout <<  "Please input 3 argcs..." << std::endl;
		
		return 0;
	}

	PressureClient pressure;

	int clientNum = atoi(argv[1]);
	int postLen = atoi(argv[2]);

	pressure.Start(clientNum, postLen);

	return 0;
}