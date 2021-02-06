#include "pressureClient.h"
#include "tcpWrap.h"

using namespace std;

PressureClient::PressureClient()
{
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	bzero(sock, sizeof(sock));
	epfd = 0;
}

void PressureClient::Start(int clientNum, int postLen)
{
	static struct epoll_event ev, events[EPOLL_SIZE];
	TcpDataWrap clientTcp;
	// map<int, int>fd_id;
	// map<int, int>id_fd;

	int i, connfd, recvNum = 0;
	struct timeval tpstart, tpend;
  	float timeuse;
	char send_buf[postLen + 1];
	for(i = 0; i < postLen; i++)
	{
		send_buf[i] = '1';
	}
	send_buf[postLen] = '\0';

	epfd = epoll_create(EPOLL_SIZE);
	if(epfd < 0) {
		perror("epfd error");
		exit(-1);
	}
	
	gettimeofday(&tpstart, NULL);
	// 先把客户端都起起来
	for(i = 1; i <= clientNum; i++)
	{
		Connect(i);
		// 把自己的id发给服务器
		send(sock[i], &i, sizeof(int), 0);

	}
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
	timeuse /= 1000000;
	printf("conn time %lf s\n", timeuse);

	for(i = 0; i < clientNum; i++)
	{
		ev.data.fd = sock[i];
		cout << "fd:" << sock[i] << endl; 
		ev.events = EPOLLOUT;
    	epoll_ctl(epfd, EPOLL_CTL_ADD, sock[i], &ev);
		// fd_id.insert(pair<int, int>(sock[i], i + 1));
		// id_fd.insert(pair<int, int>(i + 1, sock[i]));
  	}

	gettimeofday(&tpstart, NULL);
	while (true)
	{
		if(recvNum >= clientNum)
			break;
		cout << "recvNum:" << recvNum << endl;
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		for(i = 0; i < epoll_events_count; i++)
		{
			connfd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
            {			
				char *recvBuf = new char[BUF_SIZE];
				memset(recvBuf, 0, BUF_SIZE);
				int tarID = 0;

				clientTcp.recvUnpackData(connfd, &tarID, recvBuf);

				cout << "recv:" << recvBuf << endl;

				recvNum++;
				delfd(epfd, connfd);
				delete[] recvBuf;
			}
			else if(events[i].events & EPOLLOUT)
            {
				// map<int, int>::iterator it;
				// it = fd_id.find(connfd);
				// int toID, nextID, nextconnfd;
				// toID = it -> second + clientNum / 2;
				// nextID = it -> second + 1;
				// nextconnfd = id_fd.find(nextID)->second;

				// cout << "nextID:" << nextID << endl;
				// cout << "nextconnfd:" << nextconnfd << endl;
				cout << "cnnfd:" << connfd << endl;
				clientTcp.sendPackData(connfd, connfd, send_buf, strlen(send_buf));
				ev.events = EPOLLIN;
				ev.data.fd = connfd;
				
				epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ev);
			}
		}
	}
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
	timeuse /= 1000000;
	printf("echo time %lf s\n", timeuse);
	
}

void PressureClient::Connect(int clientNo)
{
	sock[clientNo] = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("sock error");
		exit(-1);
	}

	if(connect(sock[clientNo], (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("connect error");
		exit(-1);
	}
}

void PressureClient::Close()
{
	for(int i = 0; i < EPOLL_SIZE; i++)
	{
		if(sock[i] > 0)
		{
			close(sock[i]);
		}
	}
	close(epfd);
}