#include "utils.h"

/* gsoap */
#include "SoapServiceSoapBinding.nsmap"
#include "soapSoapServiceSoapBindingProxy.h"


/* glog单例 */
// CGlog *p_glog = CGlog::GetInstance();


CUtils::CUtils()
{
	UtilsError utilsError;
	SysNowTime();// 初始化时间
}


CUtils::~CUtils()
{
}


UtilsError CUtils::LoadFile(ifstream &file)
{
	string filename;
	string strFileName;
	string strData;
	string strRunLogPath;
	UtilsError utilsError;

	if ((utilsError = GetConfigValue(strRunLogPath, "RunLogPath")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(filename, "FileName")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		// TODO 异常抛出
		abort();
	}

	if (bIsNextDay())
	{
	}
	strFileName = strRunLogPath + "/" + m_stCurrSysTime.pDate + "/" + filename;

	file.open(strFileName.c_str(), ios::binary | ios::in);

	if (!file)
	{
		LOG_FIRST_N(ERROR, 5) <<  "打开文件失败, 文件: \"" << strFileName << "\" 不存在!" << endl;
		return UTILS_FILE_ERROR;
	}
	else
	{
		LOG(INFO) <<  "打开文件: \"" << strFileName << "\" 成功!" << endl;
		return UTILS_RTMSG_OK;
	}
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
	int iMsStart;
	int year, month, day, hour, minute, second;
	string strLogType;

	char * formate;
	int iLen = iEnd - iStart;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = strOrig.substr(iStart, iLen);

#ifdef OS_IS_LINUX
	sscanf(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);
#else
	sscanf_s(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);
#endif

	if (UTILS_RTMSG_OK != GetConfigValue(strLogType, "LogType"))
	{
		LOG(ERROR) << "获取配置LogType失败" << endl;
		abort();
	}
	if (strLogType == "WIN")
	{
		iStart = iStart + 1;
		iEnd = iEnd + 1;
		iMsStart = 22;
	}
	else if (strLogType == "UNIX")
	{
		iMsStart = iEnd + 1;
	}

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
		// 20180202-091002-549327 取549为毫秒
		time_t tm_ms = (tm_s * 1000) + stoi(strOrig.substr(iMsStart, 3)); // 转化为毫秒
		return tm_ms;
	}
}


char * CUtils::GetConfigPath()
{
	return m_ConfigPath;
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


void CUtils::SysNowTime(const char* pDataFormat, const char* pTimeFormat)
{
	time_t timep;
	time(&timep);
	struct timeb tb;
	/*
	string test = "20180929";
	char Date[16];
	test.copy(m_stCurrSysTime.pDate, 8, 0);
	*(m_stCurrSysTime.pDate + 8) = '\0';
	*/

	strftime(m_stCurrSysTime.pDate, sizeof(m_stCurrSysTime.pDate), pDataFormat, localtime(&timep));
	strftime(m_stCurrSysTime.pTime, sizeof(m_stCurrSysTime.pTime), pTimeFormat, localtime(&timep));

	ftime(&tb);
	sprintf(m_stCurrSysTime.pTimeMs, "%03d", tb.millitm);
}


time_t CUtils::GetCurrentTimeMs()
{
	auto time_now = chrono::system_clock::now();
	auto duration_in_ms = chrono::duration_cast<chrono::milliseconds>(time_now.time_since_epoch());
	return duration_in_ms.count();
}


/* // 这种方法获取的时间比实际时间大
time_t CUtils::GetCurrentTimeMs()
{
	currTime = time(NULL);
	return currTime * 1000 + clock();
}
*/


UtilsError CUtils::TailLine(ifstream &file, int iLineNum, vector<string>& vecRetStr)
{
	int i;
	string strLine;
	streampos nCurentPos;

	if (!file)
	{
		LOG(ERROR) << "获取文件最后" << iLineNum << "行失败" << endl;
		return UTILS_FILE_ERROR;
	}

	// file.seekg(-2, file.cur);
	file.seekg(-2, ios::end); //倒回最后两个字符处

	for (i = 0; i < iLineNum; i++)
	{
		while (file.peek() != file.widen('\n'))
		{
			nCurentPos = file.tellg();
			if (nCurentPos == 0)
			{
				break;
			}
			file.seekg(-1, file.cur);
		}
		//读到"\n"标识 ，表示已经有一行了
		if (nCurentPos != 0)//倒回文件的开头，停止倒退
		{
			file.seekg(-1, file.cur);
		}
		else
		{
			break;
		}
	}
	if (nCurentPos != 0)
	{
		file.seekg(2, file.cur);
	}

	while (getline(file, strLine))
	{
		vecRetStr.push_back(strLine);
	}
	return UTILS_RTMSG_OK;
}


string CUtils::WString2String(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}


wstring CUtils::String2WString(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[]wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}


// string CUtils::WebServiceAgent(string strJsonData, string &strResp)
string CUtils::WebServiceAgent(string strJsonData)
{
	int iRetCode = 0;
	int iRet = 0;
	struct soap LbmRiskSoap;
	char szResult[1024 + 1];
	char szTemp[1024 + 1];
	char szIdCode[32 + 1] = { 0 };
	char szErrMsg[512 + 1] = { 0 };

	string strWebServiceUrl; // url 地址
	string strPort; // 端口
	string strServiceName; // 服务名
	string strFullUrl; // 完整 url
	string strHttpHeader; // 请求头
	string strResp;
	UtilsError utilsError;

	if ((utilsError = GetConfigValue(strWebServiceUrl, "WebServiceUrl", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		// TODO 异常抛出
		return to_string(utilsError);
	}

	// 如果发送的数据有中文, 对接的webservice 不认
	// SoapServiceSoapBindingProxy proxy(strWebServiceUrl.c_str(), SOAP_C_MBSTRING);
	SoapServiceSoapBindingProxy proxy(strWebServiceUrl.c_str(), SOAP_C_MBSTRING);

	ns1__doService *pInput = new ns1__doService();
	ns1__doServiceResponse *pstrResponse = new ns1__doServiceResponse();

	pInput->requestXml = &strJsonData;

	iRetCode = proxy.doService(pInput, *pstrResponse);


	LOG(INFO) << "==============-------------------------==============" << "发送的数据为:   " << "==============-------------------------==============" << endl;
	LOG(INFO) << strJsonData << endl;
	if (iRetCode != SOAP_OK)
	{
		LOG(ERROR) << "==============-------------------------==============" << "webservice 调用失败, 错误码为: " << iRetCode << "==============-------------------------==============" << endl;
		return to_string(iRetCode);
	}
	else
	{
		// FIXME 收到的回复有中文乱码
		strResp = *(pstrResponse->return_);
		LOG(INFO) << "==============-------------------------==============" << "webservice 调用成功! " << "==============-------------------------==============" << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		return strResp;
	}
}


void CUtils::GetWebServiceRet(vector<future<string>>& vecfuResults)
{
	// std::chrono::milliseconds TimeOut(100);
	for (auto && result : vecfuResults)
	{
		string strWebServiceRet = result.get();
		LOG(INFO) << "==============-------------------------==============" << "收到的回复为:   " << "==============-------------------------==============" << endl;
		LOG(INFO) << strWebServiceRet << endl;
	}

}


vector<string> CUtils::SplitString(string strOrig, string strSplit)
{
	char *pDelim = (char*)strSplit.c_str();
	char *strToken = NULL;
	char *nextToken = NULL;
	string sLine;
	vector <string> vecStringLine;
	vecStringLine.clear();

	strToken = strtok_s((char*)strOrig.c_str(), pDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		vecStringLine.push_back(sLine);
		strToken = strtok_s(NULL, pDelim, &nextToken);
	}
	return vecStringLine;
}


void CUtils::SoapProxyInit(struct soap *soap)
{
	if (strcmp(m_szIsProxy, "YES") == 0)
	{
		soap->proxy_host = m_szProxyHost;
		soap->proxy_port = m_iProxyPort;
		soap->proxy_userid = m_szProxyUserid;
		soap->proxy_passwd = m_szProxyPasswd;
	}
}


// 组装的Json格式如下
/*
{
	"REQUESTS":
	[
		{
			"REQ_COMM_DATA":
			{
				"service": "IRMSLBMRISKWARNING",
				"REGKEY_ID" : "CHECK_TICKET",
				"LBM_CODE" : "L1190165",
				"USER_CODE" : "1595156513"
				...
			}
		}
	]
}
*/

string CUtils::AssembleJson(string strReqData, string strAnsData, int iStart, int iLen)
{
	int iVecSize;
	bool bAnsIsEmpty;
	string strReqBuf;
	string strLogType;
	string strServiceName; // 服务名
	UtilsError utilsError;
	vector<string> vecStrBuf;
	vector<string> vecStrSubBuf;
	rapidjson::StringBuffer strBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);

	if ((utilsError = GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strLogType, "LogType")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		abort();
	}

	if (strLogType == "WIN")
	{
		iLen = 31;
	}

	if (strAnsData.empty())
	{
		bAnsIsEmpty = true;
	}
	else
	{
		bAnsIsEmpty = false;
	}

	writer.StartObject();
	writer.String("REQUESTS");
	writer.StartArray();
	writer.StartObject();

	writer.String("REQ_COMM_DATA");
	writer.StartObject();

	writer.String("service");
	writer.String(strServiceName.c_str());
	writer.String("REGKEY_ID");
	writer.String("CHECK_TICKET");

	writer.String("LBM_CODE");
	writer.String((char*)GetMsgValue(strReqData, "LBM").c_str());
	writer.String("MsgId");
	writer.String((char*)GetMsgValue(strReqData, "MsgId").c_str());
	writer.String("ReqTime");
	writer.String((char*)strReqData.substr(iStart, iLen).c_str());

	// 切割Buf
	strReqBuf = GetMsgValue(strReqData, "Buf");
	vecStrBuf = SplitString(strReqBuf, "&");

	for (size_t i = 0; i < vecStrBuf.size(); i++)
	{
		vecStrSubBuf = SplitString(vecStrBuf[i], "=");
		iVecSize = vecStrSubBuf.size();
		writer.String(vecStrSubBuf[0].c_str());
		// 有的参数"="号后面没有值
		if (iVecSize < 2)
		{
			writer.String("");
		}
		else
		{
			writer.String(vecStrSubBuf[1].c_str());
		}
	}

	if (!bAnsIsEmpty)
	{
		writer.String("AnsTime");
		writer.String(strAnsData.substr(iStart, iLen).c_str());

		string strAnsRet1 = GetMsgValue(strAnsData, "&_1", "&_2");

		// 去除结果集中的 '[' 和 ']' 字符
		for (auto it = strAnsRet1.begin(); it != strAnsRet1.end(); it++)
		{
			if (*it == '[' || *it == ']')
			{
				strAnsRet1.erase(it);
				it--;
			}
		}

		size_t uiFlag = strAnsRet1.substr(0).find(',');
		size_t uiCode = strAnsRet1.substr(uiFlag + 1).find(',');

		writer.String("AnsRet1Flag");
		writer.String(strAnsRet1.substr(0, 1).c_str());
		writer.String("AnsRet1RetCode");
		writer.String(strAnsRet1.substr(2, uiCode).c_str());
		writer.String("AnsRet1RetMsg");
		writer.String(strAnsRet1.substr(uiCode + 1 + uiFlag + 1).c_str());
		writer.String("AnsRet2Sec");
		// 确保ans串中没有``字符串才能截取&_2往后的字符串
		writer.String(GetMsgValue(strAnsData, "&_2", "``").c_str());

	}
	else
	{
		writer.String("AnsTime");
		writer.String("");
		writer.String("AnsRet1Flag");
		writer.String("");
		writer.String("AnsRet1RetCode");
		writer.String("");
		writer.String("AnsRet1RetMsg");
		writer.String("");
		writer.String("AnsRet2Sec");
		writer.String("");
	}


	writer.EndObject(); // end REQ_COMM_DATA
	writer.EndObject(); // end REQUESTS
	writer.EndArray(); // end REQUESTS

	writer.EndObject(); // end JSON

	string strJson = strBuffer.GetString();
	return strJson;
}



// http://einverne.github.io/post/2016/03/rapidjson-c-demo.html
string CUtils::ParseJson(const string ret, const string strKey)
{
	rapidjson::Document doc;
	doc.Parse<0>(ret.c_str());

	if (doc.HasMember("ANSWERS"))
	{
		const rapidjson::Value & answersjson = doc["ANSWERS"];
		for (rapidjson::SizeType i = 0; i < answersjson.Size(); ++i)
		{
			if (answersjson[i].HasMember("ANS_MSG_HDR"))
			{
				const rapidjson::Value & errorstroke = answersjson[i]["ANS_MSG_HDR"];
				for (rapidjson::Value::ConstMemberIterator iter = errorstroke.MemberBegin(); iter != errorstroke.MemberEnd(); ++iter)
				{
					if(iter->name.GetString() == strKey)
					{
						return iter->value.GetString();
					}
                }
            }
        }
    }
}


bool CUtils::bIsNextDay()
{
	time_t timep;
	time(&timep);
	struct timeb tb;
	CurrentSysTime stCurrSysTime;
	const char* pDataFormat = "%Y%m%d";

	strftime(stCurrSysTime.pDate, sizeof(m_stCurrSysTime.pDate), pDataFormat, localtime(&timep));

	// cout << stCurrSysTime.pDate << "\t" << m_stCurrSysTime.pDate << endl;
	// 新的一天
	if (strcmp(stCurrSysTime.pDate, m_stCurrSysTime.pDate) != 0)
	{
		memcpy(m_stCurrSysTime.pDate, stCurrSysTime.pDate, sizeof(stCurrSysTime.pDate));
		return true;
	}
	else
	{
		return false;
	}
}


// linux 下不可用
/*
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
*/


// linux下不可用
/*
UtilsError CUtils::DoPost(char * pData, string &strResp)
{
	string name;
	string value;
	CLibcurl libcurl;
	unsigned int uiPosKey;
	UtilsError enumError;
	CURLcode curlErrorCode;

	char* pUrl;
	int iPort;
	int iTimeOut;
	string strPort;
	string strTimeOut;
	string strHttpUrl; // http 地址
	string strHttpHeader; // http 请求头
	string strHttpRepeatNum; // 超时重发次数
	string strHttpTimeOut; // 超时时间
	UtilsError utilsError;

	if ((utilsError = GetConfigValue(strHttpUrl, "HttpUrl", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strPort, "HttpPort", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strTimeOut, "HttpTimeOut", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strHttpHeader, "HttpHeader", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strHttpRepeatNum, "HttpRepeatNum", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		return utilsError;
	}
	else
	{
		pUrl = (char*)strHttpUrl.c_str();
		iPort = stoi(strPort);
		iTimeOut = stoi(strTimeOut);

		libcurl.SetUserAgent("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
		libcurl.SetHttpPort(iPort);
		libcurl.SetConnectTimeout(iTimeOut);

		uiPosKey = strHttpHeader.find(":");
		if (uiPosKey != string::npos)
		{
			name = strHttpHeader.substr(1, uiPosKey);
			value = strHttpHeader.substr(uiPosKey + 1, strHttpHeader.length()-1);
			libcurl.AddHeader(name.c_str(), value.c_str());
		}
	}
	curlErrorCode = libcurl.Post((char*)strHttpUrl.c_str(), pData);

	if (CURLE_OK != curlErrorCode)
	{
		return UTILS_URL_ERROR;
	}
	else
	{
		strResp = libcurl.GetRespons();
		return UTILS_RTMSG_OK;
	}
}
*/


/* DoPost弃用, 改用webservice了*/
/*
UtilsError CUtils::DoPost(char * pData, string &strResp)
{
	string name;
	string value;
	unsigned int uiPosKey;
	UtilsError enumError;
	CURLcode curlErrorCode;

	char* pUrl;
	int iPort;
	int iTimeOut;
	string strPort;
	string strTimeOut;
	string strHttpUrl; // url 地址
	string strServiceName; // 服务名
	string strFullUrl; // 完整 url
	string strHttpHeader; // 请求头
	string strHttpRepeatNum; // 超时重发次数
	string strHttpTimeOut; // 超时时间
	UtilsError utilsError;

	if ((utilsError = GetConfigValue(strHttpUrl, "HttpUrl", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strPort, "HttpPort", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strTimeOut, "HttpTimeOut", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strHttpHeader, "HttpHeader", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strHttpRepeatNum, "HttpRepeatNum", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		return utilsError;
	}
	else
	{
		strFullUrl = strHttpUrl + strServiceName;
		pUrl = (char*)strFullUrl.c_str();
		iPort = stoi(strPort);
		iTimeOut = stoi(strTimeOut);

		curl_easy_setopt(pUrl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
		curl_easy_setopt(pUrl, CURLOPT_PORT, iPort);
		curl_easy_setopt(pUrl, CURLOPT_CONNECTTIMEOUT, iTimeOut); // 连接等待时间

		uiPosKey = strHttpHeader.find(":");
		if (uiPosKey != string::npos)
		{
			name = strHttpHeader.substr(1, uiPosKey);
			value = strHttpHeader.substr(uiPosKey + 1, strHttpHeader.length() - 1);
			string strHeader(name);
			strHeader.append(": ");
			strHeader.append(value);
			struct curl_slist* headers = NULL;
			curl_slist_append(headers, strHeader.c_str());
			curl_easy_setopt(pUrl, CURLOPT_HTTPHEADER, headers);
		}
	}
	curl_easy_setopt(pUrl, CURLOPT_POST, 1);
	curl_easy_setopt(pUrl, CURLOPT_POSTFIELDS, pData); // Post请求的数据
	curl_easy_setopt(pUrl, CURLOPT_URL, pUrl);
	strResp = curl_easy_perform(pUrl); // 开始执行
	return UTILS_RTMSG_OK;
}
*/


