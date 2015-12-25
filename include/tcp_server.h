#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <inc.h>
class Socket
{
public:
	int fd;
	sockaddr_in addr;
};
class TcpServer
{
public:
	TcpServer(const char *ip = "0.0.0.0", unsigned short port = 8888);
	~TcpServer();
	Socket listen_sock;
	Socket accept();
};
#endif /*__TCP_SERVER_H__*/
