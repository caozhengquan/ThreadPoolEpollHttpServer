/*************************************************************************
    > File Name: thread_pool.cpp
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2015年12月23日 星期三 16时07分13秒
 ************************************************************************/
#include <iostream>
using namespace std;
#include <thread_pool.h>
#include <debug.h>
#include <pthread.h>
#include <inc.h>

Event::Event(EPOLL_EVENTS e, int fd):pe(e), fd(fd)
{
	cout<<e<<" "<<fd<<endl;
}

Epoll::Epoll()
{
	CHECK(epollfd == epoll_create(MAXTASK));
}
/*************************************************************************/

Worker::Worker(int ppfd)
{
	pipefd = ppfd;
}
void Worker::run()

{
	printf("I am running\n");
}
/*************************************************************************/

void Epoll::addfd(Event e)
{
	bool exist = false;
	epoll_event event;
	event.data.fd = e.fd;
	event.events = e.pe;

	if(rfds.find(e.fd) != rfds.end())
	{
		event.events |= EPOLLIN;
		exist = true;
	}
	if(wfds.find(e.fd) != wfds.end())
	{
		event.events |= EPOLLOUT;
		exist = true;
	}
	if(exist)
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.fd, &event));
	else
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_ADD, e.fd, &event));
	if(e.pe == EPOLLIN)
	{
		rfds.insert(e.fd);
	}
	else if(e.pe == EPOLLOUT)
	{
		wfds.insert(e.fd);
	}
}

void Epoll::delfd(Event e)
{
	epoll_event event;
	event.data.fd = e.fd;
	event.events = 0;

	if(rfds.find(e.fd) != rfds.end())
	{
		event.events |= EPOLLIN;
	}
	if(wfds.find(e.fd) != wfds.end())
	{
		event.events |= EPOLLOUT;
	}
	event.events &= ~e.pe;
	if(event.events != 0)
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.fd, &event));
	else
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_DEL, e.fd, &event));
	if(e.pe == EPOLLIN)
	{
		rfds.erase(e.fd);
	}
	else if(e.pe == EPOLLOUT)
	{
		wfds.erase(e.fd);
	}
}
void Epoll::poll()
{
	int i, nfds;
	while(true)
	{
		nfds = epoll_wait(epollfd, events, MAXTASK, -1);
		if(nfds == -1)
		{
			cout<<"epoll wait fail!"<<endl;
			break;
		}
		for(i = 0; i < nfds; i++)
		{
			if(events[i].events & EPOLLIN)
			{
				handle(Event(EPOLLIN, events[i].data.fd));
			}

			if(events[i].events & EPOLLOUT)
			{
				handle(Event(EPOLLIN, events[i].data.fd));
			}
		}
	}
}
