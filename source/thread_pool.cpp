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



Epoll::Epoll()
{
	CHECK(epollfd = epoll_create(MAXTASK));
}
/*************************************************************************/

Worker::Worker(int ppfd)
{
	pipefd = ppfd;
	epoll_event e;
	e.data.fd = ppfd;
	e.events = EPOLLIN;
	addfd(e);
}
void Worker::run()

{
	printf("I am running\n");
	poll();
}

void Worker::handle(epoll_event &e)
{

}
/*************************************************************************/

void Epoll::addfd(epoll_event &e)
{
	bool exist = false;
	epoll_event event;
	event.data.fd = e.data.fd;
	event.events = e.events;

	if(rfds.find(e.data.fd) != rfds.end())
	{
		event.events |= EPOLLIN;
		exist = true;
	}
	if(wfds.find(e.data.fd) != wfds.end())
	{
		event.events |= EPOLLOUT;
		exist = true;
	}
	if(exist)
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.data.fd, &event));
	else
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_ADD, e.data.fd, &event));
	if(e.events & EPOLLIN)
	{
		rfds.insert(e.data.fd);
	}
	else if(e.events & EPOLLOUT)
	{
		wfds.insert(e.data.fd);
	}
}

void Epoll::delfd(epoll_event &e)
{
	epoll_event event;
	event.data.fd = e.data.fd;
	event.events = 0;

	if(rfds.find(e.data.fd) != rfds.end())
	{
		event.events |= EPOLLIN;
	}
	if(wfds.find(e.data.fd) != wfds.end())
	{
		event.events |= EPOLLOUT;
	}
	event.events &= ~e.events;
	if(event.events != 0)
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.data.fd, &event));
	else
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_DEL, e.data.fd, &event));
	if(e.events & EPOLLIN)
	{
		rfds.erase(e.data.fd);
	}
	else if(e.events & EPOLLOUT)
	{
		wfds.erase(e.data.fd);
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
			handle(events[i]);
		}
	}
}
