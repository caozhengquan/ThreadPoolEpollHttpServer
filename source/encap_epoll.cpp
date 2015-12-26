/*************************************************************************
    > File Name: encap_epoll.cpp
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/
#include <iostream>
#include <pthread.h>
using namespace std;
#include <debug.h>
#include <inc.h>
#include <encap_epoll.h>

/**
 * 创建epoll内核对象
 */
Epoll::Epoll(int ntask):maxtask(ntask)
{
	bestop = false;
	if(ntask > MAXTASK)
		ntask = MAXTASK;
	CHECK3(ntask > 0);
	events = new epoll_event[ntask];
	CHECK(epollfd = epoll_create(ntask));
}

/**
 * 销毁epoll内核对象。
 * 关闭所有文件描述符号(即所有连接)
 */
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

	delete []events;
	MSG_DEBUG("Epoll destroyed!");
}

/**
 * 向内核中加入文件描述符。
 * 支持同时注册多个事件。
 */
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
	if(exist)							//内核中有该描述符，所以修改
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.data.fd, &event));
	else								//内核中没有该描述符，所以增加
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_ADD, e.data.fd, &event));

	if(e.events & EPOLLIN)			//注册的是读事件
	{
		rfds.insert(e.data.fd);
	}
	if(e.events & EPOLLOUT)		//注册的是写事件
	{
		wfds.insert(e.data.fd);
	}
}

/**
 * 删除一个文件描述符
 * 可以同时删除同一个描述符上的多个事件
 */
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
	if(event.events != 0)			//还有事件，  修改文件描述符
	{
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_MOD, e.data.fd, &event));

	}
	else								//已经没有事件， 删除并且关闭描述符
	{
		CHECK(epoll_ctl(epollfd, EPOLL_CTL_DEL, e.data.fd, &event));
		::close(e.data.fd);
		MSG_DEBUG("Remove:%d", e.data.fd);
	}

	if(e.events & EPOLLIN)			//删除的是读事件
	{
		rfds.erase(e.data.fd);
	}
	if(e.events & EPOLLOUT)		//删除的是写事件
	{
		wfds.erase(e.data.fd);
	}
}

/**
 * 使用epoll_wait等待事件发生
 */
void Epoll::poll()
{
	int i, nfds;
	while(!bestop)
	{
		if(rfds.size()==0 && wfds.size()==0)
			break;
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

