/*************************************************************************
    > File Name: main.cpp
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/
#include <config.h>
#include <iostream>
#include <thread_pool.h>
#include <inc.h>
#include <debug.h>
#include <http/http_server.h>
using namespace std;
int main()
{
	HttpServer *hs = HttpServer::get_http_server();
	hs->start_server();
	return 0;
}
