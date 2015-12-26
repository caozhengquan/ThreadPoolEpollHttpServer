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
#include <debug.h>
#include <worker.h>
#include <encap_epoll.h>
#include <msg.h>

#define MAXTHREAD 20

template<typename TWorker>
class ThreadPool:public Epoll
{
private:
	static void* run_child(void *arg);

public:
	~ThreadPool();

	static ThreadPool *thread_pool_create(unsigned short port, int nthread = 8, string ip = "0.0.0.0");
	static ThreadPool *get_thread_pool();
private:
	virtual void handle(epoll_event &e);
	ThreadPool(int nthread, unsigned short port, string ip /*= "0.0.0.0"*/);
	static void handle_sig(int sig);
	void stop_thread(int i);
	void stop_all_thread();
	int pipefd[MAXTHREAD][2];
	static ThreadPool *tp;
	int maxthread;
	TcpServer ts;
	int cur_worker;
};

template<typename TWorker>
ThreadPool<TWorker>::ThreadPool(int nthread, unsigned short port, string ip):maxthread(nthread), ts(ip.c_str(), port),  cur_worker(0)
{
	epoll_event e;
	e.data.fd = ts.listen_sock.fd;
	e.events = EPOLLIN;
	addfd(e);
	CHECK3(signal(SIGINT, handle_sig) != SIG_ERR);
	for(int i = 0; i < maxthread; i++)
	{
		CHECK(socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd[i]));
		pthread_t tid;
		printf("create thread %d.\n", i);
		TWorker *w = new TWorker(pipefd[i][1]);
		CHECK2(pthread_create(&tid, NULL, ThreadPool::run_child, w));
		pthread_detach(tid);
	}
}

template<typename TWorker>
ThreadPool<TWorker> *ThreadPool<TWorker>::get_thread_pool()
{
	return tp;
}

template<typename TWorker>
void ThreadPool<TWorker>::stop_all_thread()
{
	for(int i = 0; i < maxthread; i++)
		stop_thread(i);
}
template<typename TWorker>
void ThreadPool<TWorker>::stop_thread(int i)
{
	Msg msg;
	msg.type = Msg::Stop;
	send(pipefd[i][0], &msg, sizeof(msg), 0);
}
template<typename TWorker>
void ThreadPool<TWorker>::handle_sig(int sig)
{
	if(sig == SIGINT)
	{
		cout<<"Thread pool exited!"<<endl;
		ThreadPool<TWorker>::get_thread_pool()->stop_all_thread();
		sleep(1);
		exit(0);
	}
}
template<typename TWorker>
ThreadPool<TWorker>* ThreadPool<TWorker>::thread_pool_create(unsigned short port, int nthread /* = 8 */, string ip /*= "0.0.0.0"*/)
{
	if(tp != NULL)
		return tp;
	if(nthread <= 0)
		return NULL;
	return tp = new ThreadPool<TWorker>(nthread, port, ip);

}

template<typename TWorker>
ThreadPool<TWorker> *ThreadPool<TWorker>::tp = NULL;

template<typename TWorker>
ThreadPool<TWorker>::~ThreadPool()
{
	if(tp != NULL)
		delete tp;
	int i;
	for(i = 0; i < maxthread; i++)
		close(pipefd[i][0]);
	DEBUGMSG("Thread Poll destroyed!");
}

template<typename TWorker>
void* ThreadPool<TWorker>::run_child(void *arg)
{
	TWorker *w = (TWorker*)arg;
	w->run();
	return NULL;
}
template<typename TWorker>
void ThreadPool<TWorker>::handle(epoll_event &e)
{
	if(e.data.fd == ts.listen_sock.fd)
	{
		Msg msg;
		msg.type = Msg::NewConn;
		CHECK(msg.data.fd = ts.accept().fd);
		DEBUGMSG("accept a client:%d", msg.data.fd);
		CHECK3(send(pipefd[cur_worker][0], &msg, sizeof(msg), 0) == sizeof(Msg));
		cur_worker = (cur_worker + 1) % maxthread;
	}
}

#endif /*__THREAD_POOL_H__ */
