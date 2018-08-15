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
 * @brief       �� pFileBuffer �а���װ��vec
 * @param[in]   int				�����, loadedFile[step]
 * @param[in]   streamoff		��ʼλ��
 * @param[in]   streamsize		����λ��
 * @return      vector<string>	����string���͵�vec, ÿ��Ԫ����һ��
 */
vector<string> ReadLineToVec(int, int, streamoff, streamsize);


/*
 * @brief       ��ȡ�ļ��ٽ粻�ضϵ�������С
 * @param[in]   ifstream	�򿪵��ļ�ָ��
 * @param[in]   streamoff	����ʼλ��
 * @param[in]   streamsize	����С
 * @return      streamsize	����ʵ�ʵ�����С
 */
streamsize inline getRealSize(ifstream *, streamoff, streamsize);


/*
 * @brief       �ļ����뵽��
 * @param[in]   ifstream	�򿪵��ļ�ָ��
 * @param[in]   streamsize	����С
 * @return      
 */
void inline readLoad(int, ifstream *, streamoff, streamsize);


/*
 * @brief       �ضϼ��,������һ��, �����һ�����ݱ���
 * @param[in]   streamoff	��ƫ��λ��
 * @param[in]   streamsize	����С
 * @return      streamsize	�����ʵ������С
 */
streamsize inline getBlockSize(int, streamoff, streamsize);


void readBlock(int, int, streamoff, streamsize);