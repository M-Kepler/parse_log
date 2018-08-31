#pragma once
#include "define.h"
#include <string>
#ifdef OS_IS_LINUX
#include <cstring>
#endif
#include <curl/curl.h>
using std::string;


class CLibcurlCallback
{
public:
	virtual void Progress(void* lpParam, double dTotal, double bLoaded) = 0;
};

enum LibcurlFlag
{
	Lf_None = 0,
	Lf_Download,
	Lf_Post,
	Lf_Get,
};

class CLibcurl
{
public:
	CLibcurl(void);
	~CLibcurl(void);
	/******************************************************************************
	*封装类的外部调用接口
	*/
	CURLcode SetHttpPort(LONG port);										//设置连接端口号
	CURLcode SetTimeout(int nSecond);										//设置执行超时（秒）
	CURLcode SetConnectTimeout(int nSecond);								//设置连接超时（秒）
	CURLcode SetUserAgent(LPCSTR lpAgent);									//设置用户代理
	CURLcode SetResumeFrom(LONG lPos);										//设置断点续传起始位置
	CURLcode SetResumeFromLarge(LONGLONG llPos);							//设置断点续传起始位置，针对大文件
	CURLcode AddHeader(LPCSTR lpKey, LPCSTR lpValue);						//添加自定义头
	void ClearHeaderList();													//清理HTTP列表头
	CURLcode SetCookie(LPCSTR lpCookie);									//设置HTTP请求cookie
	CURLcode SetCookieFile(LPCSTR lpFilePath);								//设置HTTP请求cookie文件
	const char* GetError()const;											//获取错误详细信息
	void SetCallback(CLibcurlCallback* pCallback, void* lpParam);			//设置下载进度回调
	CURLcode DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);					//下载文件到磁盘
	CURLcode Post(LPCSTR lpUrl, LPCSTR lpData);								//Post 字符串数据
	CURLcode Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize); //Post 字符串或者二进制数据
	CURLcode Get(LPCSTR lpUrl);												//Get 请求
	const string& GetRespons()const;										//获取Post/Get请求返回数据
	const char*	GetResponsPtr()const;										//获取Post/Get请求返回数据

protected:
	static size_t	WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
	static size_t	HeaderCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
	static int		ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow);

private:
	CURL	*m_pCurl;
	LONG	m_nPort;
	HANDLE	m_hFile;
	CURLcode m_curlCode;
	string	m_strRespons;
	LibcurlFlag m_lfFlag;
	curl_slist *m_curlList;
	void	*m_pCallbackParam;
	CLibcurlCallback	*m_pCallback;
};