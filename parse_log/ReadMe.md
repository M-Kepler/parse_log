

# 任务列表


## FIXME

[ ] 我的异常应该怎么定义和抛出?
[ ] 文件持续写入、multi_thread需要重构封装到类
[ ] 超时扫描的优化

## TODO

[*] **定时**扫描功能
[*] 文件在实时增长
	[√] 打算这样: 一次读取到文件尾,然后用现在的逻辑去分析到一个allmap中, 然后把文件尾指针当文件头指针, 再获取一次文件尾指针,如果两个指针不一致
		说明文件有了增长, 再次把增长的文件放到现在的逻辑中去分析(或者设置定是时间, 定时时间一到就把这段文件拿去分析<这样确实没有刘欢的那个实时>)
	[*] 首次读取读文件尾, 若是ans则丢弃
	[*] ans串还有cost字段
[*] MFC 前端界面
[*] 而且每天一个文件夹, 也没有处理
[ ] 一个请求分割多行的情况
[ ] KCXP的日志? 三方网关的日志?
[ ] HTTP


## XXX

[ ] 组合 iThreadCount 后再扫, 改为插入的时候就进行判断



# 第三方库


## GLog

### 编译安装

* [glog下载和使用](https://blog.csdn.net/Pig_Pig_Bang/article/details/81632962)

* [glog unix下编译使用](https://blog.csdn.net/csm201314/article/details/75094527)

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

## libcurl

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
   2.4 单例模式可以很好解决这个问题;
	   但是, 单例模式怎么写啊...
