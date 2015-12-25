/*************************************************************************
    > File Name: thread_pool.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2015年12月23日 星期三 16时12分58秒
 ************************************************************************/
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <string>
#include <set>
#include <map>
using namespace std;
#include <tcp_server.h>
#define MAXTHREAD 20
#define MAXTASK 1000
enum PollEvent{READ, WRITE, NEWCONN};
class Event
{
public:
	Event(PollEvent e):pe(e){}
	PollEvent pe;
};
class Epoll
{
private:
	set<int> rfds;
	set<int> wfds;
};

class ThreadPool
{
private:
	static void* run_child(void *arg);

public:
	~ThreadPool();
	static ThreadPool *thread_pool_create(unsigned short port, int nthread = 8, string ip = "0.0.0.0");
private:
	ThreadPool(unsigned short port, string ip /*= "0.0.0.0"*/);
	int pipefd[MAXTHREAD][2];
	static ThreadPool *tp;
	static string ip;
	static unsigned short port;
	static int maxthread;
	TcpServer ts;
};

class Worker
{
public:
	Worker(int ppfd);
	void run();
private:
	int pipefd;
};
#endif /*__THREAD_POOL_H__ */
