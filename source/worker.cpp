/*************************************************************************
    > File Name: worker.cpp
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/

#include <iostream>
#include <pthread.h>
using namespace std;
#include <debug.h>
#include <inc.h>
#include <worker.h>
#include <msg.h>

/**
 * 把与父进程通信的管道描述符加入epoll
 */
Worker::Worker(int ppfd, int maxevent):Epoll(maxevent)
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
	poll();										//开始事件循环
	delete this;									//自我销毁
}

/**
 * Epoll事件首先经过这里，过滤后, 再调用子类方法
 */
void Worker::handle(epoll_event &e)
{
	if(e.data.fd == pipefd)				//主线程有消息
	{

		int ret;
		Msg msg;
		ret = recv(e.data.fd, &msg, sizeof(msg), 0) == sizeof(Msg);
		if(ret <= 0)
		{
			e.events = EPOLLIN | EPOLLOUT;
			delfd(e);
			//close(e.data.fd);
			return;
		}
		if(msg.type == Msg::NewConn)		//新连接
		{

			epoll_event e;
			e.data.fd = msg.data.fd;
			e.events = 0;
			DEBUGMSG("a new conntion! fd:%d", e.data.fd);
			handle_new_conn(e);				//调用子类处理方法
		}
		else if(msg.type == Msg::Stop)	//停止命令， 停止子线程
		{
			bestop = true;
		}
	}
	else
		handle_conn(e);						//其他事件，调用子类处理方法
}

/**
 * 处理一个新连接
 * 这是个虚函数，应该由子类重写。
 */
void Worker::handle_new_conn(epoll_event &e)
{
	DEBUGMSG("thread:%u handle  new conntion!", (unsigned)pthread_self());
	e.events = EPOLLIN;
	addfd(e);
}

/**
 * 处理连接上的数据
 * 这是个虚函数，应该由子类重写。
 */
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
		DEBUGMSG("event EPOLLOUT!");
	}
	else
	{
		e.events = EPOLLIN | EPOLLOUT;
		delfd(e);
		//close(e.data.fd);
		return;
	}
}
