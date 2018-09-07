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
	UTILS_GET_INI_ERROR = 100001,   // 获取配置文件值出错
	UTILS_FILE_ERROR = 100001,
	UTILS_DATE_EMPTY = 200001,       // 数据为空　　
	UTILS_DOC_FAILED = 200002,       // 序列化失败
	UTILS_URL_ERROR =  300001
};


class CUtils
{
public:
	CUtils();
	~CUtils();

public:

	/*
	 * @brief	加载文件
	 * @param   ifstream		ifstream的引用
	 * @param   filename		文件名
	 * @return	enum			错误码
	 */
	UtilsError LoadFile(ifstream &file, const char* filename);


	/*
	 * @brief	从请求/应答串中查找key对应的value值
	 * @param	strOrig			待分割的字符串
	 * @param	strKey			待查找的key
	 * @param	strSplit			从key开始出现的第一个截取结束符;
	 * @return	strRetValue		查找成功返回value,查找失败返回空字符串; 若从key开始到字符串结束没有找到str_split, 则返回key开始到字符串结束
	 */
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief	日志行字符串转毫秒时间
	 * @param   str				整个字符串
								(开始位置往后15位表示时间,如果有>15位,则16-19为毫秒)
								时间格式为:formate = (char*)"%4d%2d%2d-%2d%2d%2d
	 * @param   int				字符串(时间开始位置)
	 * @param   int				字符串(时间结束位置)
	 * @return	time_t			返回毫秒级时间
	 */
	// u版runlog : time_t StringToMs(string strOrig, int iStart = 0, int iEnd = 19);
	// creator.out 测试数据:
	time_t StringToMs(string strOrig, int iStart = 39, int iEnd = 54);


	/*
	* @brief	判断字符串前len位是否是有效日期
	* @param    str				字符串
	* @param	int				字符串(时间开始位置)
	* @param	int				字符串(时间结束位置)
	* @return	bool			是则返回 true, 否则返回false
	*/
	bool bCheckDate(string strOrig, int iStart, int iEnd);


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
	 * @brief	发http请求
	 * @param	pData			数据
	 * @param	strResp			收到的数据
	 * @return	UtilsError		错误码
	 */
	UtilsError DoPost(char* pData, string &strResp);



	/*
	 * @brief	发送Get请求
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
