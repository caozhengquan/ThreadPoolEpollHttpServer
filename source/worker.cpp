#include <iostream>
#include <pthread.h>
using namespace std;
#include <debug.h>
#include <inc.h>
#include <worker.h>
#include <msg.h>

Worker::Worker(int ppfd)
{
	pipefd = ppfd;
	epoll_event e;
	e.data.fd = ppfd;
	e.events = EPOLLIN;
	addfd(e);
}
Worker::~Worker()
{
	close(pipefd);
	DEBUGMSG("worker destroyed!");
}
void Worker::run()

{
	DEBUGMSG("I am running...");
	poll();
	delete this;
}

void Worker::handle(epoll_event &e)
{
	if(e.data.fd == pipefd)
	{

		int ret;
		Msg msg;
		ret = recv(e.data.fd, &msg, sizeof(msg), 0) == sizeof(Msg);
		if(ret < 0)
		{
			e.events = EPOLLIN | EPOLLOUT;
			delfd(e);
			return;
		}
		if(msg.type == Msg::NewConn)
		{

			epoll_event e;
			e.data.fd = msg.data.fd;
			e.events = 0;
			DEBUGMSG("a new conntion! fd:%d", e.data.fd);
			handle_new_conn(e);
		}
		else if(msg.type == Msg::Stop)
		{
			bestop = true;
		}
	}
	else
		handle_conn(e);
}
void Worker::handle_new_conn(epoll_event &e)
{
	DEBUGMSG("thread:%u handle  new conntion!", (unsigned)pthread_self());
	e.events = EPOLLIN;
	addfd(e);
}
void Worker::handle_conn(epoll_event &e)
{
	DEBUGMSG("thread:%u handle client!", (unsigned)pthread_self());
	if(e.events & EPOLLIN)
	{
		char data[512];
		int ret = recv(e.data.fd, data, 512, 0);
		if(ret <= 0)
		{
			e.events = EPOLLIN | EPOLLOUT;
			delfd(e);
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

	}
	else
	{
		e.events = EPOLLIN | EPOLLOUT;
		delfd(e);
		return;
	}
}
