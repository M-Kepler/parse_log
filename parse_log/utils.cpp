

#include "utils.h"


// TODO 
// 可不可以弄个全局的, 类创建对象时就初始化glog

CUtils::CUtils()
{
	cout << __FILE__ << __LINE__ << "utils creator" << endl;
	// InitGlog(ConfigPath);
}

CUtils::~CUtils()
{
	cout << "utils destroy" << endl;
	// ShutdownGlog();
}

// const char * CUtils::ConfigPath = (char*)"./runlog_config.ini";

// UtilsError CUtils::InitGlog(const char* ConfigPath)
UtilsError CUtils::InitGlog()
{
	IniFile clIniFile;
	int iRetCode;
	string Section;
	string Key_Log_Lvl;
	string Key_Log_Path;
	string Key_LogFile_Max_Size;
	string Key_Log_Buf_Secs;
	string Value_Log_Lvl;
	string Value_Log_Path;
	string Value_LogFile_Max_Size;
	string Value_Log_Buf_Secs;
	char* ConfigFile;

	ConfigFile = getConfigPath();
	Key_Log_Lvl = "Log_Lvl";
	Key_LogFile_Max_Size = "LogFile_Max_Size";
	Key_Log_Path = "Log_Path";
	Key_Log_Buf_Secs = "Log_Buf_Secs";
	Section = "GLOG";

	clIniFile.load(ConfigFile);
	clIniFile.getValue(Section, Key_Log_Lvl, Value_Log_Lvl);
	cout << Value_Log_Lvl << endl;

	if ((iRetCode = clIniFile.getValue(Section, Key_Log_Lvl, Value_Log_Lvl)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Path, Value_Log_Path)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_LogFile_Max_Size, Value_LogFile_Max_Size)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Buf_Secs, Value_Log_Buf_Secs)) != RET_OK
		)
	{
		return UTILS_INI_FILE_ERROR;
	}

	// FIXME
	// 全局初始化glog
	google::InitGoogleLogging("test");
    // 设置glog的输出级别，这里的含义是输出INFO级别以上的信息
	google::SetStderrLogging(google::GLOG_INFO);

	// 设置INFO/WARNING/ERROR级别以上的信息log文件的路径和前缀名
	google::SetLogDestination(google::GLOG_INFO, (Value_Log_Path + "\\INFO_").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (Value_Log_Path + "\\WARNING_").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (Value_Log_Path + "\\ERROR_").c_str());

	FLAGS_logbufsecs = stoi(Value_Log_Buf_Secs);
	FLAGS_max_log_size = stoi(Value_LogFile_Max_Size);

	// FLAGS_colorlogtostderr = true;                     // 开启终端颜色区分
	return UTILS_RTMSG_OK;
}


void CUtils::ShutdownGlog()
{
	google::ShutdownGoogleLogging();
}


UtilsError CUtils::LoadFile(ifstream &file, const char* filename)
{
	// ifstream file;
	file.open(filename, ios::binary | ios::in);

	if (!file)
	{
		// 打印日志
		LOG(INFO) <<  "Error: File \"" << filename << "\" do not exist!" << endl;
		return UTILS_FILE_NOT_FOUND;
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
	// str = strOrig.substr(0, 15);
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
	time_t tm_ms = (tm_s * 1000) + stoi(strOrig.substr(iEnd + 1, 3)); // 转化为毫秒
	return tm_ms;
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

char * CUtils::getConfigPath()
{
	return ConfigPath;
}

