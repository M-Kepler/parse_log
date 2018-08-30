/*
 * 程序第一次开始运行时, 已存在的日志内容不需要读取;
 * 所以如果开始运行时, 已存在的日志中最后一行是ans串,则应该丢弃掉该行
 *
 * 操作员登录的时候有一条是请求所有菜单时, 返回的串长度是很长很长的
 *
 * 还没试过日志不断增长的情况
 *
*/
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


int multi_thread()
{
	int iThreadCount;
	int iLineBuffer;
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	streamsize llLoadSize;
	streamsize llMaxSize;

	// enumUtilsError = clUtils.GetConfigValue(iLineBuffer, "LoadSize");

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

	const char* filename = "runlog0-3.1.log";
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
		file.seekg(0, ios::end); // 文件指针指到文件末尾
		streamoff llFileLen = file.tellg();
		streamoff llThreadIndex, llThreadPart;
		streamsize llRealSize; // 实际读入大小(因为可能遇到需要的单词被截位)
		bool bBufferIndex = 0; // 缓存下标
		bool bNeedWait = false;
		thread *threads = new thread[iThreadCount];

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
		cout << "文件大小: " << llFileSize << endl; // debug

		while (llFileSize)
		{
			llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoi(strLoadSize)) : llFileSize;
			llThreadIndex = 0;
			llThreadPart = llRealSize / iThreadCount;

			cout << "\n\n\n读入数据到 loadedFile[bBufferIndex]: " << bBufferIndex << endl; // debug
			cout << "计划一次读入大小 llLoadSize: " << llLoadSize << "\t实际一次读入大小 llRealSize: " << llRealSize << endl; // debug
			cout << "读入开始位置:llStart: " << llStart << "\t总文件剩余读取大小: llFileSize: " << llFileSize << "\t\t计划每个线程读入大小llThreadPart: " << llThreadPart << endl << endl << endl; // debug

			/* 读入 realsize 大小的文件数据到缓存 loadedFile[step] 中 */
			readLoad(bBufferIndex, &file, llStart, llRealSize);
			llStart += llRealSize;
			llFileSize -= llRealSize;

			/* 阻塞主线程,等待上一个数据块分析结束,再对下一数据块进行分析*/
			if (bNeedWait)
			{
				for (int i = 0; i < iThreadCount; ++i)
				{
					threads[i].join();
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

				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);

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
		}

		// 清理
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

		// 组合map
		// 每个线程对各自的map进行插入操作,避免了release模式下正常, debug模式下偶现线程lang住的问题
		// 但是之前时间600ms左右，现在是1000ms左右。。。
		//	LogMap *map = pLogMaps + id;
		for (int i = 0; i < iThreadCount; ++i)
		{
			LogMapKeySet p = (pLogMaps + i)->begin();
			LogMapKeySet end = (pLogMaps + i)->end();
			for (; p != end; ++p)
			{
				allLogMap.insert(pair<string, string>(p->first, p->second));
			}
		}
	}
	TimeoutScan(allLogMap);
	t_end = clock();

	cout << "\r\nAll completed in " << t_end - t_start << "ms." << endl; // debug
	return 0;
}


streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize)
{
	file->seekg(llStart + llSize);
	// XXX 换行符
	while (file->get() != '\n')
	{
		++llSize;
	}
	return llSize;
}


streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = loadedFile[iStep] + llStart + llSize;
	// XXX 换行符
	while (*p != '\n')
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


// TODO
// 多行
// 第一结果集
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

#ifdef WINDOWS
	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
#else

#endif
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		vecStringLine.push_back(sLine);
#ifdef WINDOWS
		strToken = strtok_s(NULL, strDelim, &nextToken);
#else
#endif
	}
	/*
	cout << vecStringLine[0] << endl; // debug
	cout << vecStringLine.back() << endl; // debug
	*/

	return vecStringLine;
}


void ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	// FIXME
	// 加上这句后, 偶现超时的输出有问题
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
				// ORA错误
				// 对于一个请求分割多行的情况, 可否通过重载这个函数来实现, 判断行的开始和结束，并从日志行中提取需要的撞到map中
				// 如果日志行中没有MsgId呢?
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
			}
		}
	}
}


// TODO 定时遍历map
// 扫描map, 找出超时的lbm
// 扫描map, 找出应答错误的lbm
// 扫描的时候是否需要考虑线程同步
void TimeoutScan(unordered_multimap<string, string> mymap)
{
	// 遍历map, 对指定键的元素数小于2的，认为缺失req或ans
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			// XXX
			char* pMsgData = (char*)begin->second.c_str();
			utilsError = clUtils.DoPost(pMsgData, strResponse);
			if (UTILS_RTMSG_OK != utilsError)
			{
				LOG(ERROR) << "发送POST请求失败! 错误码: " << utilsError << endl;
			}
			else
			{
				cout << "\n缺失应答串的MsgId： \t" << clUtils.GetMsgValue(begin->second, "MsgId") << endl; // debug
				LOG(INFO) << "\n==========================================发POST请求成功! " << "===========================================" << endl;
				LOG(INFO) << "\n==========================================发送的数据为:   " << "===========================================" << endl;
				LOG(INFO) << pMsgData << endl;
				LOG(INFO) << "\n==========================================收到的回复为:   " << "===========================================" << endl;
				LOG(INFO) << strResponse << endl;
				LOG(INFO) << "\n==========================================================================================================" << endl;
			}
		}
	}
}
