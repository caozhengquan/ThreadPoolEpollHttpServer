/*
 * client_data.h
 *
 *  Created on: 2015-12-29
 *      Author: wyg
 */

#ifndef CLIENT_DATA_H_
#define CLIENT_DATA_H_

#define MAX_RCV_SIZE 1024
#define MAX_SND_SIZE 1024
#include <string>
#include <map>
using namespace std;
enum Status {PARSE_FIRST_LINE, PARSE_HEADER, HANDLE_HEADER, PARSE_ENTITY, BUILD_RESPONSE, SEND_DATA, SEND_FILE};
class ClientData
{
public:

	ClientData(int fd);
	~ClientData();
	int sock;

	//请求
	char rcv_buf[MAX_RCV_SIZE];
	int rcv_len;
	char *ett_buf;
	int ett_len;
	int req_errno;
	Status req_status;
	map<string, string> req_info;
	//响应
	char snd_buf[MAX_SND_SIZE];
	int snd_len;
	Status rep_status;
	int rep_errno;
	int file_fd;
	unsigned file_size;
};
#endif /* CLIENT_DATA_H_ */
