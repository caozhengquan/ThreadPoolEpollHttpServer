#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <inc.h>
/**
 * 封装socket需要的文件描述符和地址信息
 */
class Socket
{
public:
	int fd;
	sockaddr_in addr;
};

/**
 * 简单的TCP服务器
 * 在构造函数里开始监听端口
 */
class TcpServer
{
public:
	TcpServer(const char *ip = "0.0.0.0", unsigned short port = 8888);
	~TcpServer();
	Socket listen_sock;
	Socket accept();
};
#endif /*__TCP_SERVER_H__*/
