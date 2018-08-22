#pragma once
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#ifndef _UTILS_H
#define _UTILS_H

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>
#include <fstream>
#include "inifile.h"
#include "glog/logging.h"

using namespace std;
using namespace inifile;


enum UtilsError {
	UTILS_RTMSG_OK = 000000,
	UTILS_INI_FILE_ERROR = 000001,
	UTILS_OPEN_SUCCESS = 100000,
	UTILS_FILE_NOT_FOUND = 100001,
	UTILS_DATE_EMPTY = 600001,       // ����Ϊ�ա���
	UTILS_DOC_FAILED = 600004,       // ���л�ʧ��
};

class CUtils
{
public:
	CUtils();
	~CUtils();

public:

	UtilsError InitGlog();
	// static UtilsError InitGlog(const char*);
	void ShutdownGlog();


	/*
	 * @brief       �����ļ�
	 * @param[in]   ifstream		ifstream������
	 * @param[in]   filename		�ļ���
	 * @return      enum			������
	 */
	UtilsError LoadFile(ifstream &file, const char* filename);


	/*
	 * @brief	   ������/Ӧ���в���key��Ӧ��valueֵ
	 * @param[in]  strOrig			���ָ���ַ���
	 * @param[in]  strKey			�����ҵ�key
	 * @param[in]  strSplit			��key��ʼ���ֵĵ�һ����ȡ������;
	 * @return	   strRetValue		���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���; ����key��ʼ���ַ�������û���ҵ�str_split, �򷵻�key��ʼ���ַ�������
	 */
	//TODO		   ��Բ�ͬ����־��ʽ,���ظ÷���
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief       ��־���ַ���ת����ʱ��
	 * @param[in]   str				�ַ���(ǰ15λ��ʾʱ��,16-19������)
	 * @param[in]   int				�ַ���(ʱ�俪ʼλ��)
	 * @param[in]   int				�ַ���(ʱ�����λ��)
	 * @return      time_t			���غ��뼶ʱ��
	 */
	time_t StringToMs(string strOrig, int iStart, int iEnd);


	/*
	* @brief        �ж��ַ���ǰlenλ�Ƿ�����Ч����
	* @param[in]    str				�ַ���
	* @param[in]   int				�ַ���(ʱ�俪ʼλ��)
	* @param[in]   int				�ַ���(ʱ�����λ��)
	* @return       bool			���򷵻� true, ���򷵻�false
	*/
	bool bCheckDate(string strOrig, int iStart, int iEnd);

	char * getConfigPath();


private:
	// IniFile clIniFile;
	UtilsError _errorCode;
	// static const char* ConfigPath;
	char* ConfigPath = (char*)"./runlog_config.ini";

	int _errorLineNum;
	string _errorStr;
};


#endif
