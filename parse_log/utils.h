#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <time.h>
// #include <atltime.h>
#include <iostream>
#include <string>
#include <fstream>
#include "inifile.h"
#include "log.h"

/*����webservice��*/
/*
// �����װ���˺ܶ�windows��api
// #include "mylibcurl.h"
#include <curl/curl.h>
*/

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>


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
	UTILS_WEBSERVICE_FAIL = 300001 // web serviceʧ��
};


typedef struct CurrentSysTime
{
	char pDate[16];		//������
	char pTime[16];		//ʱ����
	char pTimeMs[4];	//����
}CurrentSysTime, *pCurrSysTime;


class CUtils
{
public:
	CUtils();
	~CUtils();

public:
	CurrentSysTime m_stCurrSysTime;

public:

	/*
	 * @brief	���������е�RunLogPath���ص������־�ļ�
	 * @param   ifstream		ifstream������
	 * @param   filename		Ҫ�򿪵��ļ���
	 * @return	enum			������
	 */
	UtilsError LoadFile(ifstream &file, string filename = "runlog0.log");


	/*
	 * @brief	�����в���key("="�����)��Ӧ��value("="���ұ�)ֵ
	 * @param	strOrig			���ָ���ַ���
	 * @param	strKey			�����ҵ�key
	 * @param	strSplit		��ȡ��(��key��ʼ���ֵĵ�һ��strSplit)
	 * @return	strRetValue		���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���;
								����key��������û�ҵ�str_split,�򷵻�key��ʼ���ַ�������
	 */
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief	���ַ���ת����ʱ�� (��ʼλ������15λ������ʱ��,��>15λ,��16-19Ϊ����)
	 * @param   strOrig			�����ַ���
								ʱ���ʽΪ:YYYYMMDD-HHMMSS; formate = (char*)"%4d%2d%2d-%2d%2d%2d
	 * @param   iStart			�ַ�����ʱ�俪ʼλ��
	 * @param   iLen			�ַ�����**�뼶*ʱ�����λ��
								��: runlog0.log ΪiStart = 0, iLen = 15
	 * @return	time_t			���غ��뼶ʱ��
	 */
	time_t StringToMs(string strOrig, int iStart = 0, int iLen = 15);


	/*
	* @notice	XXX WINDOWS ONLY
	* @brief	�ж��ַ���ǰlenλ�Ƿ�����Ч����
	* @param    str				�ַ���
	* @param	int				�ַ���(ʱ�俪ʼλ��)
	* @param	int				�ַ���(ʱ�����λ��)
	* @return	bool			���򷵻� true, ���򷵻�false
	*/
	// bool bCheckDate(string strOrig, int iStart, int iEnd);


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
	 * @brief       ��ȡ��ǰϵͳ����ʱ��
	 * @param[in]		pDataFormat				���ڵĸ�ʽ
	 * @param[in]		pTimeFormat				ʱ���ʽ
	 * @return      �ú����ڳ�ʼ�������ʱ, ����ʼ�����Աm_stCurrSysTime
	 */
	// ������Ϊ���ܵ������ļ�����ȥ��runlog��
	void SysNowTime(const char* pDataFormat = "%Y%m%d", const char* pTimeFormat = "%H%M%S");

	/*
	 * @brief	��Http Post����
	 * @param	pData			����
	 * @param	strResp			�յ�������
	 * @return	UtilsError		������
	 */
	UtilsError DoPost(char* pData, string &strResp);


	/*
	 * @brief       ����webservice
	 * @param[in]   strJsonData			���͵�����
	 * @param[in]   &strResp			���ص�����
	 * @return      string				���ص�����
	 */
	int WebServiceAgent(string strJsonData, string &strResp);


	/*
	 * @brief       ��ȡ�ļ����n��(���ݵ��к���β�п��е����)
	 * @param[in]   file			�Ѵ򿪵��ļ�
	 * @param[in]   iLineNu			��Ҫ��ȡ������
	 * @param[in]	vector<string>&	�����˴Ӻ���ǰ��n�е�vector
	 * @return      ������
	 */
	UtilsError TailLine(ifstream &file, int iLineNum, vector<string>& vecRetStr);


	/*
	 * @brief       ��װjson��
	 * @param[in]   strReqData		����
	 * @param[in]   strAnsData		Ӧ��(��ʱ��û��ans, Ĭ��ֵ��Ϊ���ַ���)
	 * @param[in]	iStart			�ַ�����ʱ�俪ʼλ��
	 * @param[in]	iLen			�ַ�����ʱ�䳤��, ��: 20180914-145814-608 ��Ϊ 19
	 * @return      string			��װ���json
	 */
	string AssembleJson(string strReqData, string strAnsData = "", int iStart = 0, int iLen = 19);


	/*
	 * @brief       �и��ַ���
	 * @param[in]   strOrig			ԭʼ�ַ���
	 * @param[in]   strSplit		�ָ���/��
	 * @return      vector<string>	�����Ӵ�
	 */
	vector<string> SplitString(string strOrig, string strSplit);


	void SoapProxyInit(struct soap* soap);


private:
	UtilsError _errorCode;
	char* m_ConfigPath = (char*)"./runlog_config.ini";
	int m_iErrorLineNum;
	string m_strErrorStr;

	/* soap */
	char m_szIsProxy[8 + 1];
	char m_szProxyType[8 + 1];
	char m_szProxyHost[32 + 1];	/* Proxy Server host name */
	int m_iProxyPort;		/* Proxy Server port (default = 8080) */
	char m_szProxyUserid[32 + 1];	/* Proxy Authorization user name */
	char m_szProxyPasswd[32 + 1];	/* Proxy Authorization password */

};


#endif
