#include <iostream>
#include <pthread.h>
using namespace std;
#include <debug.h>
#include <inc.h>
#include <encap_epoll.h>
Epoll::Epoll()
{
	bestop = false;
	CHECK(epollfd = epoll_create(MAXTASK));
}
Epoll::~Epoll()
{
	close(epollfd);

	set<int>::iterator ite;
	for(ite = rfds.begin(); ite != rfds.end(); ite++)
	{
		close(*ite);
		wfds.erase(*ite);
	}
	for(ite = wfds.begin(); ite != wfds.end(); ite++)
	{
		close(*ite);
	}

	DEBUGMSG("Epoll destroyed!");
}
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
	DEBUGMSG("Remove:%d", e.data.fd);
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
	if(e.events & EPOLLOUT)
	{
		wfds.erase(e.data.fd);
	}
}
void Epoll::poll()
{
	int i, nfds;
	while(!bestop)
	{
		nfds = epoll_wait(epollfd, events, MAXTASK, -1);
		if(nfds == -1)
		{

			if(errno == EAGAIN || errno == EINTR)
				continue;
			CHECK(nfds);
			break;
		}
		for(i = 0; i < nfds; i++)
		{
			handle(events[i]);
		}
	}
}

