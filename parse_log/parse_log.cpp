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
#include "inifile.h"
#include "multi_thread.h"
#include "utils.h"

using namespace std;
using namespace inifile;

string Section = "CONFIG";

void output(int i)
{
	cout << i << endl;
}



int main(int argv, char* argc)
{
	/*
	IniFile ini;
	char filepath[] = "runlog_config.ini";
	ini.load(filepath);

	for (size_t i = 0; i < 4; i++)
	{
		thread t(output, i);
		t.join();
	}

	string Key_ScanTime = "ScanTime";
	string Value_ScanTime;
	ini.getValue(Section, Key_ScanTime, Value_ScanTime);
	cout << Value_ScanTime << endl;
	*/

	// multi_thread();


	string str_req = "20181222-091309-522345 18225    99 Req: LBM=L0301002, MsgId=0000000100F462171E4D4B25, Len=299, Buf=_ENDIAN=0&F_OP_USER=9999&F_OP_ROLE=2&F_SESSION=0123456789&F_OP_SITE=0050569e247d&F_OP_BRANCH=999&F_CHANNEL=0&USE_NODE_FUNC=106127&CUSTOMER=150165853&MARKET=1&BOARD=0";
	string str_ans = "20180719-094803-110762 12343    98 Ans: LBM=L1160005, MsgId=000001050001D55B1F297DD2, Len=792, Cost=161, Buf=&_1=0, 0, 业务程序运行正常, &_2=13863, 2, 21, 开户权限组<营业部>, 0, 10012, 2011-07-11 17:12:43.299541";
	// string str_ans = "20180719-094819-891374 12347    98 Ans: LBM=L1190271, MsgId=000001050001D5871F297DE3, Len=283, Cost=12, Buf=&_1=0, 100, 业务程序运行正常，没有查询数据, ";

	char *key_lbm = (char *)"Buf";
	string key_ret_code = "LBM";
	string key_lbm_value = get_msg_value(str_req, key_ret_code , ",");
	if (key_lbm_value != "")
	{
		cout << key_lbm_value << endl;
	}
	// char * str_time = (char*) "2012-01-01 14:00:00";
	if (bCheckDate(str_req))
	{
		cout << string2ms(str_req) << endl;
	}

	system("pause");
	return 0;
}