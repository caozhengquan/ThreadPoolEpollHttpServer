/*
 * http_worker.cpp
 *
 *  Created on: 2015-12-28
 *      Author: root
 */
#include <iostream>
using namespace std;
#include <http/http_worker.h>
#include <worker.h>
#include <debug.h>
#include <inc.h>
#include <http/client_data.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>
using namespace std;

#include <string_util.h>


HttpWorker::HttpWorker(int ppfd, int maxevent):Worker(ppfd, maxevent)
{

}
/**
 * 处理一个新连接
 */
void HttpWorker::handle_new_conn(epoll_event &e)
{
	MSG_DEBUG("Http Worker thread:%u handle  new conntion!", (unsigned)pthread_self());

	ASSERT(client.find(e.data.fd) == client.end());
	ClientData *cd = new ClientData(e.data.fd);
	client[e.data.fd] = cd;
	e.events = EPOLLIN;
	addfd(e);
	MSG_IFO("%d clinum:%d", e.data.fd, client.size());
}

/**
 * 处理连接上的数据
 */
void HttpWorker::handle_conn(epoll_event &e)
{
	MSG_DEBUG("Http Worker thread:%u handle client!", (unsigned)pthread_self());
	if(e.events & EPOLLIN)
	{
		ASSERT(client.find(e.data.fd)!=client.end());
		ClientData *cd = client[e.data.fd];
		int ret = recv(e.data.fd, cd->rcv_buf+cd->rcv_len, MAX_RCV_SIZE - cd->rcv_len, 0);
		if(ret >= 0)
			cd->rcv_len += ret;
		if(ret <= 0 || !parse_req(cd))
		{
			e.events = EPOLLIN | EPOLLOUT;
			delfd(e);
			delete client[e.data.fd];
			client.erase(e.data.fd);
			//close(e.data.fd);
			return;
		}
		//char rep[] = "Hello, I handled you!\n";
		//send(e.data.fd, rep, sizeof(rep), 0);
	}
	else
	if(e.events & EPOLLOUT)
	{
		MSG_DEBUG("event EPOLLOUT!");
		ASSERT(client.find(e.data.fd)!=client.end());
		ClientData *cd = client[e.data.fd];
		send_response(cd);
	}
	else
	{
		e.events = EPOLLIN | EPOLLOUT;
		delfd(e);
		delete client[e.data.fd];
		client.erase(e.data.fd);
		close(e.data.fd);
		return;
	}
}

int  HttpWorker::parse_line(char *s, int offset, int len)
{
	int i;
	for(i = offset; i < len-1; i++)							 //分捡行
	{
		if((s[i] == '\r') && (s[i+1] == '\n'))
			return i;
	}
	return -1;
}
int HttpWorker::find_first_char(char *s, int offset, int len, char c)
{
	int i;
	for(i = offset; i < len; i++)
		if(s[i] == c)
			return i;
	return -1;
}

bool HttpWorker::check_error(ClientData *cd)
{
	if(cd->req_errno != 0)
	{
		snprintf(cd->snd_buf, MAX_SND_SIZE, "%s %d %s\r\n\r\n", cd->req_info["VER"].c_str(), cd->req_errno, HttpServer::errors[cd->req_errno].c_str());
		cd->snd_len = strlen(cd->snd_buf);
		cd->req_status = SEND_DATA;
		epoll_event e;
		e.data.fd = cd->sock;
		e.events = EPOLLOUT;
		addfd(e);
		cd->req_errno = 0;
		e.events = EPOLLIN;
		delfd(e);
		return false;
	}
	return true;
}
void HttpWorker::handle_header(ClientData *cd)
{
	if(cd->req_info["METHOD"] == "GET")
	{
		const string &cwd = HttpServer::get_http_server()->get_config().get_string("cwd");
		string file = cwd+cd->req_info["URL"];
		MSG_DEBUG("request file:%s", file.c_str());
		struct stat file_stat;
		if(stat(file.c_str(), &file_stat) == -1)
		{
			cd->req_errno = 404;
			return;
		}
		if((cd->file_fd = open(file.c_str(), O_RDONLY)) == -1)
		{
			cd->req_errno = 403;
			return;
		}
		snprintf(cd->snd_buf, MAX_SND_SIZE, "%s %d ok\r\n", cd->req_info["VER"].c_str(), 200);
		int n = strlen(cd->snd_buf);
		snprintf(cd->snd_buf + n, MAX_SND_SIZE - n, "Content-type:text/plain\r\n");

		n = strlen(cd->snd_buf);
		snprintf(cd->snd_buf + n, MAX_SND_SIZE - n, "Content-length:%u\r\n", (unsigned int)file_stat.st_size);

		n = strlen(cd->snd_buf);
		snprintf(cd->snd_buf+n, MAX_SND_SIZE - n, "\r\n");
		n = strlen(cd->snd_buf);
		cd->snd_len = n;
		cd->file_size = (unsigned int)file_stat.st_size;
		cd->req_status = SEND_DATA;
		epoll_event e;
		e.data.fd = cd->sock;
		e.events = EPOLLOUT;
		addfd(e);

		e.events = EPOLLIN;
		delfd(e);
	}
}

bool HttpWorker::send_response(ClientData *cd)
{
	bool stop = false;
	while(check_error(cd) && !stop)
	{
		switch(cd->req_status)
		{
		case SEND_FILE:
		{
			int ret = read(cd->file_fd, cd->snd_buf + cd->snd_len, MAX_SND_SIZE- cd->snd_len);
			if (ret <= 0 &&  MAX_SND_SIZE- cd->snd_len != 0)
			{
				cd->file_size = 0;
				close(cd->file_fd);
				cd->file_fd = -1;
			}
			else
			{
				cd->file_size -= ret;
				cd->snd_len += ret;
				MSG_DEBUG("send:%d bytes", cd->snd_len);
				if (cd->file_size == 0)
					close(cd->file_fd);
			}
			cd->req_status = SEND_DATA;
			stop = true;
			break;
		}
		case SEND_DATA:
		{
			int ret = send(cd->sock, cd->snd_buf, cd->snd_len, 0);
			if (ret <= 0 && cd->snd_len != 0)
			{
				epoll_event e;
				e.data.fd = cd->sock;
				e.events = EPOLLIN | EPOLLOUT;
				delfd(e);
				delete client[e.data.fd];
				client.erase(e.data.fd);
				stop = true;
				break;
			}
			memmove(cd->snd_buf, cd->snd_buf + ret, cd->snd_len - ret);
			cd->snd_len -= ret;
			if (cd->snd_len == 0 && cd->file_size != 0)
			{
				cd->req_status = SEND_FILE;
				break;
			}
			if (cd->snd_len == 0)
			{
				epoll_event e;
				e.data.fd = cd->sock;
				e.events = EPOLLOUT;
				delfd(e);
				delete client[e.data.fd];
				client.erase(e.data.fd);
				stop = true;
				break;
			}
		}
		default:
			MSG_DEBUG("Un handled status!");
			break;
		}
	}
	return true;
}
bool HttpWorker::parse_req(ClientData *cd)
{
	if(cd->rcv_len == 0)
		return true;
	int p = 0;
	int ed = 0;
	vector<string> tmp;
	bool stop = false;
	while(check_error(cd) && !stop)
	{
		switch(cd->req_status)
		{
		case PARSE_FIRST_LINE:
		{
			ed = parse_line(cd->rcv_buf, p, cd->rcv_len);
			if(ed == -1)
			{
				stop = true;

				break;
			}
			tmp.clear();
			split(string(cd->rcv_buf+p, cd->rcv_buf+ed), tmp, " ");
			if(tmp.size()!=3)
			{
				cd->req_errno = -1;
				break;
			}
			cd->req_info["METHOD"] = tmp[0];
			cd->req_info["URL"] = tmp[1];
			cd->req_info["VER"] = tmp[2];
			cd->req_status = PARSE_HEADER;
			p = ed + 2;
		}
		case PARSE_HEADER:
		{
			ed = parse_line(cd->rcv_buf, p, cd->rcv_len);
			if(ed == -1)
			{
				stop = true;
				break;
			}
			if(p == ed)
			{
				p = ed + 2;
				if(cd->req_info.find("CONTENT-LENGTH") != cd->req_info.end())
				{
					cd->ett_len = atoi(cd->req_info["CONTENT-LENGTH"].c_str());
					cd->req_status = PARSE_ENTITY;

				}
				else
					cd->req_status = HANDLE_HEADER;
				break;
			}
			split(string(cd->rcv_buf + p, cd->rcv_buf + ed), tmp, ":");
			int fc = find_first_char(cd->rcv_buf, p, ed, ':');
			if (fc == -1)
			{
				cd->req_errno = -1;
				break;
			}
			string name = string(cd->rcv_buf+p, cd->rcv_buf+fc);
			cd->req_info[strtoupper(name)] = string(cd->rcv_buf+fc+1, cd->rcv_buf+ed);
			p = ed + 2;
			break;
		}
		case HANDLE_HEADER:
		{
			map<string, string>::iterator ite;
			//for(ite = cd->req_info.begin(); ite != cd->req_info.end(); ite++)
			//	cout<<ite->first<<":"<<ite->second<<endl;
			handle_header(cd);
			stop = true;
			break;
		}
		case PARSE_ENTITY:
		{
			if(cd->ett_len >0)
			{
				int n = p + cd->ett_len < cd->rcv_len ? cd->ett_len : cd->rcv_len - p;
				p += n;
			}
			if(cd->ett_len == 0)
				cd->req_status = HANDLE_HEADER;
			break;
		}
		default:
			MSG_DEBUG("Un handled status!");
			break;
		}
	}

	if((p == 0) && (cd->rcv_len == MAX_RCV_SIZE))				//
		return false;
	if(p != 0)
	{
		memmove(cd->rcv_buf, cd->rcv_buf + p, cd->rcv_len - p);
		cd->rcv_len -= p;
	}
	return true;
}


