#include <iostream>
#include "client.h"
#include "tcpWrap.h"

using namespace std;

Client::Client(){
	
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	sock = 0;
	isClientwork = true;	
	epfd = 0;
}

void Client::Connect() {
	cout << "Connect Server: " << SERVER_IP << " : " << SERVER_PORT << endl;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("sock error");
		exit(-1);
	}

	if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("connect error");
		exit(-1);
	}

	epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0) {
		perror("epfd error");
		exit(-1);
	}
	
	addfd(epfd, sock, true, true);
	addfd(epfd, STDIN_FILENO, true, false);
}

void Client::Close() {
	close(sock);
	close(epfd);
}

void Client::Start(char* toID, char* selfID) {

	int m_id = atoi(selfID);
	static struct epoll_event events[2];
	char buf[BUF_SIZE];
	TcpDataWrap clientTcp;
	
	Connect();

	// 把自己的id发给服务器
	send(sock, &m_id, sizeof(int), 0);
	
	while(isClientwork){
		int epoll_events_count = epoll_wait( epfd, events, 2, -1 );

		for(int i = 0; i < epoll_events_count ; ++i)
		{
			if(events[i].data.fd == sock)
			{
				char *recvBuf = new char[BUF_SIZE];
				memset(recvBuf, 0, BUF_SIZE);
				int tarID = 0;
				clientTcp.recvUnpackData(sock, &tarID, recvBuf);
				if(strncasecmp(recvBuf, REPEATEDID, strlen(REPEATEDID)) == 0)
				{
					cout << REPEATEDID << endl;
					isClientwork = 0;
				}
				else
				{
					cout << "Client recv:" << recvBuf << endl;
				}
								
				delete[] recvBuf;					
			}
			else if(events[i].data.fd == STDIN_FILENO)
			{			
				fgets(buf, BUF_SIZE, stdin);
				if(strncasecmp(buf, EXIT, strlen(EXIT)) == 0){
					isClientwork = 0;
				}
				clientTcp.sendPackData(sock, atoi(toID), buf, strlen(buf));
	
			}	
		}			
	}

	Close();
}