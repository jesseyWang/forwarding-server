#include <iostream>
#include "client.h"

int main(int argc, char *argv[]) {
	Client client;
	if(argc < 3){
		std::cout <<  "Please input 3 argcs..." << std::endl;
		
		return 0;
	}

	client.Start(argv[1], argv[2]);

	return 0;
}