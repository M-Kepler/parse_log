
#include <iostream>
#include <thread>
#include <mutex>
#include "inifile.h"
#include "multi_thread.h"
#include "utils.h"
#include "log.h"
#include <curl/curl.h>
#include "mylibcurl.h"

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

int main(int argv, char* argc[])
{
	bool step = 0;
	streamsize loadsize = 250000;
	loadedFile2[0] = new char[loadsize];
	loadedFile2[1] = new char[loadsize];
	UtilsError utileError;

	CGlog *p_glog = CGlog::GetInstance();

	string str_req = "20180430-211359-522345 18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	string str_ans = "20180719-094803-110762 12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, 业务程序运行正常, &_2=13863, 2, 21, 开户权限组<营业部>, 0, 10012, 2011-07-11 17:12:43.299541";

	// 读取文件
	CUtils clUtils;

	ifstream file;
	streamoff start = 0;
	streamsize size;
	int iLen;
	file.open(filename, ios::binary | ios::in);
	if (!file)
	{
		cout << "open file fail" << endl;
	}
	// multi_thread();


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
	const char* key_lbm = "LBMa";
	const char* str_split = ",";
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

