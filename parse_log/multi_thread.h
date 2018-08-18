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


int multi_thread();


/*
 * @brief       �� pFileBuffer �а���װ��vec
 * @param[in]   step			�����, loadedFile[step]
 * @param[in]   id				�̺߳� (XXX δ�õ�)
 * @param[in]   start			����ʼλ��
 * @param[in]   size			����С
 * @return      vector<string>	����string���͵�vec, ÿ��Ԫ����һ��
 */
vector<string> ReadLineToVec(int step, streamoff start, streamsize size, int id = 0);




/*
 * @brief		��ȡ�ļ��ٽ粻�ضϵ�������С
 * @param[in]	file			�򿪵��ļ�ָ��
 * @param[in]   start			����ʼλ��
 * @param[in]   size			����С
 * @return      streamsize		�����ʵ������С
 */
streamsize inline getRealSize(ifstream *file, streamoff start, streamsize size);




/*
 * @brief		�������ݵ�Buffer
 * @param[in]   step			���� Buffer ���±�
 * @param[in]   file			�򿪵��ļ�ָ��
 * @param[in]   start			����ʼλ��
 * @param[in]   size			����С
 * @return      ��				��ͨ��file�����ʶ��������
 */
void inline readLoad(int step, ifstream *file, streamoff start, streamsize size);




/*
 * @brief		�ضϼ��,������һ��, �����һ�����ݱ���
 * @param[in]   step			���� Buffer ���±�
 * @param[in]   start			��ƫ��λ��
 * @param[in]   size			����С
 * @return		streamsize		�����ʵ������С
 */
streamsize inline getBlockSize(int step, streamoff start, streamsize size);


/*
* @brief		����vector�е���־��
* @param[in]	sevc			���� Buffer ���±�
* @param[in]	KeyStr			�Ӵ���ָ�����ĸ���hashֵ��ΪKey
* @return		��
*/
void ParseMsgLine(vector<string> sevc, string KeyStr = "MsgId");



