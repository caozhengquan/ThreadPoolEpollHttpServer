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

using namespace std;
int main()
{
	Config cf("http.conf");
	cf.set_default("ip", "0.0.0.0");
	cf.set_default("port", "12345");
	cf.set_default("maxthread", "4");
	cf.set_default("maxevent", "1024");
	cf.read_config();
	ThreadPool<Worker> *th = ThreadPool<Worker>::thread_pool_create(
			cf.get_int("port"), cf.get_int("maxthread"), cf.get_string("ip"), cf.get_int("maxevent"));
	th->poll();

	return 0;
}
