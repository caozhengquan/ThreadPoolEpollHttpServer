/*************************************************************************
    > File Name: inc.h
    > Author: wyg
    > Mail: wyg_0802@126.com
    > Created Time: 2015年12月23日 星期三 16时07分33秒
 ************************************************************************/
#ifndef __INC_H__
#define __INC_H__

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <mqueue.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <debug.h>
#define CHECK(x) do{ if((x)==-1) { MSG_ERR("[%s] %s", #x, strerror(errno)); exit(-1);} }while(0)
#define CHECK2(x) do{ if((x)!=0) { MSG_ERR("[%s] failed!",#x); exit(-1);} }while(0)
#define CHECK3(x) do{ if(!(x))   { MSG_ERR("[%s] failed!",#x); exit(-1);} }while(0)
#define CHECKMSG(x, msg) do{ if(!(x)) { MSG_ERR("[%s] %s\n",#x, msg); exit(-1);} }while(0)

#define err_quit(format, ...) do { MSG_ERR(format, ##__VA_ARGS__); exit(-1); }while (0)
#define ERR_LEN 256
#define CHECK_THROW(x) do{ if((x)==-1) { char _buf[ERR_LEN];\
		snprintf(_buf, ERR_LEN, "%s(%s-%d):%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));\
		throw std::runtime_error(_buf);} }while(0)
#endif /* __INC_H__*/
