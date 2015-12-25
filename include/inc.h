#ifndef __INC_H__
#define __INC_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
#define CHECK(x) do{ if((x)==-1) { fprintf(stderr, "%s(%s-%d):%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno)); exit(-1);} }while(0)
#define CHECK2(x) do{ if((x)!=0) { fprintf(stderr, "%s(%s-%d):failed!\n", __FILE__, __FUNCTION__, __LINE__); exit(-1);} }while(0)
#define CHECKMSG(x, msg) do{ if((x)!=0) { fprintf(stderr, "%s(%s-%d):%s\n", __FILE__, __FUNCTION__, __LINE__, msg); exit(-1);} }while(0)
#define ASSERT(x) do{ if(!(x)) { fprintf(stderr, "ASSERT Failed:%s(%s-%d):%s\n", __FILE__, __FUNCTION__, __LINE__, #x); exit(-1);} }while(0)
#define err_quit(format, ...) do { fprintf(stderr, format, ##__VA_ARGS__); fprintf(stderr, "\n"); exit(-1); }while (0)
#define ERR_LEN 256
#define CHECK_THROW(x) do{ if((x)==-1) { char _buf[ERR_LEN];\
		snprintf(_buf, ERR_LEN, "%s(%s-%d):%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));\
		throw std::runtime_error(_buf);} }while(0)
#endif /* __INC_H__*/
