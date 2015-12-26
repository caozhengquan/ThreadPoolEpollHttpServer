/*************************************************************************
    > File Name: worker.h
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/
#ifndef __WORKER_H__
#define __WORKER_H__
#include <string>
#include <set>
#include <map>
using namespace std;

#include <encap_epoll.h>

/**
 * 工作线程，处理连接
 * 这个类应该用来继承，不应该的这个类里处理客户逻辑
 */
class Worker:public Epoll
{
public:
	Worker(int ppfd, int maxevent = 64);
	virtual ~Worker();
	void run();												//线程函数里调用，用来开始个一个worker
	virtual void handle_new_conn(epoll_event &e);		//子类应该重写这个函数，实现自己的逻辑
	virtual void handle_conn(epoll_event &e);			//子类应该重写这个函数，实现自己的逻辑
private:
	virtual void handle(epoll_event &e);				//实现包父类Epoll里的虚函数
	int pipefd;												//与主线程通信的管道
};
#endif
