/*
 * http_worker.h
 *
 *  Created on: 2015-12-28
 *      Author: root
 */

#ifndef HTTP_WORKER_H_
#define HTTP_WORKER_H_
#include <worker.h>
class HttpServer;
class HttpWorker:public Worker
{
public:
	HttpWorker(int ppfd, int maxevent = 64);
	virtual void handle_new_conn(epoll_event &e);
	virtual void handle_conn(epoll_event &e);
};

#endif /* HTTP_WORKER_H_ */
