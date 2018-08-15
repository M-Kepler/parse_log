
#include "utils.h"


string GetMsgValue(string str_orig, string str_key, string str_split)
{
	string str_value = "";
	int str_orig_len;
	int key_len;
	size_t pos_key_begin;
	size_t pos_key_end;
	size_t pos_str_split;

	str_orig_len = str_orig.length();
	key_len = str_key.length();
	pos_key_begin = str_orig.find(str_key);

	if (pos_key_begin != string::npos)
	{
		// ��key��λ�ÿ�ʼ,��һ�γ��� str_split ��λ��
		pos_str_split  =  str_orig.substr(pos_key_begin).find(str_split);
		if (pos_str_split != string::npos)
		{
			pos_key_end = pos_key_begin + pos_str_split;
		}
		else
		{
			pos_key_end = str_orig_len;
		}
		int pos_begin = pos_key_begin + key_len + 1; // +1 ����'='�ַ�
		int value_len = pos_key_end - pos_begin;
		str_value = str_orig.substr(pos_begin, value_len);
		return str_value;
	}
	return str_value;
}


time_t StringToMs(string str_orig)
{
	tm tm_;
	string str;
	int year, month, day, hour, minute, second;
	char * formate;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = str_orig.substr(0, 15);
	sscanf_s(str.c_str(), formate, &year, &month, &day, &hour, &minute, &second);

	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;

	time_t tm_s = mktime(&tm_);
	time_t tm_ms = (tm_s * 1000) + stoi(str_orig.substr(16, 3)); // ת��Ϊ����
	return tm_ms;
}


bool bCheckDate(string str_orig, int len)
{
	char* formate;
	string str;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;

	formate = (char*)"%4d%2d%2d-%2d%2d%2d";
	str = str_orig.substr(0, len);
	sscanf_s(str.c_str(), formate, &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);

	if (nYear < 1970 || nYear > 3000 || nMonth < 1 || nMonth> 12 || nDay < 1 || nDay  > 31
		|| nHour < 0 || nHour > 23 || nMinute < 0 || nMinute>59 || nSecond < 0 || nSecond>59)
	{
		return false;
	}
	CTime t(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	// CTime��Է����������ڽ��н�λ����, ��4��31��ת��Ϊ5��1��,��������з���ת������Ϊ����������

	return (nYear == t.GetYear()
		&& nMonth == t.GetMonth()
		&& nDay == t.GetDay()
		&& nHour == t.GetHour()
		&& nMinute == t.GetMinute()
		&& nSecond == t.GetSecond()
		);
}
