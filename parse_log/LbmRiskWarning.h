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
#include <Windows.h>
#include "utils.h"
#include "define.h"

 using namespace std;


/*
// 为 char* 类型提供等价准则
// 告知容器,当发生hash值冲突, 如何区分hash值相同的不同对象
struct StrCmp
{
	bool operator()(const char *str1, const char *str2) const
	{
		return strcmp(str1, str2) == 0;
	}
};


// 为 char* 类型提供hash函数
// 告知容器, 用哪种方法计算hash值
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


int multi_thread();


/*
 * @brief	从 pFileBuffer 中按行装入vec
 * @param   iStep			缓存块, loadedFile[step]
 * @param   llStart			流开始位置
 * @param   llSize			流大小
 * @return	vector<string>	返回string类型的vec, 每个元素是一行
 */
vector<string> ReadLineToVec(int iStep, streamoff llStart, streamsize llSize);


/*
 * @brief	获取文件临界不截断的真正大小
 * @param	file			打开的文件指针
 * @param   llStart			流开始位置
 * @param   llSize			流大小
 * @return	streamsize		检查后的实际流大小
 */
streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize);


/*
 * @brief	读入数据到Buffer
 * @param   iStep			缓存 Buffer 的下标
 * @param   file			打开的文件指针
 * @param   llStart			流开始位置
 * @param   llSize			流大小
 * @return	无				可通过file来访问读入的数据
 */
void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize);


/*
 * @brief	截断检查,往后多读一点, 避免把一行数据被拆开
 * @param   iStep			缓存 Buffer 的下标
 * @param   llStart			流偏移位置
 * @param   llSize			流大小
 * @return	streamsize		检查后的实际流大小
 */
streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize);


/*
* @brief	把vector中的日志行插入到map中
* @param	vecStr			vector
* @param	strKey			从串中指定用哪个的hash值作为Key
* @return	无
*/
void ParseMsgLine(vector<string> vecStr, int id, string strKey);


/*
 * @brief	定时扫描map
 * @param   mymap			所有日志的map
 * @param   iAnsNum			一条req可对应的ans串的数量
 * @return	return			return_command
 */
void TimeoutScan(unordered_multimap<string, string> &mymap, int iAnsNum);

/*
 * @brief	关键处理函数
 * @param   file			打开的文件
 * @param   llFileSize		要处理的文件大小
 * @param   pCurrPos		要处理的文件的开始指针
 * @param   strLoadSize		计划一次加载大小
 * @param   llMaxSize		实际一次可加载大小(防截位后的大小)
 * @param   llStart			文件处理开始指针
 * @param   iThread			处理线程数
 * @param   iAnsNum			一条req可对应的ans串的数量
 * @param   iScanTime		文件间隔扫描时间
 * @return	return			return_command
 */
void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime);