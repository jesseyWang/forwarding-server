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
	map<int, int>fd_id;
	map<int, int>id_fd;
	map<int, int>::iterator it;

	int i, connfd, recvNum = 0;
	int selfID, toID, toconnfd;
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
	for(i = 0; i < clientNum; i++)
	{
		Connect(i);
	}
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
	timeuse /= 1000000;
	printf("conn time %lf s\n", timeuse);

	for(i = 0; i < clientNum; i++)
	{
		ev.data.fd = sock[i];

		if(i < clientNum / 2)
			ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP | EPOLLERR;
		else
			ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;

    	epoll_ctl(epfd, EPOLL_CTL_ADD, sock[i], &ev);
		fcntl(sock[i], F_SETFL, fcntl(sock[i], F_GETFD, 0)| O_NONBLOCK);
		fd_id.insert(pair<int, int>(sock[i], i + 1));
		id_fd.insert(pair<int, int>(i + 1, sock[i]));
  	}

	gettimeofday(&tpstart, NULL);
	while (true)
	{
		if(recvNum >= clientNum / 2)
			break;
		
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		for(i = 0; i < epoll_events_count; i++)
		{
			connfd = events[i].data.fd;
			it = fd_id.find(connfd);
			selfID = it->second;
			toID = selfID + clientNum / 2;			
			if (events[i].events & EPOLLIN)
			{
				char *recvBuf = new char[BUF_SIZE];
				memset(recvBuf, 0, BUF_SIZE);
				int tarID = 0;

				clientTcp.recvUnpackData(connfd, &tarID, recvBuf);
				cout << "selfID:" << selfID << endl;
				// cout << "recv:" << recvBuf << endl;

				recvNum++;
				delfd(epfd, connfd);
				delete[] recvBuf;
			}
			else if(events[i].events & EPOLLOUT)
            {
				clientTcp.sendPackData(connfd, toID, send_buf, strlen(send_buf));

			}
			else if(events[i].events & EPOLLERR || events[i].events & EPOLLRDHUP)
			{
				Close();
			}
		}
	}
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
	timeuse /= 1000000;
	printf("echo time %lf s\n", timeuse);

	Close();
}

void PressureClient::Connect(int clientNo)
{
	sock[clientNo] = socket(PF_INET, SOCK_STREAM, 0);
	if(sock[clientNo] < 0) {
		perror("sock error");
		exit(-1);
	}

	if(connect(sock[clientNo], (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("connect error");
		exit(-1);
	}
	int selfID = clientNo + 1;
	send(sock[clientNo], &selfID, sizeof(int), 0);
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