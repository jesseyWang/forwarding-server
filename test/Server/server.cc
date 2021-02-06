#include <iostream>
#include "server.h"
#include "tcpWrap.h"

using namespace std;

TcpDataWrap serverTcp;

Server::Server(){
	
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	listener = 0;
	epfd = 0;
}


void Server::Init() {
	cout << "Init Server..." << endl;

	listener = socket(PF_INET, SOCK_STREAM, 0);

	if(listener < 0) 
	{ 
		perror("listener"); 
		exit(-1);
	}

	if( bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind error");
		exit(-1);
	}

	int ret = listen(listener, EPOLL_SIZE);
	if(ret < 0) {
		perror("listen error");
		exit(-1);
	}
	cout << "Start to listen: " << SERVER_IP << endl;
	
	epfd = epoll_create (EPOLL_SIZE);
	if(epfd < 0) {
		perror("epfd error");
		exit(-1);
	}

	addfd(epfd, listener, true, true);
}

void Server::Close() {
	close(listener);
	close(epfd);
}

int Server::ForwardingMessage(int clientfd)
{
	int recvret, sendret, toID = 0;
	char * recvBuf = new char[BUF_SIZE];
	map<int, int>::iterator it;

	memset(recvBuf, 0, BUF_SIZE);
	recvret = serverTcp.recvUnpackData(clientfd, &toID, recvBuf);

	if(recvret == 0)
	{
		delfd(epfd, clientfd);
		cout << CLOSENORMAL << endl;
	}
	else if(recvret < 0)
	{
		if(errno != EAGAIN || errno != EINTR)
		{
			delfd(epfd, clientfd);
			cout << CLOSEERROR << endl;
		}
	}

	it = idTofdMap.find(toID);
	if(it != idTofdMap.end())
	{
		sendret = serverTcp.sendPackData(it->second, toID, recvBuf, strlen(recvBuf));
		
		if(sendret < 0)
		{
			return sendret;
		}
	}
	else
	{
		cout << NOTONLINE << endl;
		memset(recvBuf, 0, BUF_SIZE);
		memcpy(recvBuf, NOTONLINE, strlen(NOTONLINE));
		sendret = serverTcp.sendPackData(clientfd, toID, recvBuf, strlen(recvBuf));

		if(sendret < 0)
		{
			return sendret;
		}
	}

	delete[] recvBuf;

	return 0;
}

void Server::Start() {
	static struct epoll_event events[EPOLL_SIZE];

	Init();

	while(1)
	{
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		if(epoll_events_count < 0) {
			perror("epoll failure");
			break;
		}
	
		for(int i = 0; i < epoll_events_count; ++i)
		{
			int sockfd = events[i].data.fd;
			if(sockfd == listener)
			{

				struct sockaddr_in client_address;
				int client_id;
				map<int, int>::iterator it;

				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = accept( listener, ( struct sockaddr* )&client_address, &client_addrLength );
				
				cout << "Client connection from: "
				<< inet_ntoa(client_address.sin_addr) << ":"
				<< ntohs(client_address.sin_port) << ", clientfd = "
				<< clientfd << endl;

				int ret = recv(clientfd, &client_id, sizeof(int), 0);
				if(ret < 0)
				{
					perror("recv error");
				}
				else
				{
					// 供客户端为终端输入时使用，压力测试时无需此段代码
					// if((it = idTofdMap.find(client_id)) != idTofdMap.end())
					// {
					// 	ret = serverTcp.sendPackData(clientfd, clientfd, REPEATEDID, strlen(REPEATEDID));
					// 	if(ret < 0)
					// 	{
					// 		perror("send repeated msg error:");
					// 	}
					// 	break;
					// }
					// else
					// {
						idTofdMap.insert(pair<int, int>(client_id, clientfd));
					// }				
				}			
				
				addfd(epfd, clientfd, true, true);

				cout << "Add new clientfd = " << clientfd << " to epoll" << endl;
				cout << "Welcome." << endl;
				
			}
			else {
				if(events[i].events & EPOLLERR || events[i].events & EPOLLRDHUP)
				{
					delfd(epfd, events[i].data.fd);
					cout << CLOSEERROR << endl;
					continue;
				}
				else
				{
					int ret = ForwardingMessage(sockfd);
					if(ret < 0) {
						perror("error");
						Close();
						exit(-1);
					}
				}
			}
		}
	}
	Close();
}