/*
 * http_server.h
 *
 *  Created on: 2015-12-28
 *      Author: root
 */


#include <inc.h>
#include <debug.h>
#include <config.h>
#include <http/http_worker.h>
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__
#define CONFIG_FILE "http.conf"
#include <thread_pool.h>
class HttpWorker;
class HttpServer
{
public:

	void start_server(void);
	static HttpServer *get_http_server(void);
	static map<int, string> errors;
	Config &get_config(void);
private:
	HttpServer();
private:
	Config cf;
	ThreadPool<HttpWorker> *tp;
	static HttpServer *hs;

};
#endif /*__HTTP_SERVER_H__*/
