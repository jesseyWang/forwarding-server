#ifndef PRESSURE_CLIENT_H
#define PRESSURE_CLIENT_H
#include "common.h"

using namespace std;

class PressureClient
{
public:
	PressureClient();

	void Start(int clientNum, int postLen);

	void Connect(int clientNo);

	void Close();
	
private:
	int sock[EPOLL_SIZE];

	int epfd;

	struct sockaddr_in serverAddr;
};

#endif
