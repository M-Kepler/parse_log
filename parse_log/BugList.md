
[TOC]

# BugList

## 偶发程序读到106015停止

* 【简述】：

* 【日期】：
	2018-08-19
* 【问题】：
	Debug模式下, 程序总会在读取到某一位置(如:106015)时卡住不动了; Release模式下则正常
* 【复现】：
	对应git版本:
	   ID		  作者			   日期
    8cbab89d	M_Kepler	2018/8/18 16:43:43
* 【过程】：
	debug模式下执行, 复现问题后中断调试后, 查看变量的值发现, mymap中很多元素都是**读取字符串的字符时出错**
* 【原因】：
	ParseMsgLine 作用是把 vecStr 的 MsgId 子串作为 key, vecStr 作为 value 插入到 unordered_multimap 中的,
	但是 ParseMsgLine 没有map的入参, map被定义成了一个全局变量; 
* 【修复】：
	ParseMsgLine 入参多加一个map, 把mymap定义为局部变量
* 【改动】：
    multi_thread.cpp
	multi_thread.h
* 【教训】：
 * 变量要初始化，要防范野指针
 [VS2013 调试时读取字符串的字符时出错](https://blog.csdn.net/lemonaha/article/details/55506830)
 * 特例:dll版本的问题
 [读取字符串的字符时出错](https://blog.csdn.net/wswxfwps/article/details/51419220)


 ## 
