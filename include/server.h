#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H
#include <string>
#include "common.h"

using namespace std;

class Server {
public:

	Server();

	void Init();

	void Close();

	void Start();

private:

	int ForwardingMessage(int clientfd);

	struct sockaddr_in serverAddr;

	map<int, int> idTofdMap;

	int listener;

	int epfd;
};
#endif