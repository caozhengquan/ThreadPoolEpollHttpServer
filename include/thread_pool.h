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
#define DEFUALT_CHILD_MAXTASK 1000

/**
 * 半同步半异步TCP线程池模板
 * 使用单例设计模式
 * 需要一个worker类的对象， 这个worker类必须实现void run(void);方法
 */
template<typename TWorker>
class ThreadPool:public Epoll
{
private:
	//线程函数
	static void* run_child(void *arg);
public:
	virtual ~ThreadPool();
	static ThreadPool *thread_pool_create(unsigned short port, int nthread = 8, string ip = "0.0.0.0", int maxevent = DEFUALT_CHILD_MAXTASK);
	static ThreadPool *get_thread_pool();
private:
	ThreadPool(int nthread, unsigned short port, string ip, int maxevent);
	virtual void handle(epoll_event &e);
	static void handle_sig(int sig);
	void stop_thread(int i);
	void stop_all_thread();
private:
	int pipefd[MAXTHREAD][2];
	static ThreadPool *tp;
	int maxthread;
	TcpServer ts;
	int cur_worker;
};

/**
 * 线程池构造函数
 * 初始化来TCP服务器
 */
template<typename TWorker>
ThreadPool<TWorker>::ThreadPool(int nthread, unsigned short port, string ip, int maxevent):
	Epoll(nthread+1), maxthread(nthread), ts(ip.c_str(), port), cur_worker(0)
{
	maxthread = maxthread > MAXTHREAD ? MAXTHREAD : maxthread;
	CHECK3(maxthread > 0);

	//添加监听socket到epoll中
	epoll_event e;
	e.data.fd = ts.listen_sock.fd;
	e.events = EPOLLIN;
	addfd(e);

	//注册信号处理函数
	CHECK3(signal(SIGINT, handle_sig) != SIG_ERR);

	//创建子线程
	for(int i = 0; i < maxthread; i++)
	{
		//创建父子线程通信的管道
		CHECK(socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd[i]));
		pthread_t tid;
		MSG_DEBUG("create thread %d.\n", i);
		e.data.fd = pipefd[i][0];
		e.events = EPOLLIN;
		addfd(e);
		TWorker *w = new TWorker(pipefd[i][1]);
		CHECK2(pthread_create(&tid, NULL, ThreadPool::run_child, w));
		pthread_detach(tid);
	}
	MSG_IFO("Server started! Listened on [%s]:%d\n", ip.c_str(), port);
}

template<typename TWorker>
ThreadPool<TWorker> *ThreadPool<TWorker>::get_thread_pool()
{
	return tp;
}

/**
 * 停止所有子线程
 */
template<typename TWorker>
void ThreadPool<TWorker>::stop_all_thread()
{
	for(int i = 0; i < maxthread; i++)
		stop_thread(i);
}

/**
 * 停止一个线程。
 * 想线程i发送停止命令
 */
template<typename TWorker>
void ThreadPool<TWorker>::stop_thread(int i)
{
	Msg msg;
	msg.type = Msg::Stop;
	send(pipefd[i][0], &msg, sizeof(msg), 0);
}

/**
 * 信号处理函数
 */
template<typename TWorker>
void ThreadPool<TWorker>::handle_sig(int sig)
{
	if(sig == SIGINT)
	{
		MSG_IFO("Receive signal SIGINT");
		ThreadPool<TWorker> *tp = ThreadPool<TWorker>::get_thread_pool();
		tp->stop_all_thread();
		sleep(3);
		delete tp;
		exit(0);
	}
}

/**
 * 创建线程池
 */
template<typename TWorker>
ThreadPool<TWorker>* ThreadPool<TWorker>::thread_pool_create(
		unsigned short port, int nthread , string ip, int maxevent)
{
	if(tp != NULL)
		return tp;
	if(nthread <= 0)
		return NULL;
	return tp = new ThreadPool<TWorker>(nthread, port, ip, maxevent);

}

/**
 * 线程池唯一实例
 */
template<typename TWorker>
ThreadPool<TWorker> *ThreadPool<TWorker>::tp = NULL;

/**
 * 线程池析构
 */
template<typename TWorker>
ThreadPool<TWorker>::~ThreadPool()
{
	int i;
	for(i = 0; i < maxthread; i++)
		close(pipefd[i][0]);
	tp = NULL;
	MSG_IFO("Thread pool destroyed!");
}

/**
 * 子线程的线程函数
 * 运行TWorker的run方法
 */
template<typename TWorker>
void* ThreadPool<TWorker>::run_child(void *arg)
{
	TWorker *w = (TWorker*)arg;
	w->run();
	return NULL;
}

/**
 * 处理epoll分发下来的事件
 * 可能是监听socket上有连接了
 * 也可能是子线程有消息。
 */
template<typename TWorker>
void ThreadPool<TWorker>::handle(epoll_event &e)
{
	if(e.data.fd == ts.listen_sock.fd)
	{
		Msg msg;
		msg.type = Msg::NewConn;
		CHECK(msg.data.fd = ts.accept().fd);
		MSG_DEBUG("accept a client:%d", msg.data.fd);
		CHECK3(send(pipefd[cur_worker][0], &msg, sizeof(msg), 0) == sizeof(Msg));
		cur_worker = (cur_worker + 1) % maxthread;
	}
	else
		err_quit("not implemented!");
}

#endif /*__THREAD_POOL_H__ */
