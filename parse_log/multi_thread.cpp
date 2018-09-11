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
	int iAnsNum;
	int iScanTime;
	string strLastLine;
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	string strAnsNum;
	string strScanTime;
	streamsize llLoadSize;
	streamsize llMaxSize;

	// 获取配置
	if ((utilsError = clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strAnsNum, "AnsNum")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strScanTime, "ScanTime")) != UTILS_RTMSG_OK
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
	iAnsNum = stoi(strAnsNum);
	iScanTime = stoi(strScanTime);

	// TODO
	// 实际上文件时每天一个的
	// const char* filename = "runlog0-3.1.log";
	const char* filename = "test.log";
	ios::sync_with_stdio(false);
	pLogMaps = new LogMap[iThreadCount];

	// 双缓冲
	llMaxSize = llLoadSize + iLineBuffer;

	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];



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
		/*
		file.clear();
		file.seekg(0, ios::beg); // 从后往前读

		// XXX 改为从后往前读一行
		while (file.peek() != EOF)
		{
			getline(file, strLastLine);
		}
		*/
		vector<string> vecTailLine;
		utilsError = clUtils.TailLine(file, 1, vecTailLine);
		if (utilsError != UTILS_RTMSG_OK)
		{
			abort();
		}
		strLastLine = vecTailLine[0];
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
		else
		{
			file.clear();
			file.seekg(0, ios::end);
			llLastLinePos = file.tellg();
			// XXX WIN
			Sleep(iScanTime);
			file.clear();
			file.seekg(0, ios::end);
			llEndFilePos = file.tellg();
			llFileSize = llEndFilePos - llLastLinePos;
			llStart = llLastLinePos; // 开始读入位置回退到最末尾一行的行首
		}

		cout << "需分析文件大小: " << llFileSize << "\t分析开始位置: " << llLastLinePos << "\t分析结束位置: " << llEndFilePos << endl; // debug

		ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount, iAnsNum, iScanTime);

		// 清理
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

	}
	return 0;
}


streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize)
{
	file->seekg(llStart + llSize);
	// while (file->get() != '\n')
	// FIXME 文件增长太快的时候, 这里会掉进死循环
	// creator.out 如每2、3毫秒插入两条数据的时候可重现
	// while ((file->get() != '\n') || (file->get() != '\0'))
	// FIXME
	while ((file->peek() != EOF) && ((file->get()!= '\n') || (file->get()!= '\0')))
	{
		++llSize;
	}
	return llSize;
}


streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = loadedFile[iStep] + llStart + llSize;
	// while (*p != '\n')// 不用\n是因为没有\n时会无限循环, 比如静态文件只有一行数据, 所以加上\0一起做判断
	// FIXME 20180910 - 19:07
	while (*p != '\n' && *p != '\0')
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
	// 这个llSize有问题
	sLineBuffer = sFileBuffer.substr(llStart, llSize);
	pLineBuffer = (char*)sLineBuffer.c_str();
	string tmpLineBuffer = pLineBuffer;
	vecStringLine.clear();

	// XXX 换行符
	char *strDelim = (char*)"\n";
	char *strToken = NULL;
	char *nextToken = NULL;

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		// test begin
		if (sLine.length() < 60)
		{
			abort();
		}
		// test end
		vecStringLine.push_back(sLine);
		strToken = strtok_s(NULL, strDelim, &nextToken);
	}
	return vecStringLine;
}


void ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	// XXX glog不是线程安全的?
	// LOG(INFO) << "id: " << id << "\tstrKey: " << strKey << endl;
	auto end = map->end();
	if (!vecStr.empty())
	{
		for (string::size_type i = 0; i < vecStr.size(); ++i)
		{
			MsgId = clUtils.GetMsgValue(vecStr[i], strKey);
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


void TimeoutScan(unordered_multimap<string, string> &mymap, int iAnsNum)
{
	int iLbmTimeOut;
	string strLbmTimeOut;
	string strPostData;
	auto begin = mymap.begin();
	auto end = mymap.end();
	cout << "\n==============-------------------------==============" << "allLogMap.size():   " << allLogMap.size() << "==============-------------------------==============" << endl;
	for (; begin != end; )
	{
		if (mymap.count(begin->first) < iAnsNum)
		{
			time_t CurrTimeMs = clUtils.GetCurrentTimeMs();
			time_t MsgTimeMs = clUtils.StringToMs(begin->second);
			
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
				// ans串可能在下一个内存块,所以超时才删
				cout << "超时+发送+删除\t" << CurrTimeMs << "\t" << MsgTimeMs << "\t" << CurrTimeMs - MsgTimeMs << "\t" << begin->second << endl;
				strPostData = clUtils.AssembleJson(begin->second);
				cout << "\n==============-------------------------==============" << "发送的数据为:   " << "==============-------------------------==============" << endl;
				cout << strPostData << endl;

				// 发送并删除
				/*
				char* pMsgData = (char*)strPostData.c_str();
				// utilsError = clUtils.DoPost(pMsgData, strResponse);
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

				// begin++;
				begin = mymap.erase(begin);
				begin = mymap.begin();
			}
			else
			{
				begin++;
			}
		}
		else
		{
			// 删除同时存在req、ans的
			string key = begin->first;
			mymap.erase(begin->first);
			begin = mymap.begin();
			// end = mymap.end();
		}
	}
	cout << "\n==============-------------------------==============" << "删除后allLogMap.size():   " << allLogMap.size() << "==============-------------------------==============" << endl;
}


void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime)
{

	clock_t t_start, t_end;

	bool bBufferIndex = 0; // 缓存下标
	bool bNeedWait = false;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)
	streamsize llFileLen;
	thread *threads = new thread[iThreadCount];

	while (llFileSize)
	{
		t_start = clock();

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
			for (int i = 0; i < iThreadCount; ++i)
			{
				LogMapKeySet p = (pLogMaps + i)->begin();
				LogMapKeySet end = (pLogMaps + i)->end();
				for (; p != end; ++p)
				{
					allLogMap.insert(pair<string, string>(p->first, p->second));
				}
				// 清理掉线程的子map
				LogMap *ThreadMap = pLogMaps + i;
				ThreadMap->clear();
			}

			TimeoutScan(allLogMap, iAnsNum);

		}
		else
		{
			bNeedWait = true;
		}

		t_end = clock();
		cout << "==============-------------------------============== Scan Complete: " << t_end - t_start << "ms." << "==============-------------------------==============" << endl; // debug


		for (int i = 1; i < iThreadCount; ++i)
		{
			if (llThreadIndex != 0)
			{
				llThreadIndex += 1; // 下一个线程读取的开始位置, 跳过\n字符
				// llThreadIndex += 2; // 下一个线程读取的开始位置, 跳过\n字符
			}

			if (llThreadIndex != llRealSize) // 避免文件只有一行时getBlockSize报错
			{
				// FIXME
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// 若剩余行数少于线程数,还需要判断一下线程已处理的字符和该块数据的大小
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
		// XXX
		// 一行请求不可能少于70个字符吧
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
			// bNeedWait = false;

			// XXX WIN
			Sleep(iScanTime);
			
			file.clear();
			file.seekg(0, ios::end); // 文件指针指到文件末尾
			streampos pNewPos = file.tellg(); // 新的文件指针
			// test begin
			if (file.peek() == '\n')
			{
				abort();
			}
			// test end
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
		
	}

}

