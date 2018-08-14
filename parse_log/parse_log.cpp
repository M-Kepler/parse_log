/*
map结构:
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

* 先做3.1的日志格式
*/
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "inifile.h"
#include "multi_thread.h"
#include "utils.h"

using namespace std;
using namespace inifile;

string Section = "CONFIG";

int main(int argv, char* argc)
{
	IniFile ini;
	char filepath[] = "runlog_config.ini";
	ini.load(filepath);

	string Key_ScanTime = "ScanTime";
	string Value_ScanTime;
	ini.getValue(Section, Key_ScanTime, Value_ScanTime);
	cout << Key_ScanTime <<": " << Value_ScanTime << endl;


	string str_req = "20180430-211359-522345 18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	string str_ans = "20180719-094803-110762 12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, 业务程序运行正常, &_2=13863, 2, 21, 开户权限组<营业部>, 0, 10012, 2011-07-11 17:12:43.299541";

	const char* key_lbm = "LBM";
	const char* str_split = ",";
	string key_lbm_value = GetMsgValue(str_req, key_lbm, str_split);
	if (key_lbm_value != "")
	{
		cout << key_lbm << ": " << key_lbm_value << endl;
	}

	if (bCheckDate(str_req))
	{
		cout << str_req.substr(0, 15) << " 转换为毫秒: " << StringToMs(str_req) << endl;
	}
	else
	{
		cout << "非正常日期时间" << endl;
	}

	const char* filename = "runlog0-3.1.log";
	ifstream in(filename, ios::in | ios::binary | ios::ate);
	in.seekg(0, ios::beg);
	string s;
	vector<string> sevc;
	while(getline(in, s))
	{
		sevc.push_back(s);
	}

	/*
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		cout << sevc[i] << endl;
	}
	*/


	// multi_thread();
	system("pause");
	return 0;
}
