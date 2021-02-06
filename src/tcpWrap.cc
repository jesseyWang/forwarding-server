#include <iostream>
#include "tcpWrap.h"

TcpDataWrap::TcpDataWrap() {}

//对数据进行封包，然后发送
int TcpDataWrap::sendPackData(int sockfd, int tofd, char* buf, int bufLen)
{
	Packet pack;
	int ret;
	size_t headSize = sizeof(pack.length);
	size_t cmdSize = sizeof(pack.tofd);
	size_t totalLen = headSize + cmdSize + bufLen;
	
	pack.length = bufLen;
	pack.tofd = tofd;
	char* sendBuf = new char[totalLen + 1];
	memset(sendBuf, 0, totalLen);
	sendBuf[totalLen] = '\0';
	memcpy(sendBuf, &pack.length, headSize);
	memcpy(sendBuf + headSize, &pack.tofd, cmdSize);
	memcpy(sendBuf + headSize + cmdSize, buf, bufLen);

	// 客户端和服务器端的sockfd都相同
	std::cout << "sendbuf:" << sendBuf + headSize + cmdSize << std::endl;
	std::cout << "end of printing" << std::endl;
	ret = send(sockfd, sendBuf, totalLen,0);	//发送数据
	delete[] sendBuf;

	return ret;
}

//收到得数据存储在recvbuf中，并返回recvBuf的大小
int TcpDataWrap::recvUnpackData (int sockfd , int* toID, char*& recvBuf)
{
	Packet pack;
	int ret;

	recv(sockfd, &pack.length, sizeof(pack.length), 0);
	recv(sockfd, &pack.tofd, sizeof(pack.tofd), 0);
	
	*toID = pack.tofd;
	char* sendBuf = new char[pack.length + 1];
	memset(sendBuf, 0, pack.length);
	sendBuf[pack.length] = '\0';
	ret = recv(sockfd, sendBuf, pack.length, 0);
	memcpy(recvBuf, sendBuf, pack.length);

	delete[] sendBuf;

	return ret;
}