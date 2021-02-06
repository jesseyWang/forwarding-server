#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H
#include <string>
#include "common.h"

using namespace std;

class Client {
public:

	Client();

	void Connect();

	void Close();

	void Start(char* toID, char* selfID);

private:

	int sock;

	int epfd;

	bool isClientwork;

	struct sockaddr_in serverAddr;

};

#endif