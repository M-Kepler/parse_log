
#include "multi_thread.h"
#include "utils.h"

bool words[128]; // ascii表
int threadCount = 4;

streamsize loadsize = 8000; // XXX 调整 
char *loadedFile[2]; // 存放指向 char* 类型的指针的数组
HashMap *wordMaps;


unordered_multimap<string, string> mymap;
unordered_multimap<string, string>::iterator curr;
vector<string> sevc;
vector<string> sevc0;
// vector<string> vStringLine;

int multi_thread()
{
	const char* filename = "runlog0-3.1.log";
	ios::sync_with_stdio(false);
	// wordMaps = new HashMap[threadCount];

	// 双缓冲
	streamsize maxsize = loadsize + 512; // XXX 导致 delete loadedFile[1] 出错的问题在这里,这个512是为了防止阶段所以多设置出来的一段空间
	loadedFile[0] = new char[maxsize];
	loadedFile[1] = new char[maxsize];

	time_t t_start, t_end;
	t_start = time(NULL);

	/*
	memset(words, false, 128);
	// ascii中97是a, 122是z
	for (char c = 97; c != 123; ++c)
	{
		words[int(c)] = true;
	}
	// ascii中65是A, 90是Z
	for (char c = 65; c != 91; ++c)
	{
		words[int(c)] = true;
	}
	// ascii中48是0, 57是9
	for (char c = 48; c != 58; ++c)
	{
		words[int(c)] = true;
	}
	*/

	// 读取文件
	ifstream file;
	file.open(filename, ios::binary | ios::in);

	if (!file)
	{
		cout << "Error: File \"" << filename << "\" do not exist!" << endl;
		exit(1);
	}
	else
	{
		/* 确认文件大小 bytes */
		streamoff start = 0;
		streamoff size; // 文件大小
		file.seekg(0, ios::end);
		streamoff len = file.tellg();

		/* 兼容带 bom 的 utf8 编码格式 */
		if (len > 3)
		{
			char bom[3];
			file.seekg(0);
			file.read(bom, 3);
			if (bom[0] == -17 && bom[1] == -69 && bom[2] == -65)
			{
				start = 3;
				size = len - 3;
			}
			else
			{
				size = len;
			}
		}
		else
		{
			size = len;
		}
		cout << "文件大小" << size  << endl; // 123789

		thread *threads = new thread[threadCount];
		streamoff index, part;
		streamsize realsize; // 实际读入大小(因为可能遇到需要的单词被截位)
		bool step = 0; // 缓存下标
		bool needWait = false;

		while (size)
		{
			realsize = size > maxsize ? getRealSize(&file, start, loadsize) : size;
			index = 0;
			part = realsize / threadCount;

			cout << "\n\n\n读入数据到loadedFile[step]: " << step << endl;
			cout << "计划一次读入大小loadedsize: " << loadsize << "\t实际一次读入大小realsize: " << realsize << endl;
			cout << "读入开始位置:start: " << start << "\t总文件剩余读取大小: size: " << size << "\t\t计划每个线程读入大小part: " << part << endl << endl << endl;


			/* 读入 realsize 大小的文件数据到缓存 loadedFile[step] 中 */
			readLoad(step, &file, start, realsize);
			start += realsize;
			size -= realsize;


			/* 阻塞主线程,等待上一个数据块分析结束,再对下一数据块进行分析*/
			if (needWait)
			{
				for (int i = 0; i < threadCount; ++i)
				{
					threads[i].join();
				}
			}
			else
			{
				needWait = true;
			}

			for (int i = 1; i < threadCount; ++i)
			{
				if (index != 0)
				{
					index += 1; // 下一个线程读取的开始位置跳过\n字符
				}

				len = getBlockSize(step, index, part);
				// TODO
				// 如果只有两行, 不够分呢? 还需要判断一下线程已处理的字符和这一块数据的大小
				if (len + index < realsize)
				{
					cout << "线程 " << i << " 开始读入位置index: " << index << "\t实际线程 " << i << " 读入大小len:" << len << endl;
					sevc = ReadLineToVec(step, index, len, i);
					cout << "线程 " << i << " 共读入: " << sevc.size() << "行" << endl << endl << endl;
					threads[i] = thread(ParseMsgLine, sevc, "MsgId");
					index += len;
				}
				else
				{
					break;
				}
			}

			cout << "线程 4 开始读入位置 index: " << index << "\t; 4 线程读入大小 real-index: " << realsize - index << endl;
			if ((realsize - index) > 70)
			{
				sevc0 = ReadLineToVec(step, index, realsize - index);
				threads[0] = thread(ParseMsgLine, sevc0, "MsgId");
				cout << "线程: 4 " << " 共读入: " << sevc0.size() << "行" << endl;
			}

			step = !step; // 切换 Buffer 装数据
			cout << "文件剩余大小: size: " << size << endl << endl << endl << endl;
		}

		cout << "Clear" << endl;

		delete loadedFile[0];
		delete loadedFile[1];
		file.close();
	}
	t_end = time(NULL);

	cout << "\r\nAll completed in " << difftime(t_end, t_start) << "s." << endl;
	return 0;
}


streamsize inline getRealSize(ifstream *file, streamoff start, streamsize size)
{
	file->seekg(start + size);
	// TODO
	// 从配置文件获取
	while (file->get() != '\n')
	{
		++size;
	}
	return size;
}


streamsize inline getBlockSize(int step, streamoff start, streamsize size)
{
	char *p = loadedFile[step] + start + size;
	while (*p != '\n')
	{
		++size;
		++p;
	}
	return size;
}


void inline readLoad(int step, ifstream *file, streamoff start, streamsize size)
{
	file->seekg(start);
	file->read(loadedFile[step], size);
}


vector<string> ReadLineToVec(int step, streamoff start, streamsize size, int id)
{
	char *pFileBuffer = NULL;
	char *pLineBuffer = NULL;
	string sLineBuffer;
	// char *lineBuffer = NULL;
	string sFileBuffer;
	string sLine;
	vector<string> vStringLine;

	pFileBuffer = loadedFile[step];

	// debug
	sFileBuffer = pFileBuffer;
	sLineBuffer = sFileBuffer.substr(start, size);
	pLineBuffer = (char*)sLineBuffer.data();
	vStringLine.clear();

	// FIXME
	// 行分割符\r\n \n ; windows系统和unix系统的分割符不一样; 期望通过配置文件或自动知道, 也可以统一用\n为分隔符, 处理的时候兼容\r 
	// 现在这样处理,每个vector元素末尾都有一个'\r'
	char *strDelim = (char*)"\n";
	char *strToken = NULL;
	char *nextToken = NULL;

	strToken = strtok_s(pLineBuffer, strDelim, &nextToken);
	while (strToken != NULL)
	{
		sLine.assign(strToken);
		vStringLine.push_back(sLine);
		strToken = strtok_s(NULL, strDelim, &nextToken);
	}
	cout << vStringLine[0] << endl ;
	cout << vStringLine.back() << endl ;

	return vStringLine;
}


void ParseMsgLine(vector<string> sevc, string KeyStr)
{
	string MsgId;

	auto end = mymap.end();
	if (!sevc.empty())
	{
		for (string::size_type i = 0; i < sevc.size(); ++i)
		{
			MsgId = GetMsgValue(sevc[i], KeyStr);
			mymap.insert(pair<string, string>(MsgId, sevc[i]));
		}
	}

	// TODO 后面再遍历所有map, 对桶元素数量小于2的，认为确实req或ans
	/*
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\n缺失应答串的MsgId： \t" << GetMsgValue(begin->second, "MsgId") << endl << endl;
			cout << endl << begin->second << endl << endl;
		}
	}
	*/
}

