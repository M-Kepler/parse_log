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
	UTILS_GET_INI_ERROR = 100001,   // 获取配置文件值出错
	UTILS_FILE_ERROR = 100001,
	UTILS_DATE_EMPTY = 600001,       // 数据为空　　
	UTILS_DOC_FAILED = 600004,       // 序列化失败
};


class CUtils
{
public:
	CUtils();
	~CUtils();

public:

	/*
	 * @brief       加载文件
	 * @param[in]   ifstream		ifstream的引用
	 * @param[in]   filename		文件名
	 * @return      enum			错误码
	 */
	UtilsError LoadFile(ifstream &file, const char* filename);


	/*
	 * @brief	   从请求/应答串中查找key对应的value值
	 * @param[in]  strOrig			待分割的字符串
	 * @param[in]  strKey			待查找的key
	 * @param[in]  strSplit			从key开始出现的第一个截取结束符;
	 * @return	   strRetValue		查找成功返回value,查找失败返回空字符串; 若从key开始到字符串结束没有找到str_split, 则返回key开始到字符串结束
	 */
	string GetMsgValue(string strOrig, string strKey, string strSplit = ",");


	/*
	 * @brief       日志行字符串转毫秒时间
	 * @param[in]   str				字符串(前15位表示时间,16-19。。。)
	 * @param[in]   int				字符串(时间开始位置)
	 * @param[in]   int				字符串(时间结束位置)
	 * @return      time_t			返回毫秒级时间
	 */
	time_t StringToMs(string strOrig, int iStart, int iEnd);


	/*
	* @brief        判断字符串前len位是否是有效日期
	* @param[in]    str				字符串
	* @param[in]   int				字符串(时间开始位置)
	* @param[in]   int				字符串(时间结束位置)
	* @return       bool			是则返回 true, 否则返回false
	*/
	bool bCheckDate(string strOrig, int iStart, int iEnd);


	/*
	 * @brief       获取配置文件路径
	 * @return      char*			文件路径
	 */
	char * GetConfigPath();


	/*
	 * @brief       获取配置
	 * @param[in]	strValue			对应的值的引用
	 * @param[in]   strKey				键值
	 * @param[in]   strSection			节点, 默认[CONFIG]
	 * @return      strValue			对应的值
	 */
	UtilsError GetConfigValue(string & strValue, string strKey, string strSection="CONFIG");


	/*
	 * @brief       获取当前时间(毫秒)
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
