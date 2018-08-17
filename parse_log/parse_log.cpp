/*
map�ṹ:
{
	"msgid": {
		"req": {
			"req_msgid": {
				"LBM": "",
				"str_req_time": "",
				"ll_req_time": "",
				"msg_detail": ""
			}
		},
		"ans": {
			"ans_msgid": {
				"ll_ans_time": ""
			}
		}
	}
}

* ����3.1����־��ʽ
*/

#include <iostream>
#include <thread>
#include "inifile.h"
#include "multi_thread.h"
#include "utils.h"
using namespace std;
using namespace inifile;


char *loadedFile2[2];
string Section = "CONFIG";
const char* filepath = "runlog_config.ini";
const char* filename = "runlog-test.log";


int main(int argv, char* argc)
{
	bool step = 0;
	streamsize loadsize = 250000;
	loadedFile2[0] = new char[loadsize];
	loadedFile2[1] = new char[loadsize];

	// ��ȡ�ļ�
	ifstream file;
	file.open(filename, ios::binary | ios::in);
	if (!file)
	{
		cout << "open file fail" << endl;
	}

	string str_req = "20180430-211359-522345 18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	string str_ans = "20180719-094803-110762 12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, ҵ�������������, &_2=13863, 2, 21, ����Ȩ����<Ӫҵ��>, 0, 10012, 2011-07-11 17:12:43.299541";

	/* ���������ļ� */
	/*
	IniFile ini;
	ini.load(filepath);
	string Key_ScanTime = "ScanTime";
	string Value_ScanTime;
	ini.getValue(Section, Key_ScanTime, Value_ScanTime);
	cout << Key_ScanTime <<": " << Value_ScanTime << endl;
	*/


	/* ��ȡmsg��key��value */
	/*
	const char* key_lbm = "LBM";
	const char* str_split = ",";
	string key_lbm_value = GetMsgValue(str_req, key_lbm, str_split);
	if (key_lbm_value != "")
	{
		cout << key_lbm << ": " << key_lbm_value << endl;
	}
	*/


	/* ����ʱ�� */
	/*
	if (bCheckDate(str_req))
	{
		cout << str_req.substr(0, 15) << " ת��Ϊ����: " << StringToMs(str_req) << endl;
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


	// TODO 20180816
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

	multi_thread();

	delete loadedFile2[0];
	delete loadedFile2[1];
	system("pause");
	return 0;
}
