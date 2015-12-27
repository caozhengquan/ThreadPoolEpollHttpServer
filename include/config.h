/*
 * config.h
 *
 *  Created on: 2015-12-27
 *      Author: wyg
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <inc.h>
#include <debug.h>

using namespace std;

class Config
{
public:
	Config(const char *file);
	int get_int(const char *s);
	string get_string(const char *s);
	void set_default(string name, string value);
	void read_config(void);
private:
	void trim(string &s);
	string &tolower(string &s);
	string &find(const char *s);
	map<string, string> config;
	string config_file;
};
#endif /* CONFIG_H_ */
