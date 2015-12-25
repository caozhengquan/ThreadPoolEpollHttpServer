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

ThreadPool *ThreadPool::tp = NULL;
string ThreadPool::ip;
unsigned short ThreadPool::port;
int ThreadPool::maxthread;
ThreadPool::ThreadPool(unsigned short port, string ip /*= "0.0.0.0"*/):ts(ip.c_str(), port)
{
	for(int i = 0; i < maxthread; i++)
	{
		CHECK(pipe(pipefd[i]));
		pthread_t tid;
		printf("create thread %d.\n", i);
		Worker *w = new Worker(pipefd[i][0]);
		CHECK2(pthread_create(&tid, NULL, ThreadPool::run_child, w));
		pthread_detach(tid);
	}
}
ThreadPool::~ThreadPool()
{
	if(tp != NULL)
		delete tp;
}
ThreadPool* ThreadPool::thread_pool_create(unsigned short port, int nthread /* = 8 */, string ip /*= "0.0.0.0"*/)
{
	if(tp != NULL)
		return tp;
	if(nthread <= 0)
		return NULL;
	ThreadPool::ip = ip;
	ThreadPool::port = port;
	ThreadPool::maxthread = nthread;
	return tp = new ThreadPool(port, ip);

}

void* ThreadPool::run_child(void *arg)
{
	Worker *w = (Worker*)arg;
	w->run();
	return NULL;
}

/***********************************************************************************************/

Worker::Worker(int ppfd)
{
	pipefd = ppfd;
}
void Worker::run()
{
	printf("I am running\n");
}
