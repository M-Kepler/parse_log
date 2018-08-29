#include "utils.h"


// glog 单例
// CGlog *p_glog = CGlog::GetInstance();


CUtils::CUtils()
{
}


CUtils::~CUtils()
{
}


UtilsError CUtils::LoadFile(ifstream &file, const char* filename)
{
	file.open(filename, ios::binary | ios::in);

	if (!file)
	{
		// 打印日志
		LOG(ERROR) <<  "Error: File \"" << filename << "\" do not exist!" << endl;
		return UTILS_FILE_ERROR;
	}
	else
	{
		return UTILS_OPEN_SUCCESS;
	}
	// file.close();
}


string CUtils::GetMsgValue(string strOrig, string strKey, string strSplit)
{
	string strRetValue = "";
	int iStrOrigLen;
	int iStrKeyLen;
	size_t uiPosKeyBegin;
	size_t uiPosKeyEnd;
	size_t uiPosStrSplit;

	iStrOrigLen = strOrig.length();
	iStrKeyLen = strKey.length();
	uiPosKeyBegin = strOrig.find(strKey);

	if (uiPosKeyBegin != string::npos)
	{
		// 从key的位置开始,第一次出现 str_split 的位置
		uiPosStrSplit =  strOrig.substr(uiPosKeyBegin).find(strSplit);
		if (uiPosStrSplit != string::npos)
		{
			uiPosKeyEnd = uiPosKeyBegin + uiPosStrSplit;
		}
		else
		{
			uiPosKeyEnd = iStrOrigLen;
		}
		int pos_begin = uiPosKeyBegin + iStrKeyLen + 1; // +1 跳过'='字符
		int value_len = uiPosKeyEnd - pos_begin;
		strRetValue = strOrig.substr(pos_begin, value_len);
		return strRetValue;
	}
	return strRetValue;
}


time_t CUtils::StringToMs(string strOrig, int iStart, int iEnd)
{
	tm tm_;
	string str;
	int year, month, day, hour, minute, second;
	char * formate;
	int len = iEnd - iStart;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = strOrig.substr(iStart, len);
	sscanf_s(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);

	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;
	time_t tm_s = mktime(&tm_);
	if (strOrig.length() <= 15)
	{
		time_t tm_ms = tm_s * 1000;
		return tm_ms;
	} 
	else
	{
		// 20180202-091002-549327 取547为毫秒
		time_t tm_ms = (tm_s * 1000) + stoi(strOrig.substr(iEnd + 1, 3)); // 转化为毫秒
		return tm_ms;
	}
}


bool CUtils::bCheckDate(string strOrig, int iStart, int iEnd)
{
	char* formate;
	string str;
	int len = iEnd - iStart;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = strOrig.substr(iStart, len);
	sscanf_s(str.c_str(), formate, &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);

	if (nYear < 1970 || nYear > 3000 || nMonth < 1 || nMonth> 12 || nDay < 1 || nDay  > 31
		|| nHour < 0 || nHour > 23 || nMinute < 0 || nMinute>59 || nSecond < 0 || nSecond>59)
	{
		return false;
	}
	CTime t(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	// CTime会对非正常的日期进行进位处理, 如4月31会转换为5月1号,所以如果有发生转换则认为非正常日期

	return (nYear == t.GetYear()
		&& nMonth == t.GetMonth()
		&& nDay == t.GetDay()
		&& nHour == t.GetHour()
		&& nMinute == t.GetMinute()
		&& nSecond == t.GetSecond()
		);
}


char * CUtils::GetConfigPath()
{
	return ConfigPath;
}


UtilsError CUtils::GetConfigValue(string & strValue, string strKey, string strSection)
{
	int iRetCode;
	IniFile ini;
	char * configpath = GetConfigPath();
	iRetCode = ini.load(configpath);
	if (iRetCode != RET_OK)
	{
		return UTILS_FILE_ERROR;
	}
	iRetCode = ini.getValue(strSection, strKey, strValue);
	if (iRetCode != RET_OK)
	{
		return UTILS_GET_INI_ERROR;
	}
	return UTILS_RTMSG_OK;
}


UtilsError CUtils::GetConfigValue(int& iValue, string strKey, string strSection)
{
	int iRetCode;
	IniFile ini;
	char * configpath = GetConfigPath();
	iRetCode = ini.load(configpath);
	if (iRetCode != RET_OK)
	{
		return UTILS_FILE_ERROR;
	}
	iRetCode = ini.getIntValue(strSection, strKey, iValue);
	if (iRetCode != RET_OK)
	{
		return UTILS_GET_INI_ERROR;
	}
	return UTILS_RTMSG_OK;
}



time_t CUtils::GetCurrentTimsMS()
{
	time_t currTime;
	currTime = time(NULL);
	return currTime * 1000 + clock();
}

// UtilsError CUtils::DoPost(int iPort, int iTimeout, const char* cookieFilePath, char * pData,char* pHttpUrl, string &strResp)
UtilsError CUtils::DoPost(int iPort, int iTimeout, char * pData, char* pHttpUrl, string &strResp)
{
	string name;
	string value;
	CLibcurl libcurl;
	unsigned int uiPosKey;
	string strHttpHeader;
	UtilsError enumError;
	CURLcode curlErrorCode;

	libcurl.SetUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
	libcurl.SetHttpPort(iPort);
	libcurl.SetConnectTimeout(iTimeout);
	enumError = GetConfigValue(strHttpHeader, "HttpHeader", "CURL");

	if (enumError != UTILS_RTMSG_OK)
	{
		return enumError;
	}
	else
	{
		uiPosKey = strHttpHeader.find(":");
		if (uiPosKey != string::npos)
		{
			name = strHttpHeader.substr(1, uiPosKey);
			value = strHttpHeader.substr(uiPosKey + 1, strHttpHeader.length()-1);
			libcurl.AddHeader(name.c_str(), value.c_str());
		}
		/*
		char* name2 = (char*) "Content-Typ";
		char* value2 = (char*) "application/x-www-form-urlencoded;charset=UTF-8";
		libcurl.AddHeader(name2, value2);
		*/
	}
	// char* pData = (char*)"maintype=10001&subtype=100&appver=2.5.19.3753&sysver=Microsoft Windows 7&applist=100:15,200:2&sign=2553d888bc922275eca2fc539a5f0c1b";
	curlErrorCode = libcurl.Post(pHttpUrl, pData);
	if (CURLE_OK != curlErrorCode)
	{
		return UTILS_URL_ERROR;
		LOG(ERROR) << "发送Post请求失败, 错误码: " << curlErrorCode << "\t 发送的数据为: " << &pData << endl;
	}
	else
	{
		strResp = libcurl.GetRespons();
		return UTILS_RTMSG_OK;
	}
}
