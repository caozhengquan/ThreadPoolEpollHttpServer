#ifndef __ENCAP_EPOLL_H__
#define __ENCAP_EPOLL_H__
#include <string>
#include <set>
#include <map>
#include <sys/epoll.h>
using namespace std;

#define MAXTASK 1000

class Epoll
{
public:
	Epoll();
	virtual ~Epoll();
	void addfd(epoll_event &e);
	void delfd(epoll_event &e);
	void poll();
	virtual void handle(epoll_event &e) = 0;
protected:
	bool bestop;
private:
	set<int> rfds;
	set<int> wfds;
	int epollfd;
	epoll_event events[MAXTASK];
};
#endif /*__ENCAP_EPOLL_H__*/
