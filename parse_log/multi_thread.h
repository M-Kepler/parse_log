#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <time.h>
#include <thread>
using namespace std;

/*
 * @brief       1
 * @param[in]   2
 * @return      3
 */

int multi_thread();


/*
 * @brief       从 pFileBuffer 中按行装入vec
 * @param[in]   int				缓存块, loadedFile[step]
 * @param[in]   streamoff		开始位置
 * @param[in]   streamsize		结束位置
 * @return      vector<string>	返回string类型的vec, 每个元素是一行
 */
vector<string> ReadLineToVec(int, int, streamoff, streamsize);


/*
 * @brief       获取文件临界不截断的真正大小
 * @param[in]   ifstream	打开的文件指针
 * @param[in]   streamoff	流开始位置
 * @param[in]   streamsize	流大小
 * @return      streamsize	检查后实际的流大小
 */
streamsize inline getRealSize(ifstream *, streamoff, streamsize);


/*
 * @brief       文件读入到堆
 * @param[in]   ifstream	打开的文件指针
 * @param[in]   streamsize	流大小
 * @return      
 */
void inline readLoad(int, ifstream *, streamoff, streamsize);


/*
 * @brief       截断检查,往后多读一点, 避免把一行数据被拆开
 * @param[in]   streamoff	流偏移位置
 * @param[in]   streamsize	流大小
 * @return      streamsize	检查后的实际流大小
 */
streamsize inline getBlockSize(int, streamoff, streamsize);


void readBlock(int, int, streamoff, streamsize);