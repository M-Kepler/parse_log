#pragma once

#ifndef _LOGGER_H_  
#define _LOGGER_H_  

#include <Windows.h>  
#include <stdio.h>  
#include <string>  


namespace BXG
{
	class CLogger
	{
	public:

		CLogger(const std::string strLogPath = "", const std::string strLogName = "");

		virtual ~CLogger();
	public:

		void TraceInfo(const char *lpcszFormat, ...);


		static std::string GetAppPathA();

		static std::string FormatString(const char *lpcszFormat, ...);
	private:

		void Trace(const std::string &strLog);

		std::string GetTime();

		int GetCurDate();

		const char *path_file(const char *path, char splitter);

	private:

		FILE * m_pFileStream;

		std::string m_strLogPath;

		std::string m_strLogName;

		std::string m_strLogFilePath;

		int m_iCurDate;

		CRITICAL_SECTION m_cs;
	};
}

#endif  

