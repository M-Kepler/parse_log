
#include "multi_thread.h"
#include "utils.h"

const int iThreadCount = 4;

streamsize llLoadSize = 8000; // XXX 调整 
char* loadedFile[2]; // 存放指向 char* 类型的指针的数组
typedef unordered_multimap<string, string> LogMap;
typedef unordered_multimap<string, string>::iterator LogMapKeySet;
LogMap *pLogMaps;
LogMap allLogMap;

CUtils clUtils;

vector<string> vecThreadLines, vecEndThreadLines;


int multi_thread()
{
	const char* filename = "runlog0-3.1.log";
	ios::sync_with_stdio(false);
	// XXX
	// 实际上这个map是需要一致存在的
	pLogMaps = new LogMap[iThreadCount];

	// 双缓冲
	// XXX delete的问题
	// 导致 delete loadedFile[1] 出错的问题在这里,这个512是为了防止阶段所以多设置出来的一段空间
	streamsize llMaxSize = llLoadSize + 512;
	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];

	clock_t t_start, t_end;
	t_start = clock();


	// 读取文件
	ifstream file;
	/*
	file.open(filename, ios::binary | ios::in);

	if (!file)
	{
		cout << "Error: File \"" << filename << "\" do not exist!" << endl;
		exit(1);
	}
	*/
	UtilsError errCode = clUtils.LoadFile(file, filename);
	if (UTILS_OPEN_SUCCESS != errCode)
	{
		return errCode;
	}
	else
	{
		/* 确认文件大小 bytes */
		streamoff llStart = 0;
		streamoff llFileSize; // 文件大小
		file.seekg(0, ios::end);
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
		cout << "文件大小: " << llFileSize << endl; // 123789

		while (llFileSize)
		{
			llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, llLoadSize) : llFileSize;
			llThreadIndex = 0;
			llThreadPart = llRealSize / iThreadCount;

			cout << "\n\n\n读入数据到 loadedFile[bBufferIndex]: " << bBufferIndex << endl;
			cout << "计划一次读入大小 llLoadSize: " << llLoadSize << "\t实际一次读入大小 llRealSize: " << llRealSize << endl;
			cout << "读入开始位置:llStart: " << llStart << "\t总文件剩余读取大小: llFileSize: " << llFileSize << "\t\t计划每个线程读入大小llThreadPart: " << llThreadPart << endl << endl << endl;

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
				// XXX
				// 如果只有两行, 不够分呢? 还需要判断一下线程已处理的字符和这一块数据的大小
				if (llFileLen + llThreadIndex < llRealSize)
				{
					cout << "线程 " << i << " 开始读入位置llThreadIndex: " << llThreadIndex << "\t实际线程 " << i << " 读入大小llFileLen:" << llFileLen << endl;
					vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
					cout << "线程 " << i << " 共读入: " << vecThreadLines.size() << "行" << endl << endl << endl;
					// threads[i] = thread(ParseMsgLine, std::ref(mymap), vecThreadLines, "MsgId");
					threads[i] = thread(ParseMsgLine, vecThreadLines, i, "MsgId");
					llThreadIndex += llFileLen;
				}
				else
				{
					break;
				}
			}

			cout << "线程 4 开始读入位置 llThreadIndex: " << llThreadIndex << "\t; 4 线程读入大小 llRealSize - llThreadIndex: " << llRealSize - llThreadIndex << endl;
			if ((llRealSize - llThreadIndex) > 70)
			{
				vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
				// threads[0] = thread(ParseMsgLine, std::ref(mymap), vecEndThreadLines, "MsgId");
				threads[0] = thread(ParseMsgLine, vecEndThreadLines, 0,"MsgId");
				cout << "线程: 4 " << " 共读入: " << vecEndThreadLines.size() << "行" << endl;
			}

			bBufferIndex = !bBufferIndex; // 切换 Buffer 装数据
			cout << "文件剩余大小: llFileSize: " << llFileSize << endl << endl << endl << endl;
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

	cout << "\r\nAll completed in " << t_end - t_start << "ms." << endl;
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
	cout << vecStringLine[0] << endl;
	cout << vecStringLine.back() << endl;

	return vecStringLine;
}


// TODO
// 对于一个请求分割多行的情况, 可否通过重载这个函数来实现, 判断行的开始和结束，并从日志行中提取需要的撞到map中
void ParseMsgLine( vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	auto end = map->end();
	if (!vecStr.empty())
	{
		// 如果日志行中没有MsgId呢?
		for (string::size_type i = 0; i < vecStr.size(); ++i)
		{
			MsgId = clUtils.GetMsgValue(vecStr[i], strKey);
			if (MsgId.empty())
			{
				// TODO
				// 对于非LBM req和ans的日志行
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
			}

		}
	}
}


// TODO 定时遍历map
// 扫描map,找出超时的lbm
// 扫描的时候是否需要考虑线程同步
void TimeoutScan(unordered_multimap<string, string> mymap)
{
	// 后面再遍历所有map, 对桶元素数量小于2的，认为确实req或ans
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\n缺失应答串的MsgId： \t" << clUtils.GetMsgValue(begin->second, "MsgId") << endl << endl;
			// cout << endl << begin->second << endl << endl;
		}
	}
}


