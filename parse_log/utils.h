#pragma once

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>

using namespace std;

/*
 * @brief	  ������/Ӧ���в���key��Ӧ��valueֵ 
 * @param[in] str_orig  ���ָ���ַ���
 * @param[in] str_key   �����ҵ�key
 * @param[in] str_split ��key��ʼ���ֵĵ�һ����ȡ������;
 * @return	  str_value ���ҳɹ�����value,����ʧ�ܷ��ؿ��ַ���; ����key��ʼ���ַ�������û���ҵ�str_split, �򷵻�key��ʼ���ַ�������
 */
string get_msg_value(string str_orig, string str_key, string str_split);


/*
 * @brief     ��־���ַ���ת����ʱ��
 * @param[in] string �ַ���(ǰ15λ��ʾʱ��,16-19������)
 * @return    time_t ���غ��뼶ʱ��
 */
time_t string2ms(string str);

/*
* @brief     �ж��ַ���ǰlenλ�Ƿ�����Ч����
* @param[in] string �ַ���
* @param[in] int �ַ���ǰlenλ
* @return    bool ���򷵻� true, ���򷵻�false
*/
bool bCheckDate(string str, int len = 15);

/*
 * @brief     ����http����
 * @param[in]
 * @return
 */

