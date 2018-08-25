#pragma once
#ifndef __LOG_H_
#define __LOG_H_

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <iostream>
#include <glog/logging.h>
#include <string>
#include "inifile.h"

using namespace std;
using namespace inifile;

/*
 * 改造成单例模式; 只产生一个实例
 */

class CGlog
{
private:
	CGlog() { };
	CGlog(const CGlog&) = delete;
	CGlog& operator= (const CGlog &) = delete;
	~CGlog() {};

public:

	static CGlog* GetInstance()
	{
		static CGlog instance; // 局部静态变量
		return &instance;
	}

	char * GetConfigPath();
	int InitGlog();
	void CloseGlog();

private:
	char* ConfigPath = (char*)"./runlog_config.ini";

};

#undef GOOGLE_GLOG_DLL_DECL
#undef GLOG_NO_ABBREVIATED_SEVERITIES

#endif
