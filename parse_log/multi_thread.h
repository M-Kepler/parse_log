#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <time.h>
#include <thread>
// #include "utils.h"

using namespace std;


// Ϊ char* �����ṩ�ȼ�׼��
// ��֪����,������hashֵ��ͻ, �������hashֵ��ͬ�Ĳ�ͬ����
struct StrCmp
{
	bool operator()(const char *str1, const char *str2) const
	{
		return strcmp(str1, str2) == 0;
	}
};

// Ϊ char* �����ṩhash����
// ��֪����, �����ַ�������hashֵ
struct MsgIdHash
{
	// BKDR hash algorithm
	int operator()(char *str) const // char*
	{
		int seed = 131; // 31 131 1313 131313 etc..
		int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return hash & (0x7FFFFFFF);
	}
};

typedef unordered_map<char *, unsigned int, MsgIdHash, StrCmp> HashMap;
typedef unordered_map<char *, unsigned int, MsgIdHash, StrCmp>::iterator KeySet;


void multi_thread();


/*
 * @brief       �� pFileBuffer �а���װ��vec
 * @param[in]   iStep			�����, loadedFile[step]
 * @param[in]   llStart			����ʼλ��
 * @param[in]   llSize			����С
 * @return      vector<string>	����string���͵�vec, ÿ��Ԫ����һ��
 */
vector<string> ReadLineToVec(int iStep, streamoff llStart, streamsize llSize);




/*
 * @brief		��ȡ�ļ��ٽ粻�ضϵ�������С
 * @param[in]	file			�򿪵��ļ�ָ��
 * @param[in]   llStart			����ʼλ��
 * @param[in]   llSize			����С
 * @return      streamsize		�����ʵ������С
 */
streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize);




/*
 * @brief		�������ݵ�Buffer
 * @param[in]   iStep			���� Buffer ���±�
 * @param[in]   file			�򿪵��ļ�ָ��
 * @param[in]   llStart			����ʼλ��
 * @param[in]   llSize			����С
 * @return      ��				��ͨ��file�����ʶ��������
 */
void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize);




/*
 * @brief		�ضϼ��,������һ��, �����һ�����ݱ���
 * @param[in]   iStep			���� Buffer ���±�
 * @param[in]   llStart			��ƫ��λ��
 * @param[in]   llSize			����С
 * @return		streamsize		�����ʵ������С
 */
streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize);


/*
* @brief		����vector�е���־��
* @param[in]	vecStr			���� Buffer ���±�
* @param[in]	strKey			�Ӵ���ָ�����ĸ���hashֵ��ΪKey
* @return		��
*/
// void ParseMsgLine(unordered_multimap<string, string> &mymap, vector<string> vecStr, string strKey);
void ParseMsgLine(vector<string> vecStr, int id, string strKey = "MsgId");


void TimeoutScan(unordered_multimap<string, string> mymap);




