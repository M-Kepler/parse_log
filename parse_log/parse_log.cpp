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
#include "inifile.h"
#include "multi_thread.h"

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

	multi_thread();
	system("pause");
	return 0;
}