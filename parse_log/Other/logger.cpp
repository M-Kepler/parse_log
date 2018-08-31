// #include "stdafx.h"
#include "logger.h"  
#include <time.h>  
#include <stdarg.h>  
#include <direct.h>  
#include <vector>

 #include "CSGuard.h"

using std::string;
using std::vector;

namespace BXG
{
	CLogger::CLogger(const std::string strLogPath, const std::string strLogName)
		: m_strLogPath(strLogPath),
		m_strLogName(strLogName)
	{
		//初始化  
		m_pFileStream = NULL;

		if (m_strLogPath[m_strLogPath.length() - 1] != '\\')
		{
			m_strLogPath.append("\\");
		}

		m_iCurDate = GetCurDate();

		char sDate[16];
		memset(sDate, 0, sizeof(sDate));
		_itoa(m_iCurDate, sDate, 10);

		m_strLogFilePath = m_strLogPath.append(m_strLogName).append("-").append(sDate).append(".log");

		//以追加的方式打开文件流  
		m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");

		InitializeCriticalSection(&m_cs);
	}


	//析构函数  
	CLogger::~CLogger()
	{
		//释放临界区  
		DeleteCriticalSection(&m_cs);
		//关闭文件流  
		if (m_pFileStream)
		{
			fclose(m_pFileStream);
			m_pFileStream = NULL;
		}
	}

	//文件全路径得到文件名  
	const char *CLogger::path_file(const char *path, char splitter)
	{
		return strrchr(path, splitter) ? strrchr(path, splitter) + 1 : path;
	}

	//写一般信息  
	void CLogger::TraceInfo(const char *lpcszFormat, ...)
	{

		string strResult;
		if (NULL != lpcszFormat)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat); //初始化变量参数  
			size_t nLength = 4096;
			std::vector<char> vBuffer(nLength, '\0'); //创建用于存储格式化字符串的字符数组  
			int nWritten = _vsnprintf(&vBuffer[0], nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker); //重置变量参数  
		}
		if (strResult.empty())
		{
			return;
		}
		string strLog;
		strLog.append(GetTime()).append("|").append(strResult);

		//写日志文件  
		Trace(strLog);
	}

	//获取系统当前时间  
	string CLogger::GetTime()
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);

		//2016-01-01 21:30:00  
		string strTime = FormatString("%04d%02d%02d %02d:%02d:%02d.%03d", sys.wYear, sys.wMonth,
			sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

		return strTime;
	}

	//获取系统当前时间  
	int CLogger::GetCurDate()
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);


		int iCurDate = sys.wYear * 10000 + sys.wMonth * 100
			+ sys.wDay;
		return iCurDate;
	}

	//写文件操作  
	void CLogger::Trace(const string &strLog)
	{
		try
		{
			//进入临界区  
			CSGuard g(&m_cs);
			//若文件流没有打开，则重新打开  
			if (NULL == m_pFileStream)
			{
				return;
			}

			int iCurDate = GetCurDate();
			if (iCurDate != m_iCurDate)
			{
				fclose(m_pFileStream);
				m_pFileStream = NULL;

				char sDate[16];
				memset(sDate, 0, sizeof(sDate));
				_itoa(m_iCurDate, sDate, 10);

				m_strLogFilePath = m_strLogPath.append(m_strLogName).append("-").append(sDate);
				m_iCurDate = iCurDate;
				//以追加的方式打开文件流  
				m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");
				if (!m_pFileStream)
				{
					return;
				}
			}
			//写日志信息到文件流  
			fprintf(m_pFileStream, "%s\n", strLog.c_str());
			fflush(m_pFileStream);
		}
		//若发生异常，则先离开临界区，防止死锁  
		catch (...)
		{

		}
	}



	string CLogger::FormatString(const char *lpcszFormat, ...)
	{
		string strResult;
		if (NULL != lpcszFormat)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat); //初始化变量参数  
			size_t nLength = 4096;
			std::vector<char> vBuffer(nLength, '\0'); //创建用于存储格式化字符串的字符数组  
			int nWritten = _vsnprintf(&vBuffer[0], nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker); //重置变量参数  
		}
		return strResult;
	}
}

