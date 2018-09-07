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
#include "mylibcurl.h"

using namespace std;
using namespace inifile;


enum UtilsError
{
	UTILS_RTMSG_OK = 000000,
	UTILS_OPEN_SUCCESS = 100000,
	UTILS_GET_INI_ERROR = 100001,   // ��ȡ�����ļ�ֵ����
	UTILS_FILE_ERROR = 100001,
	UTILS_DATE_EMPTY = 200001,       // ����Ϊ�ա���
	UTILS_DOC_FAILED = 200002,       // ���л�ʧ��
	UTILS_URL_ERROR =  300001
};


class CUtils
{
public:
	CUtils();
	~CUtils();

public:

	/*
	 * @brief	�����ļ�
	 * @param   ifstream		ifstream������
	 * @param   filename		�ļ���
	 * @return	enum			������
	 */
	UtilsError LoadFile(ifstream &file, const char* filename);


	/*
	 * @brief	������/Ӧ���в���key��Ӧ��valueֵ
	 * @param	strOrig			���ָ���ַ���
	 * @param	strKey			�����ҵ�key
	 * @param	strSplit			��key��ʼ���ֵĵ�һ����ȡ������;
	 * @return	strRetValue		���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���; ����key��ʼ���ַ�������û���ҵ�str_split, �򷵻�key��ʼ���ַ�������
	 */
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief	��־���ַ���ת����ʱ��
	 * @param   str				�����ַ���
								(��ʼλ������15λ��ʾʱ��,�����>15λ,��16-19Ϊ����)
								ʱ���ʽΪ:formate = (char*)"%4d%2d%2d-%2d%2d%2d
	 * @param   int				�ַ���(ʱ�俪ʼλ��)
	 * @param   int				�ַ���(ʱ�����λ��)
	 * @return	time_t			���غ��뼶ʱ��
	 */
	// u��runlog : time_t StringToMs(string strOrig, int iStart = 0, int iEnd = 19);
	// creator.out ��������:
	time_t StringToMs(string strOrig, int iStart = 39, int iEnd = 54);


	/*
	* @brief	�ж��ַ���ǰlenλ�Ƿ�����Ч����
	* @param    str				�ַ���
	* @param	int				�ַ���(ʱ�俪ʼλ��)
	* @param	int				�ַ���(ʱ�����λ��)
	* @return	bool			���򷵻� true, ���򷵻�false
	*/
	bool bCheckDate(string strOrig, int iStart, int iEnd);


	/*
	 * @brief	��ȡ�����ļ�·��
	 * @return	char*			�ļ�·��
	 */
	char * GetConfigPath();


	/*
	 * @brief	��ȡ����
	 * @param	strValue			��Ӧ��ֵ������
	 * @param   strKey				keyֵ
	 * @param   strSection			�ڵ�, Ĭ��[CONFIG]
	 * @return	strValue			valueֵ
	 */
	UtilsError GetConfigValue(string & strValue, string strKey, string strSection="CONFIG");


	/*
	 * @brief       ��ȡ��ǰʱ��(����)
	 * @return      __int64				time_t
	 */
	time_t GetCurrentTimeMs();


	/*
	 * @brief	��http����
	 * @param	pData			����
	 * @param	strResp			�յ�������
	 * @return	UtilsError		������
	 */
	UtilsError DoPost(char* pData, string &strResp);



	/*
	 * @brief	����Get����
	 * @param   arg1			arg1_command
	 * @param   arg2			arg2_command
	 * @param   arg3			arg3_command
	 * @return	return			return_command
	 */
	// UtilsError DoGet();



private:
	UtilsError _errorCode;
	char* ConfigPath = (char*)"./runlog_config.ini";
	int _errorLineNum;
	string _errorStr;
};


#endif
