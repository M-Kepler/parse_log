#include "LbmRiskWarning.h"


/*
CLbmRiskWarning::CLbmRiskWarning(int m_iThreadPoolCount) :m_clThreadPool(m_iThreadPoolCount)
{
	// TODO ��ȡһЩ��ʼ����Ҫ�Ĳ���
	cout << "ddddddddd" << endl;
}
*/


CLbmRiskWarning::~CLbmRiskWarning()
{
}


UtilsError CLbmRiskWarning::multi_thread(ifstream &file)
{
	int iLineLen;
	int iThreadCount;
	int iSendThreadCount;
	int iLineBuffer;
	int iAnsNum;
	int iScanTime;
	string strLastLine;
	// XXX �з��ضϻ����С
	// ҪС��, �����lbm��ans���ܳ���; ���������Ϊans�����ܵ��ֵ
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	string strSendThreadCount;
	string strAnsNum;
	string strScanTime;
	string strIncreRead;
	streamsize llLoadSize;
	streamsize llMaxSize;

	// ��ȡ����
	if ((m_utilsError = m_clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strSendThreadCount, "SendThreadCount")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strAnsNum, "AnsNum")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strScanTime, "ScanTime")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strIncreRead, "IncreRead")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << m_utilsError << endl;
		// TODO �쳣�׳�
		return m_utilsError;
	}
	iThreadCount = stoi(strThreadCount);
	llLoadSize = stoll(strLoadSize);
	iLineBuffer = stoi(strLineBuffer);
	iAnsNum = stoi(strAnsNum);
	iScanTime = stoi(strScanTime);
	iSendThreadCount = stoi(strSendThreadCount);

	ios::sync_with_stdio(false);
	m_pLogMaps = new LogMap[iThreadCount];

	// ˫����
	llMaxSize = llLoadSize + iLineBuffer;

	m_pLoadedFile[0] = new char[llMaxSize];
	m_pLoadedFile[1] = new char[llMaxSize];


	// ��ȡ�ļ�
	// ifstream file;
		/* ȷ���ļ���С bytes */
	streamoff llStart = 0;
	streamoff llFileSize; // �ļ���С
	streamoff llCurrFileSize; // �ļ���С
	file.seekg(0, ios::end); // �ļ�ָ��ָ���ļ�ĩβ
	streamoff llFileLen = file.tellg();
	streampos pCurrPos = file.tellg(); // ��һ���ļ�ָ��
	streamoff llThreadIndex, llThreadPart;
	streampos llLastLinePos; // ���һ�����׵�λ��
	streampos llEndFilePos; // �ļ�ĩβλ��
	streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)

	bool bBufferIndex = 0; // �����±�
	bool bNeedWait = false;

	// thread *threads = new thread[iThreadCount];

	// ��ȡ��С
	/* ���ݴ� bom �� utf8 �����ʽ */
	if (llFileLen > 3)
	{
		char bom[3];
		file.seekg(0);
		file.read(bom, 3);
		if (bom[0] == -17 && bom[1] == -69 && bom[2] == -65)
		{
			llStart = 3;
			llFileSize = llFileLen - 3;
		}
		else
		{
			llFileSize = llFileLen;
		}
	}
	else
	{
		llFileSize = llFileLen;
	}

	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "���ļ���С: " << llFileSize << endl;

	// ԭ�е���־����Ҫ����
	if (strIncreRead == "Yes")
	{
		char* strReq = (char*)"Req:";

		vector<string> vecTailLine;
		m_utilsError = m_clUtils.TailLine(file, 1, vecTailLine);
		if (m_utilsError != UTILS_RTMSG_OK)
		{
			abort();
		}
		strLastLine = vecTailLine[0];
		iLineLen = strLastLine.length();
		if (strstr(strLastLine.c_str(), strReq) != NULL)
		{
			file.clear();
			file.seekg(-(iLineLen), ios::end);
			llLastLinePos = file.tellg();
		}
		else
		{
			file.clear();
			file.seekg(0, ios::end);
			llLastLinePos = file.tellg();

			chrono::milliseconds TimeOut(iScanTime);
			this_thread::sleep_for(TimeOut);
		}
	}

	file.clear();
	file.seekg(0, ios::end);
	llEndFilePos = file.tellg();
	llFileSize = llEndFilePos - llLastLinePos;
	llStart = llLastLinePos; // ��ʼ����λ�û��˵���ĩβһ�е�����

	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "���һ��Ϊ: " << strLastLine << endl;
	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;

	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "������ļ���С: " << llFileSize << "\t������ʼλ��: " << llStart << "\t��������λ��: " << llEndFilePos << endl;

	ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount, iAnsNum, iScanTime);

	// ����
	delete m_pLoadedFile[0];
	delete m_pLoadedFile[1];
	file.close();
	return UTILS_RTMSG_OK;
}


vector<string> CLbmRiskWarning::ReadLineToVec(int iStep, streamoff llStart, streamsize llSize)
{
	char *pFileBuffer = NULL;
	char *pLineBuffer = NULL;
	string sLineBuffer;
	string sFileBuffer;
	string sLine;
	vector<string> vecStringLine;

	pFileBuffer = m_pLoadedFile[iStep];

	sFileBuffer = pFileBuffer;
	sLineBuffer = sFileBuffer.substr(llStart, llSize);
	pLineBuffer = (char*)sLineBuffer.c_str();
	vecStringLine.clear();

	char *strDelim = (char*)"\n";
	char *strToken = NULL;
	char *nextToken = NULL;

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		// test begin
		// creator.out �����ٶ�<=2msʱ����
		if (sLine.length() < 72)
		{
			abort();
		}
		// test end
		vecStringLine.push_back(sLine);
		strToken = strtok_s(NULL, strDelim, &nextToken);
	}
	return vecStringLine;
}


void CLbmRiskWarning::ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = m_pLogMaps + id;

	auto end = map->end();
	if (!vecStr.empty())
	{
		for (string::size_type i = 0; i < vecStr.size(); ++i)
		{
			MsgId = m_clUtils.GetMsgValue(vecStr[i], strKey);
			if (MsgId.empty())
			{
				// XXX
				// û��MsgId�Ĵ�Ϊ��req/ans ������Ҫ���뵽map��
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
			}
		}
	}
}


void CLbmRiskWarning::TimeoutScan(unordered_multimap<string, string> &mymap, int iAnsNum)
{
	int iLbmTimeOut;
	int iSoapRetCode;
	string strLbmTimeOut;
	string strModule;
	string strPostData;
	string strRetCode;
	string strResponse;

	auto begin = mymap.begin();
	auto end = mymap.end();

	// ����
	// ��ȡ����
	if ((m_utilsError = m_clUtils.GetConfigValue(strModule, "Module")) != UTILS_RTMSG_OK)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << m_utilsError << endl;
		// TODO �쳣�׳�
		abort();
	}

	LOG(INFO) << endl;
	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "allLogMap.size(): " << m_allLogMap.size() << endl;
	for (; begin != end; )
	{
		if (mymap.count(begin->first) < iAnsNum)
		{
			time_t CurrTimeMs = m_clUtils.GetCurrentTimeMs();
			time_t MsgTimeMs = m_clUtils.StringToMs(begin->second);

			// ��ȡ����
			if ((m_utilsError = m_clUtils.GetConfigValue(strLbmTimeOut, "LbmTimeOut")) != UTILS_RTMSG_OK)
			{
				LOG(ERROR) << "��ȡ����ʧ��, ������: " << m_utilsError << endl;
				abort();
			}
			iLbmTimeOut = stoll(strLbmTimeOut);

			if (((CurrTimeMs - MsgTimeMs) > iLbmTimeOut) && (strstr((begin->second).c_str(), "Req") != NULL))
			{
				// req����ans���������ڴ��; ��req���ѳ�ʱ,��ɾ��
				LOG(INFO) << "ɨ��ʱ��(ms): " << CurrTimeMs << "\tReq��ʱ��(ms): " << MsgTimeMs << "\tʱ���: " << CurrTimeMs - MsgTimeMs;
				LOG(INFO) << endl;

				strPostData = m_clUtils.AssembleJson(begin->second);

				// begin++;
				begin = mymap.erase(begin);
				begin = mymap.begin();
			}
			else if (((CurrTimeMs - MsgTimeMs) > iLbmTimeOut) && (strstr((begin->second).c_str(), "Ans") != NULL))
			{
				// req����ans���������ڴ��; ��ans ����Ӧ��req������ʱ��ɾ����,��ansҲɾ��
				begin = mymap.erase(begin);
				begin = mymap.begin();
			}
			else
			{
				// req����ans���������ڴ��; req��δ��ʱ,����,(������һ���ڴ���ʱ��, ���ҵ�����Ӧ��ans����,���ʱ���ӳ��ܽ���)
				begin++;
			}
		}
		else
		{
			if ((strstr((begin->second).c_str(), "Ans") != NULL))
			{
				strRetCode = m_clUtils.GetMsgValue(begin->second, "&_1");

				// û��ʱ, ��ans����
				if (((strModule == "KBSS") && (strRetCode != "0"))
					|| ((strModule == "CTS") && (strRetCode != "0"))
					)
				{
					// �ҵ���ans��Ӧ��req��
					LogMapKeySet iterReqData = mymap.find(begin->first);
					for (size_t i = 0; i < mymap.count(begin->first); i++, iterReqData++)
					{
						if (strstr(iterReqData->second.c_str(), "Req") != NULL)
						{
							break;
						}
					}
					strPostData = m_clUtils.AssembleJson(iterReqData->second, begin->second);
				}
			}

			// ɾ��ͬʱ����req��ans��
			string key = begin->first;
			mymap.erase(begin->first);
			begin = mymap.begin();
			// end = mymap.end();
		}

		if (!strPostData.empty())
		{
			// ���Ͳ�ɾ��
			// Post
			/*
			char* pMsgData = (char*)strPostData.c_str();
			utilsError = clUtils.DoPost(pMsgData, strResponse);
			*/

			// WebService
			// WebServiceRet.emplace_back(pool.enqueue(&CUtils::WebServiceAgent, m_clUtils, strPostData, strResponse));
			m_vecWebServiceRet.emplace_back(m_clThreadPool.enqueue(&CUtils::WebServiceAgent, m_clUtils, strPostData, strResponse));

			strPostData = "";
		}

	}
	// cout << "ɾ����allLogMap.size():   " << allLogMap.size() << endl;// debug
}


void CLbmRiskWarning::ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime)
{

	clock_t t_start, t_end; // debug

	// test begin
	pCurrPos = llStart + llFileSize;
	// test end

	bool bBufferIndex = 0; // �����±�
	bool bNeedWait = false;
	string strMsgKey;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
	streamsize llFileLen;
	streampos pNewPos;
	thread *threads = new thread[iThreadCount];

	if ((m_utilsError = m_clUtils.GetConfigValue(strMsgKey, "MsgKey")) != UTILS_RTMSG_OK)
	{
		LOG(ERROR) << "��ȡ����MsgKeyʧ��" << endl;
		abort();
	}

	while (llFileSize)
	{
		t_start = clock(); // debug

		// test begin
		// bak
		// �� llFileSize > llMaxSize�����Ѿ������˽�λ�Ļ��棬��������
		llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoll(strLoadSize)) : llFileSize;
		// llRealSize = llFileSize;
		// test end

		llThreadIndex = 0;

		// llThreadIndex = llFileSize - iLineLen - 1; // ��ȥ���һ������һ��֮���\n�ַ�
		llThreadPart = llRealSize / iThreadCount;

		LOG(INFO) << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		LOG(INFO) << "�������ݵ� loadedFile[bBufferIndex]: " << bBufferIndex << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "�ƻ�һ�ζ����С llLoadSize: " << stoll(strLoadSize) << "\tʵ��һ�ζ����С llRealSize: " << llRealSize << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "���뿪ʼλ�� llStart: " << llStart << "\t���ļ�ʣ���ȡ��С llFileSize: " << llFileSize << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "�ƻ�ÿ���̶߳����С llThreadPart: " << llThreadPart;
		LOG(INFO) << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;


		// ���� llRealSize ��С���ļ����ݵ����� loadedFile[bBufferIndex] ��
		readLoad(bBufferIndex, &file, llStart, llRealSize);
		llStart += llRealSize;
		llFileSize -= llRealSize;

		// �������߳�,�ȴ���һ�����ݿ��������,�ٶ���һ���ݿ���з���
		if (bNeedWait)
		{
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}
			// �Ѹ����ݿ����ȫ�嵽һ��map��
			for (int i = 0; i < iThreadCount; ++i)
			{
				LogMapKeySet p = (m_pLogMaps + i)->begin();
				LogMapKeySet end = (m_pLogMaps + i)->end();
				for (; p != end; ++p)
				{
					m_allLogMap.insert(pair<string, string>(p->first, p->second));
				}
				// ������̵߳���map
				LogMap *ThreadMap = m_pLogMaps + i;
				ThreadMap->clear();
			}

			// ɨ��map��ʱ��̵ܶ�, ���Ƿ��͵�ʱ��ܳ�, ���������첽��
			TimeoutScan(m_allLogMap, iAnsNum);

			// FIXME �������첽�����ķ���ֵ
			// thread WatchRetJob(&CUtils::GetWebServiceRet, m_clUtils, std::ref(m_vecWebServiceRet));
			// WatchRetJob.detach();

		}
		else
		{
			bNeedWait = true;
		}

		t_end = clock(); // debug
		cout << "\nScan Complete in : " << t_end - t_start << "ms." << endl << endl; // debug

		LOG(INFO) << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		for (int i = 1; i < iThreadCount; ++i)
		{
			if (llThreadIndex != llRealSize) // �����ļ�ֻ��һ��ʱ getBlockSize ����
			{
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// ��ʣ�����������߳���,����Ҫ�ж�һ���߳��Ѵ�����ַ��͸ÿ����ݵĴ�С
			if (llFileLen + llThreadIndex < llRealSize)
			{
				LOG(INFO) << "�߳� " << i << " ��ʼ����λ��llThreadIndex: " << llThreadIndex << "\tʵ���߳� " << i << " �����СllFileLen:" << llFileLen << endl;
				vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
				LOG(INFO) << "�߳� " << i << " ������: " << vecThreadLines.size() << "��" << endl;

				threads[i] = thread(&CLbmRiskWarning::ParseMsgLine, this, vecThreadLines, i, strMsgKey);

				llThreadIndex += llFileLen;
			}
			else
			{
				break;
			}
		}

		LOG(INFO) << "�߳� 4 ��ʼ����λ�� llThreadIndex: " << llThreadIndex << "\tʵ���߳� 4 �����С llRealSize-llThreadIndex: " << llRealSize - llThreadIndex << endl;

		// XXX
		// һ�����󲻿�������70���ַ���
		if ((llRealSize - llThreadIndex) > 70)
		{
			vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
			threads[0] = thread(&CLbmRiskWarning::ParseMsgLine, this, vecEndThreadLines, 0, strMsgKey);
			LOG(INFO) << "�߳� 4 " << "������: " << vecEndThreadLines.size() << "��" << endl;
		}

		bBufferIndex = !bBufferIndex; // �л� Buffer װ����
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		LOG(INFO) << "�ļ�ʣ���С: llFileSize: " << llFileSize << endl;

		// �ļ���������
		if (llFileSize != 0)
		{
			continue;
		}
		else
		{
			LOG(INFO) << endl;
			LOG(INFO) << "============== ----------------------- ============== ----------------------- ==============";
			LOG(INFO) << "============== ----------------------- ������һ���ݿ� ----------------------- ==============";
			LOG(INFO) << "============== ----------------------- ============== ----------------------- ==============";
			LOG(INFO) << endl;
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}

			// ��������ļ�����
			// XXX ʱ�����ŵ�������? �����������־��û�д�����?
			do
			{
				std::chrono::milliseconds TimeOut(iScanTime);
				// Sleep(iScanTime*SLEEP_MULTIPLE);
				std::this_thread::sleep_for(TimeOut);
				file.clear();
				file.seekg(0, ios::end); // �ļ�ָ��ָ���ļ�ĩβ
				pNewPos = file.tellg(); // �µ��ļ�ָ��
				llFileSize = pNewPos - pCurrPos;
				if (m_clUtils.bIsNextDay())
				{
					// ʱ���Ѿ���Խ����һ��,���н���; �ر��ļ�������һ����ļ�

					// WatchRetJob.join();
					break;
				}
			} while (llFileSize <= 0);

			LOG(INFO) << endl;
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			LOG(INFO) << "��ǰָ��λ��: " << pNewPos << "\t�ϴ�ָ��λ��: " << pCurrPos << "\t�����ļ���С: " << llFileSize << endl;

			if (llFileSize < 0)
			{
				llFileSize = 0;
			}
			pCurrPos = pNewPos;
		}
	}

}

