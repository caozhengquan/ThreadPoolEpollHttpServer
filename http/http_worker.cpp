/*
 * http_worker.cpp
 *
 *  Created on: 2015-12-28
 *      Author: root
 */
#include <iostream>
using namespace std;
#include <http/http_worker.h>
#include <worker.h>
#include <debug.h>
#include <inc.h>

HttpWorker::HttpWorker(int ppfd, int maxevent):Worker(ppfd, maxevent)
{

}
/**
 * 处理一个新连接
 */
void HttpWorker::handle_new_conn(epoll_event &e)
{
	MSG_DEBUG("Http Worker thread:%u handle  new conntion!", (unsigned)pthread_self());
	e.events = EPOLLIN;
	addfd(e);
}

/**
 * 处理连接上的数据
 */
void HttpWorker::handle_conn(epoll_event &e)
{
	MSG_DEBUG("Http Worker thread:%u handle client!", (unsigned)pthread_self());
	if(e.events & EPOLLIN)
	{
		char data[512];
		int ret = recv(e.data.fd, data, 512, 0);
		if(ret <= 0)
		{
			e.events = EPOLLIN | EPOLLOUT;
			delfd(e);
			//close(e.data.fd);
			return;
		}
		data[ret] = 0;
		cout<<data;
		char rep[] = "Hello, I handled you!\n";
		send(e.data.fd, rep, sizeof(rep), 0);
	}
	else
	if(e.events & EPOLLOUT)
	{
		MSG_DEBUG("event EPOLLOUT!");
	}
	else
	{
		e.events = EPOLLIN | EPOLLOUT;
		delfd(e);
		//close(e.data.fd);
		return;
	}
}

