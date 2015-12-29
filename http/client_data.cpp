/*
 * client_data.cpp
 *
 *  Created on: 2015-12-29
 *      Author: wyg
 */
#include <http/client_data.h>
ClientData::ClientData(int fd)
{
	sock = fd;
	rcv_len = 0;
	req_errno = 0;
	snd_len = 0;
	req_errno = 0;
	file_fd = -1;
	file_size = 0;
	ett_len = 0;
	req_status = PARSE_FIRST_LINE;
}
ClientData::~ClientData()
{
	if(file_fd != -1)
		close(file_fd);
}

