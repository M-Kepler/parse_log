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
 * ����ɵ���ģʽ; ֻ����һ��ʵ��
 * �Ӳ�����һ��������ʼ���ж�glog�ĳ�ʼ��
 * �������н���������������ʱ��ر�glog
 */

class CGlog
{
private:
	CGlog()
	{
		InitGlog();
	};
	CGlog(const CGlog&) = delete;
	CGlog& operator= (const CGlog &) = delete;
	~CGlog()
	{
		CloseGlog();
	};

public:

	static CGlog* GetInstance()
	{
		static CGlog instance; // �ֲ���̬����
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