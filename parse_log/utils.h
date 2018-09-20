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

/*改用webservice了*/
/*
// 这个封装用了很多windows的api
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
	UTILS_GET_INI_ERROR = 100001,   // 获取配置文件值出错
	UTILS_FILE_ERROR = 100001,
	UTILS_DATE_EMPTY = 200001,       // 数据为空　　
	UTILS_DOC_FAILED = 200002,       // 序列化失败
	UTILS_WEBSERVICE_FAIL = 300001 // web service失败
};


typedef struct CurrentSysTime
{
	char pDate[16];		//年月日
	char pTime[16];		//时分秒
	char pTimeMs[4];	//毫秒
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
	 * @brief	根据配置中的RunLogPath加载当天的日志文件
	 * @param   ifstream		ifstream的引用
	 * @param   filename		要打开的文件名
	 * @return	enum			错误码
	 */
	UtilsError LoadFile(ifstream &file, string filename = "runlog0.log");


	/*
	 * @brief	从行中查找key("="号左边)对应的value("="号右边)值
	 * @param	strOrig			待分割的字符串
	 * @param	strKey			待查找的key
	 * @param	strSplit		截取符(从key开始出现的第一个strSplit)
	 * @return	strRetValue		查找成功返回value,查找失败返回空字符串;
								若从key到结束都没找到str_split,则返回key开始到字符串结束
	 */
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief	行字符串转毫秒时间 (开始位置往后15位是日期时间,若>15位,则16-19为毫秒)
	 * @param   strOrig			整个字符串
								时间格式为:YYYYMMDD-HHMMSS; formate = (char*)"%4d%2d%2d-%2d%2d%2d
	 * @param   iStart			字符串中时间开始位置
	 * @param   iLen			字符串中**秒级*时间结束位置
								例: runlog0.log 为iStart = 0, iLen = 15
	 * @return	time_t			返回毫秒级时间
	 */
	time_t StringToMs(string strOrig, int iStart = 0, int iLen = 15);


	/*
	* @notice	XXX WINDOWS ONLY
	* @brief	判断字符串前len位是否是有效日期
	* @param    str				字符串
	* @param	int				字符串(时间开始位置)
	* @param	int				字符串(时间结束位置)
	* @return	bool			是则返回 true, 否则返回false
	*/
	// bool bCheckDate(string strOrig, int iStart, int iEnd);


	/*
	 * @brief	获取配置文件路径
	 * @return	char*			文件路径
	 */
	char * GetConfigPath();


	/*
	 * @brief	获取配置
	 * @param	strValue			对应的值的引用
	 * @param   strKey				key值
	 * @param   strSection			节点, 默认[CONFIG]
	 * @return	strValue			value值
	 */
	UtilsError GetConfigValue(string & strValue, string strKey, string strSection="CONFIG");


	/*
	 * @brief       获取当前时间(毫秒)
	 * @return      __int64				time_t
	 */
	time_t GetCurrentTimeMs();


	/*
	 * @brief       获取当前系统日期时间
	 * @param[in]		pDataFormat				日期的格式
	 * @param[in]		pTimeFormat				时间格式
	 * @return      该函数在初始化类对象时, 即初始化类成员m_stCurrSysTime
	 */
	// 初衷是为了能到当天文件夹下去找runlog的
	void SysNowTime(const char* pDataFormat = "%Y%m%d", const char* pTimeFormat = "%H%M%S");

	/*
	 * @brief	发Http Post请求
	 * @param	pData			数据
	 * @param	strResp			收到的数据
	 * @return	UtilsError		错误码
	 */
	UtilsError DoPost(char* pData, string &strResp);


	/*
	 * @brief       调用webservice
	 * @param[in]   strJsonData			发送的数据
	 * @param[in]   &strResp			返回的数据
	 * @return      string				返回的数据
	 */
	int WebServiceAgent(string strJsonData, string &strResp);


	/*
	 * @brief       获取文件最后n行(兼容单行和行尾有空行的情况)
	 * @param[in]   file			已打开的文件
	 * @param[in]   iLineNu			需要获取的行数
	 * @param[in]	vector<string>&	保存了从后往前的n行的vector
	 * @return      错误码
	 */
	UtilsError TailLine(ifstream &file, int iLineNum, vector<string>& vecRetStr);


	/*
	 * @brief       组装json串
	 * @param[in]   strReqData		请求串
	 * @param[in]   strAnsData		应答串(超时串没有ans, 默认值设为空字符串)
	 * @param[in]	iStart			字符串中时间开始位置
	 * @param[in]	iLen			字符串中时间长度, 如: 20180914-145814-608 则为 19
	 * @return      string			组装后的json
	 */
	string AssembleJson(string strReqData, string strAnsData = "", int iStart = 0, int iLen = 19);


	/*
	 * @brief       切割字符串
	 * @param[in]   strOrig			原始字符串
	 * @param[in]   strSplit		分隔符/串
	 * @return      vector<string>	返回子串
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
