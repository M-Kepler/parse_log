#pragma once

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;


/*
 * @brief	   ������/Ӧ���в���key��Ӧ��valueֵ 
 * @param[in]  str_orig  ���ָ���ַ���
 * @param[in]  str_key   �����ҵ�key
 * @param[in]  str_split ��key��ʼ���ֵĵ�һ����ȡ������;
 * @return	   str_value ���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���; ����key��ʼ���ַ�������û���ҵ�str_split, �򷵻�key��ʼ���ַ�������
 */
string GetMsgValue(string str_orig, string str_key, string str_split=",");


/*
 * @brief       ��־���ַ���ת����ʱ��
 * @param[in]   str �ַ���(ǰ15λ��ʾʱ��,16-19������)
 * @return      time_t ���غ��뼶ʱ��
 */
time_t StringToMs(string str);


/*
* @brief        �ж��ַ���ǰlenλ�Ƿ�����Ч����
* @param[in]    str �ַ���
* @param[in]    len �ַ���ǰlenλ
* @return       bool ���򷵻� true, ���򷵻�false
*/
bool bCheckDate(string str, int len = 15);


