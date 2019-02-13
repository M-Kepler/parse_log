
[TOC]

### BugList


### 模板

* 【日期】：
* 【问题】：
* 【由来】：
* 【原因】：
* 【修复】：
* 【改动】：
* 【教训】：


### Debug下偶发程序读到106015停止


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
	多线程模式下, 对map进行插入操作会有问题, STL的容器都不是线程安全的

* 【修复】：
	每个线程对各自的map进行插入操作, 最后再把所有的map组合起来
	**但是** 在改之前时间在600ms左右, 现在线程插入完后还要遍历组合, 时间是1000ms左右...

* 【改动】：
    multi_thread.cpp
	multi_thread.h

* 【教训】：
 * 变量要初始化，要防范野指针
 [VS2013 调试时读取字符串的字符时出错](https://blog.csdn.net/lemonaha/article/details/55506830)
 * 特例:dll版本的问题
 [读取字符串的字符时出错](https://blog.csdn.net/wswxfwps/article/details/51419220)
 * STL的map不是线程安全的, 多线程下很容易爆发问题
 [STL容器是否是线程安全的](https://www.cnblogs.com/ztteng/archive/2013/11/07/3411738.html)
 * 如果还有多线程同时读写, 写要注意线程安全问题, 读还要注意线程同步问题


 ### 文件只有一行且末尾没有换行符时

* 【日期】：20180905-14:20
* 【问题】：当文件只有一行,且莫行没有换行符时, 用seekg(0, ios::end)无法取得指向文件末尾的指针
* 【由来】：
* 【原因】：https://blog.csdn.net/super_zq/article/details/78251192
* 【修复】：在seekg之前先clear
* 【改动】：
* 【教训】：


### 获取文件最后一行时, 同一程序linux下和windows下运行结果不一样, windows下会多读几个字符

* 【日期】：20180909-13:27
* 【问题】：同样一份代码操作相同的文件,在linux下用g++(version:5.4)和windows下visual studio2017运行结果有差错
```c++
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

void readback(int lineCount)
{
    std::ifstream  fin("./test.log", std::ios::ate);
    if (!fin)
    {
        cerr << "打开日志文件失败！";
    }

    // 先倒回文件末尾两个字符
    streampos now1, now2, now3, now4;
    fin.seekg(-2, fin.cur);
    now1 = fin.tellg();
    // 假定反向读取100行记录
    for (int i = 0; i < lineCount; i++)
    {
        // 查看前一个字符是否为回车符
        while (fin.peek() != fin.widen('\n'))
        {
            fin.seekg(-1, fin.cur);
            now2 = fin.tellg();
        }
        // 走到这里表示跳过一行了，所以继续跳直到够100行
        now3 = fin.tellg();
        fin.seekg(-1, fin.cur);
        now4 = fin.tellg();
        cout << now1 << "\t" << now2 << "\t" << now3 << "\t" << now4 << endl;
    }

    fin.seekg(2, fin.cur);
    // 现在文件指针指向99行的末尾，可以读取了
    vector<string> result;
    std::string  line;
    while (getline(fin, line))
    {
        cout << line << endl;
        result.push_back(line);
    }
    fin.clear();
    fin.close();
    // </string>
}
int main(int argc, char **argv)
{
    readback(atoi(argv[1]));
    return 0;
}
```

* 【由来】：
windows下运行结果:
```
10012573        10012434        10012431        10012430
22
MsgId=0D2C8E8E16BF4E7EADA4684D7FA29E9A,20180908-201040-624,Req:144623
MsgId=0D2C8E8E16BF4E7EADA4684D7FA29E9A,20180908-201040-624,Ans:144624
```
linux下运行结果:
```
10012573        10012504        10012504        10012503
MsgId=0D2C8E8E16BF4E7EADA4684D7FA29E9A,20180908-201040-624,Ans:144624
```

* 【修复】：
```
	// file.seekg(-2, file.cur);
	file.seekg(-2, ios::end); //倒回最后两个字符处

	for (i = 0; i < iLineNum; i++)
	{
		while (file.peek() != file.widen('\n'))
		{
			nCurentPos = file.tellg();
			if (nCurentPos == 0)
			{
				break;
			}
			file.seekg(-1, file.cur);
		}
		//读到"\n"标识 ，表示已经有一行了
		if (nCurentPos != 0)//倒回文件的开头，停止倒退
		{
			file.seekg(-1, file.cur);
		}
		else
		{
			break;
		}
	}
	if (nCurentPos != 0)
	{
		file.seekg(2, file.cur);
	}
```
* 【改动】：

* 【教训】：
	linux和windows的编码要注意一下


 ### 文件产生速度太快时, 如每2毫秒插入一行req一行ans时, 会掉进getRealSize函数的死循环中

 > 不是这个原因

* 【日期】：20180909-23:48
* 【问题】：
```
	while循环条件:
	1. while(file->get() != '\n'); // 不适用与只有一行的情况,只有一行的话,末尾是没有\n的
	2. while ((file->get() != '\n') || (file->get() != '\0')) // 当文件2毫秒产生两条数据的时候, 会陷入因为get()得到的字符是EOF而死循环
	3. while ((file->peek() != EOF) && ((file->get()!= '\n') || (file->get()!= '\0')))
```
* 【修复】：添加EOF判断




### c++11 往多线程函数传递引用参数时会报错:rror C2672: “std::invoke”: 未找到匹配的重载函数

https://blog.csdn.net/pengh56/article/details/78244509
引用参数的入参为 str::ref(refData)



### gsoap

1. 发送的数据包含中文时, webservice无法接收的问题

```
	// SoapServiceSoapBindingProxy proxy(strWebServiceUrl.c_str(), SOAP_C_UTFSTRING);
	改为:
	SoapServiceSoapBindingProxy proxy(strWebServiceUrl.c_str(), SOAP_C_MBSTRING);
```

2. webservice采用上诉方式后, 接收到的数据仍存在中文乱码的情况


### TimeOutScan时, 对错误数据要异步发webservice

> 原本是for循环扫描map的时候, 发现不合格的数据会发送webservice, 发完后再进入下一层循环;
> 这样肯定是不行的,如果A发送失败则会阻塞在那里等发完毕才处理下一条

* 改造成异步的形式

solution1: 这样也不行, 当进入下一个循环的时候会崩掉,除非给每个进到这里的循环都新开两条线程来异步处理发送过程
		   最好是把要发送的放到一个队列里, 然后访问这个队列进行异步发送, 可是我怎么知道该消息对应的返回结果

solution2: 框架、语言特性：thrift、c++11的future、promise进行异步
```c++
	/*
	std::promise<string> prom;                           // 生成一个 std::promise 对象.
	std::future<string> fut = prom.get_future();         // 和 future 关联.
	// 多线程使用类成员函数
	std::thread ThreadSend(&CUtils::WebServiceAgent, &clUtils, std::ref(prom), strPostData, std::ref(strResponse));            // 将 prom交给另外一个线程t1 注：std::ref,promise对象禁止拷贝构造，以引用方式传递
	std::thread ThreadGetRet(&CUtils::GetWebServiceRet, &clUtils, std::ref(fut));        // 将 future 交给另外一个线程t.

	if (ThreadSend.joinable() && ThreadGetRet.joinable())
	{
		ThreadSend.join();
		ThreadGetRet.join();
	}
	*/
```
solution3: 线程池 ThreadPool.h https://blog.csdn.net/caoshangpa/article/details/80374651

```c++
	// × 之前要等发送完毕才进入下一个循环是因为线程池不是在这里创建的 
	ThreadPool pool(4);
	auto result = pool.enqueue(CUtils::WebServiceAgent, strPostData, strResponse);

	// √ 线程池在外面创建
	WebServiceRet.emplace_back(pool.enqueue(&CUtils::WebServiceAgent, clUtils, strPostData, strResponse));

```
