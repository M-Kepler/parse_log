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
	*��װ����ⲿ���ýӿ�
	*/
	CURLcode SetHttpPort(LONG port);										//�������Ӷ˿ں�
	CURLcode SetTimeout(int nSecond);										//����ִ�г�ʱ���룩
	CURLcode SetConnectTimeout(int nSecond);								//�������ӳ�ʱ���룩
	CURLcode SetUserAgent(LPCSTR lpAgent);									//�����û�����
	CURLcode SetResumeFrom(LONG lPos);										//���öϵ�������ʼλ��
	CURLcode SetResumeFromLarge(LONGLONG llPos);							//���öϵ�������ʼλ�ã���Դ��ļ�
	CURLcode AddHeader(LPCSTR lpKey, LPCSTR lpValue);						//����Զ���ͷ
	void ClearHeaderList();													//����HTTP�б�ͷ
	CURLcode SetCookie(LPCSTR lpCookie);									//����HTTP����cookie
	CURLcode SetCookieFile(LPCSTR lpFilePath);								//����HTTP����cookie�ļ�
	const char* GetError()const;											//��ȡ������ϸ��Ϣ
	void SetCallback(CLibcurlCallback* pCallback, void* lpParam);			//�������ؽ��Ȼص�
	CURLcode DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile);					//�����ļ�������
	CURLcode Post(LPCSTR lpUrl, LPCSTR lpData);								//Post �ַ�������
	CURLcode Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize); //Post �ַ������߶���������
	CURLcode Get(LPCSTR lpUrl);												//Get ����
	const string& GetRespons()const;										//��ȡPost/Get���󷵻�����
	const char*	GetResponsPtr()const;										//��ȡPost/Get���󷵻�����

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