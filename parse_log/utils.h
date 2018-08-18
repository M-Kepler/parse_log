#pragma once

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;


/*
 * @brief	   ������/Ӧ���в���key��Ӧ��valueֵ 
 * @param[in]  strOrig			���ָ���ַ���
 * @param[in]  strKey			�����ҵ�key
 * @param[in]  strSplit			��key��ʼ���ֵĵ�һ����ȡ������;
 * @return	   strRetValue		���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���; ����key��ʼ���ַ�������û���ҵ�str_split, �򷵻�key��ʼ���ַ�������
 */
string strGetMsgValue(string strOrig, string strKey, string strSplit=",");


/*
 * @brief       ��־���ַ���ת����ʱ��
 * @param[in]   str				�ַ���(ǰ15λ��ʾʱ��,16-19������)
 * @return      time_t			���غ��뼶ʱ��
 */
time_t StringToMs(string str);


/*
* @brief        �ж��ַ���ǰlenλ�Ƿ�����Ч����
* @param[in]    str				�ַ���
* @param[in]    len				�ַ���ǰlenλ
* @return       bool			���򷵻� true, ���򷵻�false
*/
bool bCheckDate(string str, int len = 15);


