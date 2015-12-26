/*************************************************************************
    > File Name: encap_epoll.h
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/
#ifndef __ENCAP_EPOLL_H__
#define __ENCAP_EPOLL_H__
#include <string>
#include <set>
#include <map>
#include <sys/epoll.h>
using namespace std;

#define MAXTASK 1000

/**
 * 这是一个虚基类
 * 封装了epoll，可以添加删除描述符
 * 可以分发事件。
 */
class Epoll
{
public:
	Epoll(int ntask = 16);
	virtual ~Epoll();
	void addfd(epoll_event &e);
	void delfd(epoll_event &e);
	void poll();
	virtual void handle(epoll_event &e) = 0;		//子类实现处理事件的方法
protected:
	bool bestop;
private:
	set<int> rfds;										//所有读事件文件描述符
	set<int> wfds;										//所有写事件文件描述符
	int epollfd;
	epoll_event *events;
	int maxtask;
};
#endif /*__ENCAP_EPOLL_H__*/
