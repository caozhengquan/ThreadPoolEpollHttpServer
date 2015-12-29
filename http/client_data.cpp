/*
 * client_data.cpp
 *
 *  Created on: 2015-12-29
 *      Author: wyg
 */
#include <http/client_data.h>
ClientData::ClientData(int fd):sock(fd), req_errno(0), req_status(PARSE_FIRST_LINE), rep_errno(0), file_size(0)
{
}

