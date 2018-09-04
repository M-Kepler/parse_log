#pragma once
#ifndef __DEFINE_H_
#define __DEFINE_H_

//------------------------------------------------------------------------------
// ��vs��Ϊ����ȫ�ĺ���������
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE


//------------------------------------------------------------------------------
// DEBUG/RELEASEģʽ, ������ת��

#ifdef _DEBUG
#pragma comment(lib, "glogd")
#pragma comment(lib, "libcurl_a_debug")
#else
#pragma comment(lib, "glog")
#pragma comment(lib, "libcurl_a")
#endif

//------------------------------------------------------------------------------
// ��������: Libcurl
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Wldap32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "Crypt32")

//------------------------------------------------------------------------------
// ��������: Glog

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES


//------------------------------------------------------------------------------
// ϵͳ����ת��

#ifdef OS_IS_LINUX
#define strtok_s strtok_r

#define LPCSTR char*
#define LONG long
#define LONGLONG long long
#define HANDLE void*
#define INVALID_HANDLE_VALUE -1
#define DWORD unsigned long

# endif

#endif
