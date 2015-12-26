#ifndef __MSG_H__
#define __MSG_H__

class Msg
{
public:
	enum MsgType{NewConn, Stop}type;
	union Data
	{
		int fd;
		void *ptr;
	}data;
};

#endif
