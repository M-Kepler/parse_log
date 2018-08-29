#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>
#include <fstream>
#include "inifile.h"
#include "log.h"

using namespace std;
using namespace inifile;

// BXG::CLogger * g_pLogger = new BXG::CLogger(".\\log", "secu_callback");


enum UtilsError
{
	UTILS_RTMSG_OK = 000000,
	UTILS_OPEN_SUCCESS = 100000,
	UTILS_GET_INI_ERROR = 100001,   // ��ȡ�����ļ�ֵ����
	UTILS_FILE_ERROR = 100001,
	UTILS_DATE_EMPTY = 600001,       // ����Ϊ�ա���
	UTILS_DOC_FAILED = 600004,       // ���л�ʧ��
};


class CUtils
{
public:
	CUtils();
	~CUtils();

public:

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


	/*
	 * @brief       ��ȡ�����ļ�·��
	 * @return      char*			�ļ�·��
	 */
	char * GetConfigPath();


	/*
	 * @brief       ��ȡ����
	 * @param[in]	strValue			��Ӧ��ֵ������
	 * @param[in]   strKey				��ֵ
	 * @param[in]   strSection			�ڵ�, Ĭ��[CONFIG]
	 * @return      strValue			��Ӧ��ֵ
	 */
	UtilsError GetConfigValue(string & strValue, string strKey, string strSection="CONFIG");


	/*
	 * @brief       ��ȡ��ǰʱ��(����)
	 * @return      __int64				time_t
	 */
	time_t GetCurrentTimsMS();


private:
	UtilsError _errorCode;
	char* ConfigPath = (char*)"./runlog_config.ini";
	int _errorLineNum;
	string _errorStr;
};


#endif
