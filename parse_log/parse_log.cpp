#include <iostream>
#include <sys/timeb.h>
#include <mutex>
#include "inifile.h"
#include "LbmRiskWarning.h"
#include "log.h"

#include <thread>
#include <future>
#include "ThreadPool.h"

// 改用webservice
/*
#include <curl/curl.h>
#include "mylibcurl.h"
*/
#include <iomanip>
#include <stdio.h>

using namespace std;

string strResp;
char *loadedFile2[2];
const char* filepath = "runlog_config.ini";
// const char* filename = "runlog0-3.1.log";
const char* filename = "test.log";

mutex sLock;
int i = 0;

void test()
{
	for (int j = 0; j < 1000; j++)
	{
		sLock.lock();
		i++;
		sLock.unlock();
	}
}


std::string WString2String(const std::wstring& ws)
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


std::wstring String2WString(const std::string& s)
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


/*
typedef struct CurrentSysTime
{
	char pDate[16]; //年月日
	char pTime[16]; //时分秒
	char pTimeMs[4];  //毫秒
}CurrentSysTime, *pCurrSysTime;

CurrentSysTime getSysTime()
{
	time_t timep;
	time(&timep);
	CurrentSysTime date;

	strftime(date.pDate, sizeof(date.pDate), "%Y-%m-%d", localtime(&timep));
	strftime(date.pTime, sizeof(date.pTime), "%H:%M:%S", localtime(&timep));

	struct timeb tb;
	ftime(&tb);
	sprintf(date.pTimeMs, "%d", tb.millitm);
	return date;
}
*/


const std::string getCurrentSystemTime()
{
	auto tt = std::chrono::system_clock::to_time_t
	(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[60] = { 0 };
	sprintf(date, "%d-%02d-%02d      %02d:%02d:%02d",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
	return std::string(date);
}


void task(std::promise<string>& prom, string strJsonData, string &strResp)
{
	int x = 0;
	 // 这里一般一个非常耗时耗cpu的操作，在此过程中得到x的最终值，这里我们直接赋值为10
	x = 10;
	prom.set_value(strResp);         //设置共享状态的值，同时promise会设置为ready
}

// 访问异步操作的结果
void print_int(std::future<string>& fut)
{
	string x = fut.get();          //如果共享状态没有设置ready，调用get会阻塞当前线程
	std::cout << "value: " << x << '\n';

}


void prrr(vector<future<int>>& results)
{
	std::chrono::milliseconds TimeOut(100);
	for (auto && result : results)
	{
		while (result.wait_for(TimeOut) != std::future_status::ready)
		{
			std::cout << ".";
		}
		std::cout << std::endl;
	}
}

int test(int argv, char* argc[])
{
	CGlog *p_glog = CGlog::GetInstance();

	bool step = 0;
	streamsize loadsize = 250000;
	loadedFile2[0] = new char[loadsize];
	loadedFile2[1] = new char[loadsize];
	UtilsError utileError;

	string str_buf = "_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=522210&CUSTOMER=180022892&MARKET=0&BOARD=0&SECU_ACC=0139680203&NO_CHECK_STATUS=1";
	string str_req = "20180430-211359-522345-18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	// string str_ans = "20180719-094803-110762-12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, 业务程序运行正常, &_2=13863, 2, 21, 开户权限组<营业部>, 0, 10012, 2011-07-11 17:12:43.299541";
	string str_ans = "20180202-091406-895813-18190    98 Ans: LBM=L0301042, MsgId=0000000100F465BE1E4D4B5E, Len=268, Buf=&_1=2,10233001,用户代码[150165853],客户[55345]不存在";


	// 读取文件
	CUtils clUtils;
	CLbmRiskWarning clLbmRiskWarning(8);

	ifstream file;

	streamoff start = 0;
	streamsize size;
	int iLen;


	while (1)
	{

		if (UTILS_RTMSG_OK != clUtils.LoadFile(file))
		{
			continue;
		}
		else
		{
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			LOG(INFO) << "==============-------------------------【" << clUtils.m_stCurrSysTime.pDate << "】的日志文件开始处理 -------------------------==============" << endl;
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			if (UTILS_RTMSG_OK == clLbmRiskWarning.multi_thread(file))
			{
				LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
				LOG(INFO) << "==============-------------------------【" << clUtils.m_stCurrSysTime.pDate << "】的日志文件处理完毕 -------------------------==============" << endl;
				LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
				// 当天的处理完了, 要开始处理第二天的文件了
				continue;
			}
		}
	}


	// AssembleJson、WebServiceAgent
	/*
	string strJson = clUtils.AssembleJson(str_req, str_ans, 0, 19);
	cout << strJson << endl;
	string strResp;
	clUtils.WebServiceAgent(strJson, strResp);
	*/

	// 线程池
	/*
	string strPostData = "send";
	string strResponse;


	ThreadPool pool(4);
	std::vector< std::future<int> > results;

	for (int i = 0; i < 8; ++i)
	{ 
		// results.emplace_back(pool.enqueue(&CUtils::WebServiceAgent, clUtils, strPostData, strResponse));
		// results.emplace_back(pool.enqueue(clUtils.WebServiceAgent, strPostData, strResponse));
		results.emplace_back(
			pool.enqueue([i]
		{
			std::cout << "hello-" << i << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "world-" << i << std::endl;
			return i * i;
		})
		);
	}

	cout << "for循环外" << endl;

	thread watchjob(prrr, std::ref(results));
	*/

	/*
	while (1)
	{
		Sleep(1000);
		cout << "我去做别的事了" << endl;
	}
	*/


	// 打开文件
	/*
	file.open(filename, ios::binary | ios::in);
	if (!file)
	{
		cout << "open file fail" << endl;
	}
	*/


	// 打开文件 clutils.LoadFile
	/*
	if (UTILS_RTMSG_OK == clUtils.LoadFile(file))
	{
		cout << "文件打开成功" << endl;
	}
	*/


	// 当前时间(毫秒)
	/*
	string nowdate = clUtils.m_stCurrSysTime.pDate;
	string nowtime = clUtils.m_stCurrSysTime.pTime;
	string nowms = clUtils.m_stCurrSysTime.pTimeMs;
	string now = nowdate + "-" + nowtime + "-" + nowms;
	cout << now << endl;
	cout << clUtils.StringToMs(now, 0, 15) << endl;
	cout << clUtils.GetCurrentTimeMs() << endl;
	*/

	// gsoap
	/*
	const char* addr = "http://192.168.50.245/webapp/services/SoapService";
	SoapServiceSoapBindingProxy proxy(addr, SOAP_C_UTFSTRING);
	ns1__doService *info = new ns1__doService();
	ns1__doServiceResponse *response = new ns1__doServiceResponse();


	info->requestXml = new string(strJson);
	int iRetCode = proxy.doService(info, *response);
	if (iRetCode == SOAP_OK)
	{
		cout  << "SOAP_OK:" << SOAP_OK << ";RET:" << iRetCode << ";RETMSG:" << *(response->return_) << endl;
	}
	else
	{
		LOG(ERROR) << "Lbm Risk Warning Error" << endl;
	}
	*/

	//  libcurl 提交 xml 给 webservice
	/*
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

	if ((utilsError = clUtils.GetConfigValue(strHttpUrl, "HttpUrl", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strServiceName, "ServiceName", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strPort, "HttpPort", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strTimeOut, "HttpTimeOut", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strHttpHeader, "HttpHeader", "CURL")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strHttpRepeatNum, "HttpRepeatNum", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		// TODO 异常抛出
		return utilsError;
	}
	else
	{
		pUrl = (char*)strFullUrl.c_str();
		iPort = stoi(strPort);
		iTimeOut = stoi(strTimeOut);
	}
	//使用方法：webservice_sdk_submit(URL, 2000,待发数据,FALSE,返回值注意外部删除);
	char* pPostData = (char *)malloc(sizeof(char) * 20);
	char** pRetData = (char **)malloc(sizeof(char*) * 20);
	//for (int i = 0; i < 5; i++)
	//{
	//	pRetData[i] = new char[3];
	//}
	//memset(pRetData, 0, 3 * 5 * sizeof(char));
	webservice_sdk_submit((char*)strHttpUrl.c_str(), iTimeOut, pPostData, FALSE, pRetData);
	*/


	// 获取最后n行
	/*
	vector<string> vecRetStr;
	utileError = clUtils.TailLine(file, 1, vecRetStr);
	if (utileError == UTILS_RTMSG_OK)
	{
		for (size_t i = 0; i < vecRetStr.size(); i++)
		{
			cout << vecRetStr[i] << endl;
		}
	}
	else
	{
		cout << "获取文件最后n行失败" << endl;
	}
	*/


	// 获取尾行
	/*
	else
	{
		string strLastLine;
		while (file.peek() != EOF)
		{
			getline(file, strLastLine);
		}
		iLen = strLastLine.length();
		file.seekg(-(iLen + 1), ios::end);
		streampos pos1 = file.tellg();
		file.seekg(0, ios::end);
		streampos pos2 = file.tellg();
		file.seekg(-(iLen + 1), ios::end);
		size = pos2 - pos1;

		struct _stat buf;

		cout << "from file topoint: " << iLen << endl;

		file.read(loadedFile2[step], size);
	}

	char* filebuffer = loadedFile2[step];
	for (streamoff i = 0; i < size; ++i)
	{
		cout << filebuffer[i];
	}

	*/


	// 解析配置文件
	/*
	string strValue;
	if (UTILS_RTMSG_OK != clUtils.GetConfigValue(strValue, "ThreadCount"))
	{
		LOG(ERROR) << "获取配置文件值出错" << endl;
	};
	*/


	// 获取msg中key的value
	/*
	const char* key_lbm = "Buf";
	string key_lbm_value = clUtils.GetMsgValue(str_req, key_lbm);
	if (key_lbm_value.empty())
	{
		cout << "没找到" << endl;
	}
	if (key_lbm_value != "")
	{
		cout << key_lbm << ": " << key_lbm_value << endl;
	}
	*/


	// 字符串切割
	/*
	vector<string> splitRet;
	vector<string> splitRet2;
	splitRet = clUtils.SplitString(str_buf, "&");
	for (size_t i = 0; i < splitRet.size(); i++)
	{
		splitRet2 = clUtils.SplitString(splitRet[i], "=");
	}
	*/


	/*
	// XXX 换行符
	char *strDelim = (char*)"=";
	char *strToken = NULL;
	char *nextToken = NULL;
	string sLine;
	vector <string> vecStringLine;
	vecStringLine.clear();
	string strBuf = clUtils.GetMsgValue(str_req, "Buf");

	strToken = strtok_s((char*)strBuf.c_str(), strDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		vecStringLine.push_back(sLine);
		strToken = strtok_s(NULL, strDelim, &nextToken);
	}
	cout << vecStringLine.size() << endl;
	*/

	// 时间处理
	/*
	if (clUtils.bCheckDate(str_req, 0, 15))
	{
		LOG(INFO) << "打印日志";
		cout << str_req.substr(0, 15) << " 转换为毫秒: " << clUtils.StringToMs(str_req, 0, 15) << endl;
	}
	else
	{
		cout << "非正常日期时间" << endl;
	}
	*/


	// 按行读入vector
	/*
	ifstream in(filename, ios::in | ios::binary | ios::ate);
	in.seekg(0, ios::beg);
	string s;
	vector<string> sevc;
	while(getline(in, s))
	{
		sevc.push_back(s);
	}

	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		cout << sevc[i] << endl;
	}
	*/


	// 分块读vec
	/*
	file.seekg(0, ios::end);
	streamoff len = file.tellg();

	file.seekg(0);
	file.read(loadedFile2[step], len);

	char *filebuffer = loadedFile2[step]; // 缓冲块首地址

	string sfilebuffer = filebuffer;
	string sLineBuffer = sfilebuffer.substr(0, 2427 + 1); // XXX 测试的时候,这个分块要从multi_thread中获取到的
	char *pLineBuffer = (char*)sLineBuffer.data();

	char *strDelim = (char*)"\r\n"; // 缓冲块中的分隔符
	char *strToken = NULL;// 将被处理的字符串
	char *nextToken = NULL; //保存缓冲块中下一个将被读取的单词的位置

	string s; // 保存分割出来的字符串
	vector<string> sevc;
	sevc.clear();

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		s.assign(strToken);
		sevc.push_back(s);
		strToken = strtok_s(NULL, strDelim, &nextToken); // 若第一个参数为空值NULL，则函数保存的指针SAVE_PTR在下一次调用中将作为起始位
	}
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		// cout << sevc[i] << endl;
	};
	*/


	// 日志行到 unordered_map
	/*
	unordered_multimap<string, string> mymap;
	map<string, string>::iterator curr;

	auto end = mymap.end();
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		string MsgId = GetMsgValue(sevc[i], "MsgId");
		mymap.insert(pair<string, string>(MsgId, sevc[i]));
	}

	//  遍历所有map, 对桶元素数量小于2的，认为缺失req或ans
	cout << "共读入: " << sevc.size() << "行" << endl;
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\n缺失应答串的MsgId： \t" << GetMsgValue(begin->second, "MsgId") << endl << endl;
			cout << endl << begin->second << endl << endl;
		}
	}
	*/


	// 获取当前时间
	/*
	cout << "当前时间(毫秒): " << clUtils.GetCurrentTimeMs() << endl;
	*/

	// libcurl
	/*
	CURL *curl = nullptr;
	CURLcode res;
	// 得到 easy interface 型指针
	curl = curl_easy_init();
	if (curl != nullptr)
	{
		// curl_easy_setopt 设置传输选项
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
		// example.com is redirected, so we tell libcurl to follow redirection
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		// Perform the request, res will get the return code

		// 完成传输任务
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		// 释放内存
		curl_easy_cleanup(curl);
	}
	*/


	// clUtils.DoPost

	/*
	// 发送数据的格式是:name=value&name2=value2&name3=value3";
	char *pData = (char*)"username=870131615@qq.com&password=159357yp";
	// pData = (char*)"maintype=10001&subtype=100&appver=2.5.19.3753&sysver=Microsoft Windows 7&applist=100:15,200:2&sign=2553d888bc922275eca2fc539a5f0c1b";
	utileError = clUtils.DoPost(pData, strResp);
	if (UTILS_RTMSG_OK != utileError)
	{
		cout << "POST 请求失败, 错误码: " << utileError << endl;
	}
	else
	{
		cout << "发post成功" << endl << "收到回复数据为:\n" << strResp << endl;
	}
	*/

	// 清理
	delete loadedFile2[0];
	delete loadedFile2[1];
	system("pause");
	return 0;
}


void getGlogFilename()
{
	struct ::tm tm_time;
	time_t timestamp = time(NULL);

#ifdef OS_IS_LINUX
	localtime_r(&timestamp, &tm_time);
#else
	localtime_s(&tm_time, &timestamp);
#endif

	ostringstream time_pid_stream;
	time_pid_stream.fill('0');
	time_pid_stream << 1900 + tm_time.tm_year
		<< setw(2) << 1 + tm_time.tm_mon
		<< setw(2) << tm_time.tm_mday
		<< '-'
		<< setw(2) << tm_time.tm_hour
		<< setw(2) << tm_time.tm_min
		<< setw(2) << tm_time.tm_sec
		<< '.'
		<< GetCurrentThreadId();

	const string& time_pid_string = time_pid_stream.str();
	// string string_filename = base_filename_ + filename_extension_ + time_pid_string;
	string string_filename = "INFO.log" + time_pid_string;
	char* filename = (char*)string_filename.c_str();
	cout << filename << endl;
}




int main(int argv, char* argc[])
{

	getGlogFilename();

	ifstream file;
	CUtils clUtils;
	CLbmRiskWarning clLbmRiskWarning(8);
	CGlog *p_glog = CGlog::GetInstance();

	while (true)
	{

		if (UTILS_RTMSG_OK != clUtils.LoadFile(file))
		{
			Sleep(2000);
			continue;
		}
		else
		{
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			LOG(INFO) << "==============-------------------------【" << clUtils.m_stCurrSysTime.pDate << "】的日志文件开始处理 -------------------------==============" << endl;
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			if (UTILS_RTMSG_OK == clLbmRiskWarning.multi_thread(file))
			{
				LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
				LOG(INFO) << "==============-------------------------【" << clUtils.m_stCurrSysTime.pDate << "】的日志文件处理完毕 -------------------------==============" << endl;
				LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
				// 当天的处理完了, 要开始处理第二天的文件了
				// CGlog *p_glog = CGlog::GetInstance();
				p_glog->CloseGlog();
				p_glog->InitGlog();
				LOG(INFO) << "新一天处理开始" << endl;

				continue;
			}
		}
	}

	// system("pause");
	return 0;
}
