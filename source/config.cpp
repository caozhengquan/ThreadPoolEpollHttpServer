/*
 * config.cpp
 *
 *  Created on: 2015-12-27
 *      Author: root
 */
#include <algorithm>
#include <config.h>
using namespace std;

/**
 * 构造器
 * 读取配置文件，解析数据，名字不区分大小写，值区分大小写
 * 配置文件样例：
 * # this is a comment!
 * a = 3
 * ip = 0.0.0.0
 * port = 12345 #TCP Listen port
 * thread_num 5
 *
 */
Config::Config(const char *f):config_file(f)
{
}

void Config::read_config(void)
{
	ifstream fin(config_file.c_str());
	CHECKMSG(fin.is_open(), "config file open failed!");
	string s;
	int cnt = 0;
	while(true)
	{
		cnt++;
		getline(fin, s);
		if(!fin)
			break;

		trim(s);
		if(s.size() == 0 || (s.c_str())[0] == '#')
			continue;
		if((int)s.find('=') == -1 || s.find('=') == s.size())
		{
			MSG_ERR("Bad config line:%d-->%s", cnt, s.c_str());
			continue;
		}
		MSG_DEBUG("%s", s.c_str());
		string name = s.substr(0, s.find('='));
		string value = s.substr(s.find('=')+1, s.size());
		trim(name);
		trim(value);
		config[tolower(name)] = value;
	}
		/*map<string, string>::iterator ite;
		for(ite = config.begin(); ite != config.end(); ite++)
			cout<<ite->first<<" "<<ite->second<<endl;
		*/
	fin.close();
}
/**
 * 由名字s得到一个int值
 */
int Config::get_int(const char *name)
{
	string &str = find(name);
	return atoi(str.c_str());
}

/**
 * 由名字s得到一个string值
 */
string Config::get_string(const char *name)
{
	return find(name);
}

/**
 * 设置name的默认值为value
 */
void  Config::set_default(string name, string value)
{
	config[name] = value;
}

/**
 * 查找由配置文件中的值
 */
string& Config::find(const char *s)
{
	string str(s);
	trim(str);
	tolower(str);
	map<string, string>::iterator ite;
	CHECKMSG( (ite = config.find(str)) != config.end(), "Bad config variable!");
	return ite->second;
}

/**
 * 除去s中注释 开始与结束如空白(回车 空格 制表符)
 */
void Config::trim(string &s)
{
	if((int)s.find_last_of('#') != -1)
		s.erase(s.find_last_of('#'));
	s.erase(s.find_last_not_of('\n')+1);
	s.erase(0, s.find_first_not_of(' '));
	s.erase(0, s.find_first_not_of('\t'));
	s.erase(s.find_last_not_of(' ')+1);
	s.erase(s.find_last_not_of('\t')+1);
}

/**
 * 将s转为小写
 */
string& Config::tolower(string &s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

