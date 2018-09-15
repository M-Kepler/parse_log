#include "utils.h"
/*gsoap*/
#include "SoapServiceSoapBinding.nsmap"
#include "soapSoapServiceSoapBindingProxy.h"


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
	int iLen = iEnd - iStart;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = strOrig.substr(iStart, iLen);
#ifdef OS_IS_LINUX
	sscanf(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);
#else
	sscanf_s(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);
#endif

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
		time_t tm_ms = (tm_s * 1000) + stoi(strOrig.substr(iEnd + 1, 3)); // 转化为毫秒
		return tm_ms;
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


time_t CUtils::GetCurrentTimeMs()
{
	time_t currTime;
	currTime = time(NULL);
	return currTime * 1000 + clock();
}


UtilsError CUtils::TailLine(ifstream &file, int iLineNum, vector<string>& vecRetStr)
{
	int i;
	string strLine;
	streampos nCurentPos;

	if (!file)
	{
		LOG(ERROR) << "获取文件最后" << iLineNum << "行失败" << endl;
		LOG(ERROR) << "文件未打开" << endl;
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
		// TODO 异常抛出
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


UtilsError CUtils::WebServiceAgent(string strJsonData, string &strResp)
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
	UtilsError utilsError;

	if ((utilsError = GetConfigValue(strWebServiceUrl, "WebServiceUrl", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = GetConfigValue(strPort, "HttpPort", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		// TODO 异常抛出
		return utilsError;
	}

	SoapServiceSoapBindingProxy proxy(strWebServiceUrl.c_str(), SOAP_C_UTFSTRING);
	ns1__doService *pInput = new ns1__doService();
	ns1__doServiceResponse *pstrResponse = new ns1__doServiceResponse();

	pInput->requestXml = &strJsonData;

	/*
	soap_init(&LbmRiskSoap);
	SoapProxyInit(&LbmRiskSoap);
	soap_set_mode(&LbmRiskSoap, SOAP_C_UTFSTRING);
	*/

	iRetCode = proxy.doService(pInput, *pstrResponse);
	if (iRetCode != SOAP_OK)
	{
		LOG(ERROR) << "webservice 调用失败!" << "\t错误码为: " << iRetCode << endl;
		return UTILS_WEBSERVICE_FAIL;
	}
	else
	{
		// XXX 中文乱码
		// XXX 返回的是json,还需要解析
		strResp = *(pstrResponse->return_);
		LOG(INFO) << "webservice 调用成功!"<< "\t发送给 webservice 的数据为 : " << strJsonData << endl;
		LOG(INFO) << "webservice 返回的数据为: " << *(pstrResponse->return_) << endl;
		return UTILS_RTMSG_OK;
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
	"REQUESTS": [{
		"REQ_COMM_DATA": {
			"service": "IRMSLBMRISKWARNING",
				"REGKEY_ID" : "CHECK_TICKET",
				"LBM_CODE" : "L1190165",
				"USER_CODE" : "1595156513"
				...
		}
	}]
}
*/


string CUtils::AssembleJson(string strReqData, string strAnsData, int iStart, int iLen)
{
	bool isEmpty;
	string strReqBuf;
	string strServiceName; // 服务名
	UtilsError utilsError;
	vector<string> vecStrBuf;
	vector<string> vecStrSubBuf;
	rapidjson::StringBuffer strBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);

	if ((utilsError = GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		abort();
	}

	if (strAnsData.empty())
	{
		isEmpty = true;
	}
	else
	{
		isEmpty = false;
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
		writer.String(vecStrSubBuf[0].c_str());
		writer.String(vecStrSubBuf[1].c_str());
	}


	if (!isEmpty)
	{
		writer.String("AnsTime");
		writer.String(strAnsData.substr(iStart, iLen).c_str());
		writer.String("AnsRet1");
		writer.String(GetMsgValue(strAnsData, "&_1", "&_2").c_str());
		writer.String("AnsRet2");
		writer.String(GetMsgValue(strAnsData, "&_2", "`").c_str()); // XXX `字符是为了截取&_2往后的字符串
	}
	else
	{
		writer.String("AnsTime");
		writer.String("");
		writer.String("AnsRet1");
		writer.String("");
		writer.String("AnsRet2");
		writer.String("");
	}

	writer.EndObject(); // end REQ_COMM_DATA
	writer.EndObject(); // end REQUESTS
	writer.EndArray(); // end REQUESTS

	writer.EndObject(); // end JSON 

	string strJson = strBuffer.GetString();
	return strJson;
}

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
		// TODO 异常抛出
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
