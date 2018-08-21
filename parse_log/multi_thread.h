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


void multi_thread();


/*
 * @brief       从 pFileBuffer 中按行装入vec
 * @param[in]   iStep			缓存块, loadedFile[step]
 * @param[in]   llStart			流开始位置
 * @param[in]   llSize			流大小
 * @return      vector<string>	返回string类型的vec, 每个元素是一行
 */
vector<string> ReadLineToVec(int iStep, streamoff llStart, streamsize llSize);




/*
 * @brief		获取文件临界不截断的真正大小
 * @param[in]	file			打开的文件指针
 * @param[in]   llStart			流开始位置
 * @param[in]   llSize			流大小
 * @return      streamsize		检查后的实际流大小
 */
streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize);




/*
 * @brief		读入数据到Buffer
 * @param[in]   iStep			缓存 Buffer 的下标
 * @param[in]   file			打开的文件指针
 * @param[in]   llStart			流开始位置
 * @param[in]   llSize			流大小
 * @return      无				可通过file来访问读入的数据
 */
void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize);




/*
 * @brief		截断检查,往后多读一点, 避免把一行数据被拆开
 * @param[in]   iStep			缓存 Buffer 的下标
 * @param[in]   llStart			流偏移位置
 * @param[in]   llSize			流大小
 * @return		streamsize		检查后的实际流大小
 */
streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize);


/*
* @brief		处理vector中的日志行
* @param[in]	vecStr			缓存 Buffer 的下标
* @param[in]	strKey			从串中指定用哪个的hash值作为Key
* @return		无
*/
// void ParseMsgLine(unordered_multimap<string, string> &mymap, vector<string> vecStr, string strKey);
void ParseMsgLine(vector<string> vecStr, int id, string strKey = "MsgId");


void TimeoutScan(unordered_multimap<string, string> mymap);




