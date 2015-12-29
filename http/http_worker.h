/*
 * http_worker.h
 *
 *  Created on: 2015-12-28
 *      Author: root
 */

#ifndef HTTP_WORKER_H_
#define HTTP_WORKER_H_
#include <worker.h>
#include <http/http_server.h>
#include <http/client_data.h>
#include <map>
using namespace std;
class HttpServer;
class ClientData;
class HttpWorker:public Worker
{
public:
	HttpWorker(int ppfd, int maxevent = 64);
	virtual void handle_new_conn(epoll_event &e);
	virtual void handle_conn(epoll_event &e);

private:
	void handle_header(ClientData *cd);
	bool send_response(ClientData *cd);
	int parse_line(char *s, int offset, int len);
	bool parse_req(ClientData *cd);
	bool check_error(ClientData *cd);
	int find_first_char(char *s, int offset, int len, char c);
private:
	map<int, ClientData*> client;

};

#endif /* HTTP_WORKER_H_ */
