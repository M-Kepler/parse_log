[TOC]

# ReadMe

## 默认参数说明

* 函数 StringToMs

```c++
iStart, iEnd两个默认参数分别表示时间在字符串中的开始和结束位置
time_t StringToMs(string strOrig, int iStart = 0, int iEnd = 19);
且时间格式为: formate = (char*)"%4d%2d%2d-%2d%2d%2d";
如:
u版runlog
20180202-090551-318351 18207    99 Req: LBM=L0106002,MsgI
20180202-090551-318 时间开始结束位置为0 - 19
```

* 函数 GetConfigValue

```
默认取的是[CONFIG]节点下的配置信息
```

* bCheckData用了windows的头文件



# 任务列表


## TODO

[*] Linux下要求能编译通过
[*] **定时**扫描功能
[*] 文件在实时增长
	[*] 打算这样: 一次读取到文件尾,然后用现在的逻辑去分析到一个allmap中, 然后把文件尾指针当文件头指针, 再获取一次文件尾指针,如果两个指针不一致说明文件有了增长, 再次把增长的文件放到现在的逻辑中去分析(或者设置定是时间, 定时时间一到就把这段文件拿去分析<这样确实没有刘欢的那个实时>)
	[*] 首次读取读文件尾, 若是ans则丢弃
	[*] ans串还有cost字段
[*] MFC 前端界面
[*] 而且每天一个文件夹, 也没有处理
[*] HTTP改为webservice了
[*] TimeOutScan函数要做成异步的,不能为了发送一条数据而堵死在那
[*] 这个程序是7*24要一直运行,不能因为没有日志文件增长就退出
    [ ] 要知道第二天日志就跑到另一个文件夹下去了
[*] 一个请求分割多行的情况(不处理)

[ ] TODO 三方网关的日志


## FIXME

## XXX




# 第三方库

在编译的时候, 好像与第三方库的编译模式有关;比如我选择的是多线程调试 DLL (/MDd)模式,
如果第三方库不是这个模式的话, 会有问题


## GLog(编译成静态库)

### 编译安装

* [glog下载和使用](https://blog.csdn.net/Pig_Pig_Bang/article/details/81632962)
* [glog windows编译](http://www.cnblogs.com/21207-iHome/p/9297329.html)
* [glog 功能修改](https://blog.csdn.net/tomisaboy/article/details/51418845)
* [glog unix下编译使用](https://blog.csdn.net/csm201314/article/details/75094527)

> 由于有的项目中用了dll,所以glog也要用dll方式编译,使用方法都一样

```
1. 安装工具
sudo apt-get install autoconf automake libtool

2. 编译安装
进入源码根目录（glog文件夹） 
./autogen.sh 
./configure --prefix=/path/to/install 
make -j 24 
sudo make install

3. 设置环境变量
修改$HOME/.bash_profile

# parse_log
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$HOME/parse/glog/unix/include
export LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/parse/glog/unix

```

* glog文件夹是编译后的文件,

当前是debug版本,需现在工程属性下配置一下
```
1. 右键项目属性
2. 配置属性-->C/C++-->常规-->附加包含目录-->添加 <glog安装路径>/include
3. Release模式配置: 配置属性-->C/C++-->代码生成-->运行库-->多线程DLL(/MD)
> 因为glog的库是这样编译出来的, 不配置这个会导致错误
> 3'. Debug模式配置: 配置属性-->C/C++-->代码生成-->运行库-->多线程调试 DLL (/MDd)
4. 配置属性-->链接器-->常规-->附加库目录-->添加<glog安装路径>/lib
5. 配置属性-->链接器-->输入-->附加依赖项-->添加glogd.lib（如果你的程序是Release版本就写glog.lib）
```

## libcurl(已剔除,改用webservice)

### 编译安装

* [Visual Studio(VS2017)编译并配置C/C++-libcurl开发环境](https://blog.csdn.net/DaSo_CSDN/article/details/77587916)
  * 编译命令后面加上 `ENABLE_IDN=no`

* [unix下编译安装libcurl](https://blog.csdn.net/qianghaohao/article/details/51684862)

./configure --prefix=/path/to/install 

3. 设置环境变量
修改$HOME/.bash_profile
CPLUS_INCLUDE_PATH=/path/to/libcurl/include
LIBRARY_PATH=/path/to/libcurl/lib

```

* libcurl 文件夹是编译后的文件,
```
1. 右键项目属性
2. 配置属性 --> C/C++ --> 常规-->附加包含目录 --> 添加 <curl路径>/include
3. Release模式配置: 配置属性 --> C/C++ --> 代码生成 --> 运行库 --> 多线程DLL(/MD)
4. 配置属性 --> 链接器 --> 常规 --> 附加库目录 --> 添加 <curl安装路径>/lib
5. 配置属性 --> 链接器 --> 输入 --> 附加依赖项 --> 添加 libcurl_a_debug.lib;Ws2_32.lib;Wldap32.lib;winmm.lib;Crypt32.lib
   如果你的程序是Release版本就写 libcurl_a.lib
6. 配置属性 --> C/C++ --> 预处理器 --> 预处理器定义 --> 编辑框中添加: CURL_STATICLIB 


## gsoap

* 名字空间重定义
https://bbs.csdn.net/topics/390023121
* 例子
https://blog.csdn.net/vv_demon/article/details/41282285

### 过程

* 下载gsoap

* 把文件复制到工程目录下(有的是生成的,有的是gsoap自带的)
soapSoapServiceSoapBindingProxy.h
soapSoapServiceSoapBindingProxy.cpp
soapH.h
soapC.cpp
SoapServiceSoapBinding.nsmap
soapStub.h
stdsoap2.h
stdsoap2.cpp



## Q&A

1. 本来是一个个功能函数来写的, 但是后来发现这样不行, 要采用oop的风格来构建

2. 构建类的时候, 涉及到了其他问题; 比如glog在使用之前需要初始化, 使用结束之后需要释放...
   期望是能封装到工具类中, 最好是一个全局变量, 这样就不需要每次使用之前都进行一次初始化
   2.1 第一个想法是使用全局变量; 把初始化和释放写到类CGlog中, 然后构建一个全局的对象, 这样在其他文件中extern一下就行了
	   但是, 会出现多次init了glog, 但是没有全部shutdown掉的问题
   2.2 第二个想法是使用类的静态成员;
       但是, 在需要log的地方可能不需要utils类, 这样每次都用utils类调用静态成员函数也没什么意思, 还不如拆出来写到单独文件
   2.3 把glog的初始化和释放写到单独cpp文件中, 在需要记录日志的地方包含log.h, 直接调用初始化, 在文件尾再调用shutdown
	   但是, 这样也是很冗余的, 比如我n个文件需要记录日志, 就需要初始化n次....最好是一个全局变量通吃
   2.4 使用单例模式

3. 定位到文件最后一行,判断是req还是ans的坑
   原本逻辑:
   用getline读入文件,循环结束后得到的就是最后一行了
   如果是req行, 则把指针移到最后一行的行尾, 然后再获取一下此刻的指：针
   ```
   		while (file.peek() != EOF)
		{
			getline(file, strLastLine);
		}
		iLineLen = strLastLine.length();

		if (strstr(strLastLine.c_str(), strReq) != NULL)
		{
			file.clear();
			// file.seekg(-(iLineLen + 1), ios::end); // XXX 这里+1是为了跳过换行符\n,但是当只有一行时, 行首是没有\n的
			file.seekg(-(iLineLen), ios::end);
			llLastLinePos = file.tellg();
			if (llLastLinePos < 0)
			{
				llLastLinePos = 0;
			}
			file.clear();
			file.seekg(0, ios::end);
			llEndFilePos = file.tellg();
			llFileSize = llEndFilePos - llLastLinePos;
			llStart = llLastLinePos; // 开始读入位置回退到最末尾一行的行首
		}

   ```