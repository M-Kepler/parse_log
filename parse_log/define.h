#pragma once
#ifndef __DEFINE_H_
#define __DEFINE_H_

//------------------------------------------------------------------------------
// 对vs认为不安全的函数不报错
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE


//------------------------------------------------------------------------------
// DEBUG/RELEASE模式, 三方库转换

#ifdef _DEBUG
#pragma comment(lib, "glogd")
/*改用webservice了*/
//#pragma comment(lib, "libcurl_a_debug")
#else
#pragma comment(lib, "glog")
/*改用webservice了*/
// #pragma comment(lib, "libcurl_a")
#endif

//------------------------------------------------------------------------------
// 第三方库: Libcurl依赖
// 改为使用webservice了, libcurl库不需要,
// 即关于libcurl的都可以删除:mylibcurl.h、utils.h中的dopost(),
// Sleep()可跳转声明但还是提示没有定义是因为缺少了#include <windows.h>
// 在代码中声明跟在visual studio项目属性-> 链接器->附加依赖项中输入一样的效果
/*改用webservice了*/
/*
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Wldap32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "Crypt32")
*/

//------------------------------------------------------------------------------
// 第三方库: Glog

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES


//------------------------------------------------------------------------------
// 系统函数转化

#ifdef OS_IS_LINUX
#define strtok_s strtok_r
#define PATHSPLIT '/'
#define LPCSTR const char*
#define LONG long
#define LONGLONG long long
#define HANDLE void*
#define INVALID_HANDLE_VALUE -1
#define DWORD unsigned long
#define Sleep usleep
#define SLEEP_MULTIPLE 1000 // 为了兼容windows下的Sleep函数, 这里对Unix下的usleep做放大
#else
#define PATHSPLIT '\\'
#define SLEEP_MULTIPLE 1
# endif

#endif
