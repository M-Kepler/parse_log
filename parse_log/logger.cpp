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
		//��ʼ��  
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

		//��׷�ӵķ�ʽ���ļ���  
		m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");

		InitializeCriticalSection(&m_cs);
	}


	//��������  
	CLogger::~CLogger()
	{
		//�ͷ��ٽ���  
		DeleteCriticalSection(&m_cs);
		//�ر��ļ���  
		if (m_pFileStream)
		{
			fclose(m_pFileStream);
			m_pFileStream = NULL;
		}
	}

	//�ļ�ȫ·���õ��ļ���  
	const char *CLogger::path_file(const char *path, char splitter)
	{
		return strrchr(path, splitter) ? strrchr(path, splitter) + 1 : path;
	}

	//дһ����Ϣ  
	void CLogger::TraceInfo(const char *lpcszFormat, ...)
	{

		string strResult;
		if (NULL != lpcszFormat)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat); //��ʼ����������  
			size_t nLength = 4096;
			std::vector<char> vBuffer(nLength, '\0'); //�������ڴ洢��ʽ���ַ������ַ�����  
			int nWritten = _vsnprintf(&vBuffer[0], nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker); //���ñ�������  
		}
		if (strResult.empty())
		{
			return;
		}
		string strLog;
		strLog.append(GetTime()).append("|").append(strResult);

		//д��־�ļ�  
		Trace(strLog);
	}

	//��ȡϵͳ��ǰʱ��  
	string CLogger::GetTime()
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);

		//2016-01-01 21:30:00  
		string strTime = FormatString("%04d%02d%02d %02d:%02d:%02d.%03d", sys.wYear, sys.wMonth,
			sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

		return strTime;
	}

	//��ȡϵͳ��ǰʱ��  
	int CLogger::GetCurDate()
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);


		int iCurDate = sys.wYear * 10000 + sys.wMonth * 100
			+ sys.wDay;
		return iCurDate;
	}

	//д�ļ�����  
	void CLogger::Trace(const string &strLog)
	{
		try
		{
			//�����ٽ���  
			CSGuard g(&m_cs);
			//���ļ���û�д򿪣������´�  
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
				//��׷�ӵķ�ʽ���ļ���  
				m_pFileStream = fopen(m_strLogFilePath.c_str(), "a+");
				if (!m_pFileStream)
				{
					return;
				}
			}
			//д��־��Ϣ���ļ���  
			fprintf(m_pFileStream, "%s\n", strLog.c_str());
			fflush(m_pFileStream);
		}
		//�������쳣�������뿪�ٽ�������ֹ����  
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
			va_start(marker, lpcszFormat); //��ʼ����������  
			size_t nLength = 4096;
			std::vector<char> vBuffer(nLength, '\0'); //�������ڴ洢��ʽ���ַ������ַ�����  
			int nWritten = _vsnprintf(&vBuffer[0], nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker); //���ñ�������  
		}
		return strResult;
	}
}

