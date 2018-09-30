#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <time.h>

#include <thread>
#include <future>
#include "ThreadPool.h"

#include "utils.h"
#include "define.h"

#ifdef OS_IS_LINUX
#else
#include <Windows.h>
#endif

using namespace std;


/*
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
*/


typedef unordered_multimap<string, string> LogMap;
typedef unordered_multimap<string, string>::iterator LogMapKeySet;

class CLbmRiskWarning
{
public:
	CLbmRiskWarning();
	~CLbmRiskWarning();

	/*
	 * @brief       ��Ҫ������
	 * @return      return			return_command
	 */
	UtilsError multi_thread(ifstream& file);

	/*
	 * @brief	�� pFileBuffer �а���װ��vec
	 * @param   iStep			�����, loadedFile[step]
	 * @param   llStart			����ʼλ��
	 * @param   llSize			����С
	 * @return	vector<string>	����string���͵�vec, ÿ��Ԫ����һ��
	 */
	vector<string> ReadLineToVec(int iStep, streamoff llStart, streamsize llSize);

	/*
	 * @brief	��ȡ�ļ��ٽ粻�ضϵ�������С
	 * @param	file			�򿪵��ļ�ָ��
	 * @param   llStart			����ʼλ��
	 * @param   llSize			����С
	 * @return	streamsize		�����ʵ������С
	 */
	streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize);
	// streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize);

	/*
	 * @brief	�������ݵ�Buffer
	 * @param   iStep			���� Buffer ���±�
	 * @param   file			�򿪵��ļ�ָ��
	 * @param   llStart			����ʼλ��
	 * @param   llSize			����С
	 * @return	��				��ͨ��file�����ʶ��������
	 */
	void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize);

	/*
	 * @brief	�ضϼ��,������һ��, �����һ�����ݱ���
	 * @param   iStep			���� Buffer ���±�
	 * @param   llStart			��ƫ��λ��
	 * @param   llSize			����С
	 * @return	streamsize		�����ʵ������С
	 */
	streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize);

	/*
	* @brief	��vector�е���־�в��뵽map��
	* @param	vecStr			vector
	* @param	strKey			�Ӵ���ָ�����ĸ���hashֵ��ΪKey
	* @return	��
	*/
	void ParseMsgLine(vector<string> vecStr, int id, string strKey);

	/*
	 * @brief	��ʱɨ��map
	 * @param   mymap			������־��map
	 * @param   iAnsNum			һ��req�ɶ�Ӧ��ans��������
	 * @return	return			return_command
	 */
	 // void TimeoutScan(unordered_multimap<string, string> &mymap, int iAnsNum, std::promise<string> prom);
	void TimeoutScan(unordered_multimap<string, string> &mymap, int iAnsNum);

	/*
	 * @brief	�ؼ�������
	 * @param   file			�򿪵��ļ�
	 * @param   llFileSize		Ҫ������ļ���С
	 * @param   pCurrPos		Ҫ������ļ��Ŀ�ʼָ��
	 * @param   strLoadSize		�ƻ�һ�μ��ش�С
	 * @param   llMaxSize		ʵ��һ�οɼ��ش�С(����λ��Ĵ�С)
	 * @param   llStart			�ļ�����ʼָ��
	 * @param   iThread			�����߳���
	 * @param   iAnsNum			һ��req�ɶ�Ӧ��ans��������
	 * @param   iScanTime		�ļ����ɨ��ʱ��
	 * @return	return			return_command
	 */
	void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime);
	
public:
	
private:
	int m_iThreadPoolCount; // �̳߳��߳�����
	char* m_pLoadedFile[2]; // ���ָ�� char* ���͵�ָ�������

	LogMap *m_pLogMaps;
	LogMap m_allLogMap;
	LogMap *m_pThreadMap;

	UtilsError m_utilsError;

	CUtils m_clUtils;
	ThreadPool m_clThreadPool;

	vector<string> vecThreadLines, vecEndThreadLines;
	vector< future<string> > m_vecWebServiceRet;

};


// ���ڶ���ĳ�Ա��������������

streamsize CLbmRiskWarning::getRealSize(ifstream *file, streamoff llStart, streamsize llSize)
{
	char chCurr[1] = { 0 };
	file->clear();
	file->seekg(llStart + llSize);

	while (file->get() != '\n')
	{
		llSize++;
	}

	return llSize;
}


streamsize CLbmRiskWarning::getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = m_pLoadedFile[iStep] + llStart + llSize;
	// while (*p != '\n')// ����\n����Ϊû��\nʱ������ѭ��, ���羲̬�ļ�ֻ��һ������, ���Լ���\0һ�����ж�

	while (*p != '\n' && *p != '\0')
	{
		++llSize;
		++p;
	}
	// tset begin
	llSize += 1; // ��������\n�ַ�
	// test end
	return llSize;
}


void CLbmRiskWarning::readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize)
{
	// file->clear();
	file->seekg(llStart);
	// file->seekg(llStart);
	file->read(m_pLoadedFile[iStep], llSize);
}

