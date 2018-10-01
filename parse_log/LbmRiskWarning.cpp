#include "LbmRiskWarning.h"


/*
CLbmRiskWarning::CLbmRiskWarning(int m_iThreadPoolCount) :m_clThreadPool(m_iThreadPoolCount)
{
	// TODO 获取一些初始化需要的参数
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
	// XXX 行防截断缓冲大小
	// 要小心, 特殊的lbm的ans串很长的; 这里最好设为ans串可能的最长值
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	string strSendThreadCount;
	string strAnsNum;
	string strScanTime;
	string strIncreRead;
	streamsize llLoadSize;
	streamsize llMaxSize;

	// 获取配置
	if ((m_utilsError = m_clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strSendThreadCount, "SendThreadCount")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strAnsNum, "AnsNum")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strScanTime, "ScanTime")) != UTILS_RTMSG_OK
		|| (m_utilsError = m_clUtils.GetConfigValue(strIncreRead, "IncreRead")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << m_utilsError << endl;
		// TODO 异常抛出
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

	// 双缓冲
	llMaxSize = llLoadSize + iLineBuffer;

	m_pLoadedFile[0] = new char[llMaxSize];
	m_pLoadedFile[1] = new char[llMaxSize];


	// 读取文件
	// ifstream file;
		/* 确认文件大小 bytes */
	streamoff llStart = 0;
	streamoff llFileSize; // 文件大小
	streamoff llCurrFileSize; // 文件大小
	file.seekg(0, ios::end); // 文件指针指到文件末尾
	streamoff llFileLen = file.tellg();
	streampos pCurrPos = file.tellg(); // 上一次文件指针
	streamoff llThreadIndex, llThreadPart;
	streampos llLastLinePos; // 最后一行行首的位置
	streampos llEndFilePos; // 文件末尾位置
	streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)

	bool bBufferIndex = 0; // 缓存下标
	bool bNeedWait = false;

	// thread *threads = new thread[iThreadCount];

	// 获取大小
	/* 兼容带 bom 的 utf8 编码格式 */
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
	LOG(INFO) << "总文件大小: " << llFileSize << endl;

	// 原有的日志不需要分析
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
	llStart = llLastLinePos; // 开始读入位置回退到最末尾一行的行首

	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "最后一行为: " << strLastLine << endl;
	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;

	LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
	LOG(INFO) << "需分析文件大小: " << llFileSize << "\t分析开始位置: " << llStart << "\t分析结束位置: " << llEndFilePos << endl;

	ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount, iAnsNum, iScanTime);

	// 清理
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
		// creator.out 产生速度<=2ms时必现
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
				// 没有MsgId的串为非req/ans 串不需要插入到map中
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

	// 抽离
	// 获取配置
	if ((m_utilsError = m_clUtils.GetConfigValue(strModule, "Module")) != UTILS_RTMSG_OK)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << m_utilsError << endl;
		// TODO 异常抛出
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

			// 获取配置
			if ((m_utilsError = m_clUtils.GetConfigValue(strLbmTimeOut, "LbmTimeOut")) != UTILS_RTMSG_OK)
			{
				LOG(ERROR) << "获取配置失败, 错误码: " << m_utilsError << endl;
				abort();
			}
			iLbmTimeOut = stoll(strLbmTimeOut);

			if (((CurrTimeMs - MsgTimeMs) > iLbmTimeOut) && (strstr((begin->second).c_str(), "Req") != NULL))
			{
				// req串和ans串在两个内存块; 且req串已超时,则删除
				LOG(INFO) << "扫描时间(ms): " << CurrTimeMs << "\tReq串时间(ms): " << MsgTimeMs << "\t时间差: " << CurrTimeMs - MsgTimeMs;
				LOG(INFO) << endl;

				strPostData = m_clUtils.AssembleJson(begin->second);

				// begin++;
				begin = mymap.erase(begin);
				begin = mymap.begin();
			}
			else if (((CurrTimeMs - MsgTimeMs) > iLbmTimeOut) && (strstr((begin->second).c_str(), "Ans") != NULL))
			{
				// req串和ans串在两个内存块; 且ans 串对应的req串已因超时被删除了,则ans也删掉
				begin = mymap.erase(begin);
				begin = mymap.begin();
			}
			else
			{
				// req串和ans串在两个内存块; req串未超时,跳过,(处理下一个内存块的时候, 会找到它对应的ans串的,这点时间延迟能接收)
				begin++;
			}
		}
		else
		{
			if ((strstr((begin->second).c_str(), "Ans") != NULL))
			{
				strRetCode = m_clUtils.GetMsgValue(begin->second, "&_1");

				// 没超时, 但ans报错
				if (((strModule == "KBSS") && (strRetCode != "0"))
					|| ((strModule == "CTS") && (strRetCode != "0"))
					)
				{
					// 找到该ans对应的req串
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

			// 删除同时存在req、ans的
			string key = begin->first;
			mymap.erase(begin->first);
			begin = mymap.begin();
			// end = mymap.end();
		}

		if (!strPostData.empty())
		{
			// 发送并删除
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
	// cout << "删除后allLogMap.size():   " << allLogMap.size() << endl;// debug
}


void CLbmRiskWarning::ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime)
{

	clock_t t_start, t_end; // debug

	// test begin
	pCurrPos = llStart + llFileSize;
	// test end

	bool bBufferIndex = 0; // 缓存下标
	bool bNeedWait = false;
	string strMsgKey;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)
	streamsize llFileLen;
	streampos pNewPos;
	thread *threads = new thread[iThreadCount];

	if ((m_utilsError = m_clUtils.GetConfigValue(strMsgKey, "MsgKey")) != UTILS_RTMSG_OK)
	{
		LOG(ERROR) << "获取配置MsgKey失败" << endl;
		abort();
	}

	while (llFileSize)
	{
		t_start = clock(); // debug

		// test begin
		// bak
		// 当 llFileSize > llMaxSize，且已经超出了截位的缓存，会有问题
		llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoll(strLoadSize)) : llFileSize;
		// llRealSize = llFileSize;
		// test end

		llThreadIndex = 0;

		// llThreadIndex = llFileSize - iLineLen - 1; // 减去最后一行与上一行之间的\n字符
		llThreadPart = llRealSize / iThreadCount;

		LOG(INFO) << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		LOG(INFO) << "读入数据到 loadedFile[bBufferIndex]: " << bBufferIndex << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "计划一次读入大小 llLoadSize: " << stoll(strLoadSize) << "\t实际一次读入大小 llRealSize: " << llRealSize << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "读入开始位置 llStart: " << llStart << "\t总文件剩余读取大小 llFileSize: " << llFileSize << endl;
		LOG(INFO) << endl;
		LOG(INFO) << "计划每个线程读入大小 llThreadPart: " << llThreadPart;
		LOG(INFO) << endl;
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;


		// 读入 llRealSize 大小的文件数据到缓存 loadedFile[bBufferIndex] 中
		readLoad(bBufferIndex, &file, llStart, llRealSize);
		llStart += llRealSize;
		llFileSize -= llRealSize;

		// 阻塞主线程,等待上一个数据块分析结束,再对下一数据块进行分析
		if (bNeedWait)
		{
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}
			// 把该数据块的行全插到一个map中
			for (int i = 0; i < iThreadCount; ++i)
			{
				LogMapKeySet p = (m_pLogMaps + i)->begin();
				LogMapKeySet end = (m_pLogMaps + i)->end();
				for (; p != end; ++p)
				{
					m_allLogMap.insert(pair<string, string>(p->first, p->second));
				}
				// 清理掉线程的子map
				LogMap *ThreadMap = m_pLogMaps + i;
				ThreadMap->clear();
			}

			// 扫描map的时间很短的, 但是发送的时间很长, 必须做成异步的
			TimeoutScan(m_allLogMap, iAnsNum);

			// FIXME 不关心异步操作的返回值
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
			if (llThreadIndex != llRealSize) // 避免文件只有一行时 getBlockSize 报错
			{
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// 若剩余行数少于线程数,还需要判断一下线程已处理的字符和该块数据的大小
			if (llFileLen + llThreadIndex < llRealSize)
			{
				LOG(INFO) << "线程 " << i << " 开始读入位置llThreadIndex: " << llThreadIndex << "\t实际线程 " << i << " 读入大小llFileLen:" << llFileLen << endl;
				vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
				LOG(INFO) << "线程 " << i << " 共读入: " << vecThreadLines.size() << "行" << endl;

				threads[i] = thread(&CLbmRiskWarning::ParseMsgLine, this, vecThreadLines, i, strMsgKey);

				llThreadIndex += llFileLen;
			}
			else
			{
				break;
			}
		}

		LOG(INFO) << "线程 4 开始读入位置 llThreadIndex: " << llThreadIndex << "\t实际线程 4 读入大小 llRealSize-llThreadIndex: " << llRealSize - llThreadIndex << endl;

		// XXX
		// 一行请求不可能少于70个字符吧
		if ((llRealSize - llThreadIndex) > 70)
		{
			vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
			threads[0] = thread(&CLbmRiskWarning::ParseMsgLine, this, vecEndThreadLines, 0, strMsgKey);
			LOG(INFO) << "线程 4 " << "共读入: " << vecEndThreadLines.size() << "行" << endl;
		}

		bBufferIndex = !bBufferIndex; // 切换 Buffer 装数据
		LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
		LOG(INFO) << "文件剩余大小: llFileSize: " << llFileSize << endl;

		// 文件滚动增长
		if (llFileSize != 0)
		{
			continue;
		}
		else
		{
			LOG(INFO) << endl;
			LOG(INFO) << "============== ----------------------- ============== ----------------------- ==============";
			LOG(INFO) << "============== ----------------------- 读入下一数据块 ----------------------- ==============";
			LOG(INFO) << "============== ----------------------- ============== ----------------------- ==============";
			LOG(INFO) << endl;
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}

			// 持续监控文件增长
			// XXX 时间流逝到明天呢? 而且明天的日志还没有创建呢?
			do
			{
				std::chrono::milliseconds TimeOut(iScanTime);
				// Sleep(iScanTime*SLEEP_MULTIPLE);
				std::this_thread::sleep_for(TimeOut);
				file.clear();
				file.seekg(0, ios::end); // 文件指针指到文件末尾
				pNewPos = file.tellg(); // 新的文件指针
				llFileSize = pNewPos - pCurrPos;
				if (m_clUtils.bIsNextDay())
				{
					// 时间已经跨越到新一天,运行结束; 关闭文件并找新一天的文件

					// WatchRetJob.join();
					break;
				}
			} while (llFileSize <= 0);

			LOG(INFO) << endl;
			LOG(INFO) << "==============-------------------------==============-------------------------==============" << endl;
			LOG(INFO) << "当前指针位置: " << pNewPos << "\t上次指针位置: " << pCurrPos << "\t增长文件大小: " << llFileSize << endl;

			if (llFileSize < 0)
			{
				llFileSize = 0;
			}
			pCurrPos = pNewPos;
		}
	}

}

