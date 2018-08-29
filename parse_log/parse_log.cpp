
#include <iostream>
#include <thread>
#include "inifile.h"
#include "multi_thread.h"
#include "utils.h"
#include "log.h"
#include <curl/curl.h>
#include "mylibcurl.h"

using namespace std;


char *loadedFile2[2];
const char* filepath = "runlog_config.ini";
const char* filename = "runlog0-3.1.log";



int main(int argv, char* argc[])
{
	bool step = 0;
	streamsize loadsize = 250000;
	loadedFile2[0] = new char[loadsize];
	loadedFile2[1] = new char[loadsize];
	UtilsError utileError;

	// ��ȡ�ļ�
	CUtils clUtils;

	ifstream file;
	file.open(filename, ios::binary | ios::in);
	if (!file)
	{
		cout << "open file fail" << endl;
	}

	string str_req = "20180430-211359-522345 18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	string str_ans = "20180719-094803-110762 12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, ҵ�������������, &_2=13863, 2, 21, ����Ȩ����<Ӫҵ��>, 0, 10012, 2011-07-11 17:12:43.299541";

	/* ���������ļ� */
	string strValue;
	if (UTILS_RTMSG_OK != clUtils.GetConfigValue(strValue, "ThreadCount"))
	{
		LOG(ERROR) << "��ȡ�����ļ�ֵ����" << endl;
	};

	/* ��ȡmsg��key��value */
	/*
	const char* key_lbm = "LBMa";
	const char* str_split = ",";
	string key_lbm_value = clUtils.GetMsgValue(str_req, key_lbm);
	if (key_lbm_value.empty())
	{
		cout << "û�ҵ�" << endl;
	}
	if (key_lbm_value != "")
	{
		cout << key_lbm << ": " << key_lbm_value << endl;
	}
	*/


	/* ʱ�䴦�� */
	/*
	CGlog *p_glog = CGlog::GetInstance();
	if (clUtils.bCheckDate(str_req, 0, 15))
	{
		LOG(INFO) << "��ӡ��־";
		cout << str_req.substr(0, 15) << " ת��Ϊ����: " << clUtils.StringToMs(str_req, 0, 15) << endl;
	}
	else
	{
		cout << "����������ʱ��" << endl;
	}
	*/

	/* ���ж���vector */
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


	/* �ֿ��vec */
	/*
	file.seekg(0, ios::end);
	streamoff len = file.tellg();

	file.seekg(0);
	file.read(loadedFile2[step], len);

	char *filebuffer = loadedFile2[step]; // ������׵�ַ

	string sfilebuffer = filebuffer;
	string sLineBuffer = sfilebuffer.substr(0, 2427 + 1); // XXX ���Ե�ʱ��,����ֿ�Ҫ��multi_thread�л�ȡ����
	char *pLineBuffer = (char*)sLineBuffer.data();

	char *strDelim = (char*)"\r\n"; // ������еķָ���
	char *strToken = NULL;// ����������ַ���
	char *nextToken = NULL; //���滺�������һ��������ȡ�ĵ��ʵ�λ��

	string s; // ����ָ�������ַ���
	vector<string> sevc;
	sevc.clear();

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		s.assign(strToken);
		sevc.push_back(s);
		strToken = strtok_s(NULL, strDelim, &nextToken); // ����һ������Ϊ��ֵNULL�����������ָ��SAVE_PTR����һ�ε����н���Ϊ��ʼλ
	}
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		// cout << sevc[i] << endl;
	};
	*/


	/* ��־�е� unordered_map */
	/*
	unordered_multimap<string, string> mymap;
	map<string, string>::iterator curr;

	auto end = mymap.end();
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		string MsgId = GetMsgValue(sevc[i], "MsgId");
		mymap.insert(pair<string, string>(MsgId, sevc[i]));
	}

	//  ��������map, ��ͰԪ������С��2�ģ���Ϊȱʧreq��ans
	cout << "������: " << sevc.size() << "��" << endl;
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\nȱʧӦ�𴮵�MsgId�� \t" << GetMsgValue(begin->second, "MsgId") << endl << endl;
			cout << endl << begin->second << endl << endl;
		}
	}
	*/


	// ��ȡ��ǰʱ��
	cout << "��ǰʱ��(����): " << clUtils.GetCurrentTimsMS() << endl;


	// libcurl
	// curl ��ʼ��
	/*
	CURL *curl = nullptr;
	CURLcode res;
	// �õ� easy interface ��ָ��
	curl = curl_easy_init();
	if (curl != nullptr)
	{
		// curl_easy_setopt ���ô���ѡ��
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.baidu.com");
		// example.com is redirected, so we tell libcurl to follow redirection
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		// Perform the request, res will get the return code

		// ��ɴ�������
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		// �ͷ��ڴ�
		curl_easy_cleanup(curl);
	}
	*/

	string strRespData;
	string strPort;
	string strTimeOut;
	string strHttpUrl; // http ��ַ
	string strHttpHeader; // http ����ͷ
	string strHttpRepeatNum; // ��ʱ�ط�����
	string strHttpTimeOut; // ��ʱʱ��
	UtilsError enumUtilsError;

	if ((enumUtilsError = clUtils.GetConfigValue(strHttpUrl, "HttpUrl", "CURL")) != UTILS_RTMSG_OK
		|| (enumUtilsError = clUtils.GetConfigValue(strPort, "HttpPort", "CURL")) != UTILS_RTMSG_OK
		|| (enumUtilsError = clUtils.GetConfigValue(strTimeOut, "HttpTimeOut", "CURL")) != UTILS_RTMSG_OK
		|| (enumUtilsError = clUtils.GetConfigValue(strHttpHeader, "HttpHeader", "CURL")) != UTILS_RTMSG_OK
		|| (enumUtilsError = clUtils.GetConfigValue(strHttpRepeatNum, "HttpRepeatNum", "CURL")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << enumUtilsError << endl;
		// FIXME
		// �쳣�׳�������
		abort();
	}


	string strResp;
	// clUtils.DoPost(int iPost, int iTimeout, const char* cookieFilePath, char * pData, char* pUrl, string &strResp);
	char *pData = NULL;
	char *pUrl = (char*)strHttpUrl.c_str();
	int iPort = stoi(strPort);
	int iTimeOut = stoi(strTimeOut);
	pData = (char*)"username=870131615@qq.com&password=159357yp";
	// pData = (char*)"maintype=10001&subtype=100&appver=2.5.19.3753&sysver=Microsoft Windows 7&applist=100:15,200:2&sign=2553d888bc922275eca2fc539a5f0c1b";

	// utileError = clUtils.DoPost(iPort, iTimeOut, pData, pUrl, strResp);
	utileError = clUtils.DoPost(80, 1000, pData, pUrl, strResp);
	if (UTILS_RTMSG_OK == utileError)
	{
		cout << "��post�ɹ�" << endl << "�յ��ظ�����Ϊ:\n" << strResp << endl;
	}

	// multi_thread();
	delete loadedFile2[0];
	delete loadedFile2[1];
	system("pause");
	return 0;
}

