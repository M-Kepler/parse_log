#pragma once

#include <time.h>
#include <atltime.h>
#include <iostream>
#include <string>

using namespace std;

/*
 * @brief	  从请求/应答串中查找key对应的value值 
 * @param[in] str_orig  待分割的字符串
 * @param[in] str_key   待查找的key
 * @param[in] str_split 从key开始出现的第一个截取结束符;
 * @return	  str_value 查找成功返回value,查找失败返回空字符串; 若从key开始到字符串结束没有找到str_split, 则返回key开始到字符串结束
 */
string get_msg_value(string str_orig, string str_key, string str_split);


/*
 * @brief     日志行字符串转毫秒时间
 * @param[in] string 字符串(前15位表示时间,16-19。。。)
 * @return    time_t 返回毫秒级时间
 */
time_t string2ms(string str);

/*
* @brief     判断字符串前len位是否是有效日期
* @param[in] string 字符串
* @param[in] int 字符串前len位
* @return    bool 是则返回 true, 否则返回false
*/
bool bCheckDate(string str, int len = 15);

/*
 * @brief     发送http请求
 * @param[in]
 * @return
 */

