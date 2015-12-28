/*
 * http_server.cpp
 *
 *  Created on: 2015-12-28
 *      Author: root
 */
#include <http/http_server.h>

HttpServer* HttpServer::hs = NULL;

HttpServer::HttpServer():cf(CONFIG_FILE)
{
	cf.set_default("ip", "0.0.0.0");
	cf.set_default("port", "12345");
	cf.set_default("maxthread", "4");
	cf.set_default("maxevent", "1024");
	cf.read_config();
}
void HttpServer::start_server()
{

	tp = ThreadPool<HttpWorker>::thread_pool_create(
				cf.get_int("port"), cf.get_int("maxthread"), cf.get_string("ip"), cf.get_int("maxevent"));
	tp->poll();
}

HttpServer *HttpServer::get_http_server(void)
{
	if(hs == NULL)
		return new HttpServer();
	else
		return hs;
}


