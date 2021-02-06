#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <memory>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_IP "127.0.0.2"
#define SERVER_PORT 8888
#define EPOLL_SIZE 10001
#define BUF_SIZE 0xFFFF
#define EXIT "EXIT"
#define CLOSEERROR "Client close with errors."
#define CLOSENORMAL "Client close normally."
#define NOTONLINE "The client is not online."
#define REPEATEDID "The client id is repeated."

typedef struct
{
	int length;
	int tofd;
	
}Packet;

static void addfd( int epollfd, int fd, bool enable_et, bool is_block )
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLERR | EPOLLRDHUP;
	if( enable_et )
		ev.events = EPOLLIN | EPOLLERR | EPOLLET | EPOLLRDHUP;
	
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	
	
	if(is_block)
	{
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)| O_NONBLOCK);
		
	}
	printf("fd added to epoll!\n");
}

static void delfd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	
	printf("delete the fd from epoll!\n");
}

#endif 