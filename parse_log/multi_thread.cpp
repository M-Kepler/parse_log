#include "multi_thread.h"
#include "utils.h"

char* loadedFile[2]; // 存放指向 char* 类型的指针的数组
typedef unordered_multimap<string, string> LogMap;
typedef unordered_multimap<string, string>::iterator LogMapKeySet;

LogMap *pLogMaps;
LogMap allLogMap;

CUtils clUtils;
UtilsError utilsError;

string strResponse;
vector<string> vecThreadLines, vecEndThreadLines;

int iCount = 0;

int multi_thread()
{
	int iLineLen;
	int iThreadCount;
	int iLineBuffer;
	string strLastLine;
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	streamsize llLoadSize;
	streamsize llMaxSize;

	// 获取配置
	if ((utilsError = clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
		// FIXME
		// 异常抛出到界面
		abort();
	}
	iThreadCount = stoi(strThreadCount);
	llLoadSize = stoll(strLoadSize);
	iLineBuffer = stoi(strLineBuffer);

	// const char* filename = "runlog0-3.1.log";
	const char* filename = "test.log";
	ios::sync_with_stdio(false);
	pLogMaps = new LogMap[iThreadCount];

	// 双缓冲
	llMaxSize = llLoadSize + iLineBuffer;

	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];

	clock_t t_start, t_end;
	t_start = clock();


	// 读取文件
	ifstream file;
	utilsError = clUtils.LoadFile(file, filename);
	if (UTILS_OPEN_SUCCESS != utilsError)
	{
		return utilsError;
	}
	else
	{
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
		thread *threads = new thread[iThreadCount];

		// TODO 分离
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
		cout << "\n===========-------------------------===========" << "总文件大小: " << llFileSize << "===========-------------------------===========" <<  endl; // debug

		// 原有的日志不需要分析
		char* strReq = (char*)"Req:";
		file.seekg(0, ios::beg);
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
			file.seekg(0, ios::end); // FIXME 必需确保是换行符
			llEndFilePos = file.tellg();
			llFileSize = llEndFilePos - llLastLinePos;
			/*
			while (file.get() != '\n');
			{
				++llFileSize;
			}
			*/
			llStart = llLastLinePos; // 开始读入位置回退到最末尾一行的行首
		}
		else
		{
			// TODO
			cout << "非req的行" << endl;
			file.clear();
			file.seekg(0, ios::end);
			llLastLinePos = file.tellg();
			Sleep(500);
			file.clear();
			file.seekg(0, ios::end);
			llEndFilePos = file.tellg();
			llFileSize = llEndFilePos - llLastLinePos;
			llStart = llLastLinePos; // 开始读入位置回退到最末尾一行的行首

		}

		cout << "需分析文件大小: " << llFileSize << "\t分析开始位置: " << llLastLinePos << "\t分析结束位置: " << llEndFilePos << endl; // debug

		ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount);

		// 清理
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

	}
	t_end = clock();

	cout << "\r\nAll completed in " << t_end - t_start << "ms." << endl; // debug
	return 0;
}


streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize)
{
	file->seekg(llStart + llSize);
	while (file->get() != '\n')
	{
		++llSize;
	}
	return llSize;
}


streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = loadedFile[iStep] + llStart + llSize;
	// while (*p != '\n')// 不用\n是因为当行尾没有\n时会无限循环
	while (*p != '\0')
	{
		++llSize;
		++p;
	}
	return llSize;
}


void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize)
{
	file->seekg(llStart);
	file->read(loadedFile[iStep], llSize);
}


vector<string> ReadLineToVec(int iStep, streamoff llStart, streamsize llSize)
{
	char *pFileBuffer = NULL;
	char *pLineBuffer = NULL;
	string sLineBuffer;
	string sFileBuffer;
	string sLine;
	vector<string> vecStringLine;

	pFileBuffer = loadedFile[iStep];

	sFileBuffer = pFileBuffer;
	sLineBuffer = sFileBuffer.substr(llStart, llSize);
	pLineBuffer = (char*)sLineBuffer.data();
	vecStringLine.clear();

	// XXX 换行符
	char *strDelim = (char*)"\n";
	char *strToken = NULL;
	char *nextToken = NULL;

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		vecStringLine.push_back(sLine);
		strToken = strtok_s(NULL, strDelim, &nextToken);
	}
	return vecStringLine;
}


void ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	// FIXME glog不是线程安全的?
	// LOG(INFO) << "id: " << id << "\tstrKey: " << strKey << endl;
	auto end = map->end();
	if (!vecStr.empty())
	{
		for (string::size_type i = 0; i < vecStr.size(); ++i)
		{
			MsgId = clUtils.GetMsgValue(vecStr[i], strKey);
			if (MsgId.empty())
			{
				// TODO
				// 没有MsgId的串为非req/ans 串不需要插入到map中
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
				/*
				char* strReq = (char*)"Req:";
				if ((strstr(vecStr[i].c_str(), strReq) != NULL) && (map->find(MsgId) == map->end()))
				{
					map->insert(pair<string, string>(MsgId, vecStr[i]));
				}
				*/
			}
		}
	}
}


// TODO
// void TimeoutScan(unordered_multimap<string, string> mymap)
void TimeoutScan(unordered_multimap<string, string> &mymap)
{
	int iLbmTimeOut;
	string strLbmTimeOut;
	// 遍历map, 对指定键的元素数小于2的，认为缺失req或ans
	auto begin = mymap.begin();
	cout << "\n==============-------------------------==============" << "allLogMap.size():   " << allLogMap.size() << "==============-------------------------==============" << endl;
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			time_t CurrTimeMs = clUtils.GetCurrentTimeMs();
			// time_t MsgTimeMs = clUtils.StringToMs(begin->second);
			time_t MsgTimeMs = clUtils.StringToMs(begin->second, 40, 53);
			
			// 获取配置
			if ((utilsError = clUtils.GetConfigValue(strLbmTimeOut, "LbmTimeOut")) != UTILS_RTMSG_OK )
			{
				LOG(ERROR) << "获取配置失败, 错误码: " << utilsError << endl;
				// FIXME
				// 异常抛出到界面
				abort();
			}
			iLbmTimeOut = stoi(strLbmTimeOut);

			if ((CurrTimeMs - MsgTimeMs) > iLbmTimeOut)
			{
				continue;
				/*
				cout << "当前时间(ms): " << CurrTimeMs << "\t" << "Msg时间(ms): " << MsgTimeMs << endl;
				char* pMsgData = (char*)begin->second.c_str();
				utilsError = clUtils.DoPost(pMsgData, strResponse);
				LOG(INFO) << "\n==============-------------------------==============" << "发送的数据为:   " << "==============-------------------------==============" << endl;
				LOG(INFO) << pMsgData << endl;

				if (UTILS_RTMSG_OK != utilsError)
				{
					LOG(ERROR) << "\n==============-------------------------==============" << "发POST请求失败! " << "==============-------------------------==============" << endl;
					LOG(ERROR) << "错误码: " << utilsError << endl;
				}
				else
				{
					LOG(ERROR) << "\n==============-------------------------==============" << "缺失的请求串为: " << "==============-------------------------==============" << endl;
					LOG(INFO) <<  clUtils.GetMsgValue(begin->second, "MsgId") << endl; // debug
					LOG(INFO) << "\n==============-------------------------==============" << "发POST请求成功! " << "==============-------------------------==============" << endl;
					LOG(INFO) << "\n==============-------------------------==============" << "收到的回复为:   " << "==============-------------------------==============" << endl;
					LOG(INFO) << strResponse << endl;
				}
				*/
			}
		}
		else
		{
			// 删除同时存在req、ans的
		}
	}
}


void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount)
{
	bool bBufferIndex = 0; // 缓存下标
	bool bNeedWait = false;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)
	streamsize llFileLen;
	thread *threads = new thread[iThreadCount];

	while (llFileSize)
	{
		// llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, llLoadSize) : llFileSize;
		llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoll(strLoadSize)) : llFileSize;
		llThreadIndex = 0;
		// llThreadIndex = llFileSize - iLineLen - 1; // 减去最后一行与上一行之间的\n字符
		llThreadPart = llRealSize / iThreadCount;

		cout << "\n\n\n读入数据到 loadedFile[bBufferIndex]: " << bBufferIndex << endl; // debug
		cout << "计划一次读入大小 llLoadSize: " << stoll(strLoadSize) << "\t实际一次读入大小 llRealSize: " << llRealSize << endl; // debug
		cout << "读入开始位置:llStart: " << llStart << "\t总文件剩余读取大小: llFileSize: " << llFileSize << "\t\t计划每个线程读入大小llThreadPart: " << llThreadPart << endl << endl << endl; // debug

		// 读入 realsize 大小的文件数据到缓存 loadedFile[step] 中
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
		}
		else
		{
			bNeedWait = true;
		}

		for (int i = 1; i < iThreadCount; ++i)
		{
			if (llThreadIndex != 0)
			{
				llThreadIndex += 1; // 下一个线程读取的开始位置, 跳过\n字符
			}

			if (llThreadIndex != llRealSize) // 避免文件只有一行时getBlockSize报错
			{
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// 若剩余行数少于线程数
			// 还需要判断一下线程已处理的字符和该块数据的大小
			if (llFileLen + llThreadIndex < llRealSize)
			{
				cout << "线程 " << i << " 开始读入位置llThreadIndex: " << llThreadIndex << "\t实际线程 " << i << " 读入大小llFileLen:" << llFileLen << endl; // debug
				vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
				cout << "线程 " << i << " 共读入: " << vecThreadLines.size() << "行" << endl << endl << endl; // debug
				threads[i] = thread(ParseMsgLine, vecThreadLines, i, "MsgId");
				llThreadIndex += llFileLen;
			}
			else
			{
				break;
			}
		}

		cout << "线程 4 开始读入位置 llThreadIndex: " << llThreadIndex << "\t; 实际线程 4 读入大小 llRealSize - llThreadIndex: " << llRealSize - llThreadIndex << endl; // debug
		if ((llRealSize - llThreadIndex) > 70)
		{
			vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
			threads[0] = thread(ParseMsgLine, vecEndThreadLines, 0, "MsgId");
			cout << "线程 4 " << "共读入: " << vecEndThreadLines.size() << "行" << endl; // debug
		}

		bBufferIndex = !bBufferIndex; // 切换 Buffer 装数据
		cout << "文件剩余大小: llFileSize: " << llFileSize << endl << endl << endl << endl; // debug

		// 文件滚动增长
		if (llFileSize != 0)
		{
			continue;
		}
		else
		{
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}
			bNeedWait = false;

			Sleep(500);// 文件扫描时间
			file.clear();
			file.seekg(0, ios::end); // 文件指针指到文件末尾
			streampos pNewPos = file.tellg(); // 新的文件指针
			llFileSize = pNewPos - pCurrPos;
			if (llFileSize < 0)
			{
				llFileSize = 0;
			}
			pCurrPos = pNewPos;
			// streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)
			cout << "==============-------------------------==============" << endl;
			cout << "增长文件大小: " << llFileSize << endl; // debug
			cout << "==============-------------------------==============" << endl;
		}

		// TimeOutScan
		// 阻塞主线程,等待上一个数据块分析结束,再对下一数据块进行分析

		for (int i = 0; i < iThreadCount; ++i)
		{
			LogMapKeySet p = (pLogMaps + i)->begin();
			LogMapKeySet end = (pLogMaps + i)->end();
			for (; p != end; ++p)
			{
				/*
				char* strReq = (char*)"Req:";
				string  MsgId = clUtils.GetMsgValue(p->second, "MsgId");
				if ((strstr(p->second.c_str(), strReq) != NULL) && (allLogMap.find(MsgId) == end))
				{
					allLogMap.insert(pair<string, string>(p->first, p->second));
				}
				*/
				allLogMap.insert(pair<string, string>(p->first, p->second));
			}
		}
		TimeoutScan(allLogMap);
		
	}

}

