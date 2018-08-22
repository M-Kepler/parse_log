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
	UTILS_DATE_EMPTY = 600001,       // 数据为空　　
	UTILS_DOC_FAILED = 600004,       // 序列化失败
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
	//TODO		   针对不同的日志格式,重载该方法
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
