#ifndef __WORKER_H__
#define __WORKER_H__
#include <string>
#include <set>
#include <map>
using namespace std;

#include <encap_epoll.h>
class Worker:public Epoll
{
public:
	Worker(int ppfd);
	virtual ~Worker();
	void run();
	virtual void handle_new_conn(epoll_event &e);
	virtual void handle_conn(epoll_event &e);
private:
	virtual void handle(epoll_event &e);
	int pipefd;
};
#endif
