#pragma once
#ifndef __LOG_H_
#define __LOG_H_

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <iostream>
#include <glog/logging.h>
#include <string>
#include "inifile.h"

#ifdef _DEBUG
#pragma comment(lib, "glogd")
#else
#pragma comment(lib, "glog")
#endif

using namespace std;
using namespace inifile;

/*
 * 改造成单例模式; 只产生一个实例
 * 从产生第一个单例开始进行对glog的初始化
 * 程序运行结束调用析构函数时会关闭glog
 */

class CGlog
{
private:
	CGlog()
	{
		cout << "init glog" << endl;
		InitGlog();
	};
	CGlog(const CGlog&) = delete;
	CGlog& operator= (const CGlog &) = delete;
	~CGlog()
	{
		cout << "close glog" << endl;
		CloseGlog();
	};

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
