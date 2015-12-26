#ifndef __MSG_H__
#define __MSG_H__
/**
 * 用于主线程和子线程间通信
 * 通知子线程，一个新连接或者结束或者其他
 */
class Msg
{
public:
	enum MsgType{NewConn, Stop}type; //消息类型
	union Data
	{
		int fd;
		void *ptr;
	}data;									//消息数据
};

#endif
