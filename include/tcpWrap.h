#ifndef TCPWRAP_H
#define TCPWRAP_H 
#include "common.h"

class TcpDataWrap
{
public:
	TcpDataWrap();
public:
	int sendPackData(int sockfd, int tofd, char* buf, int bufLen);
	int recvUnpackData (int sockfd ,int* toID, char*& recvBuf);
};


#endif // !TCPWRAP_H

