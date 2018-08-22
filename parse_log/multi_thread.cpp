
#include "multi_thread.h"
#include "utils.h"

const int iThreadCount = 4;

streamsize llLoadSize = 8000; // XXX ���� 
char* loadedFile[2]; // ���ָ�� char* ���͵�ָ�������
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
	// ʵ�������map����Ҫһ�´��ڵ�
	pLogMaps = new LogMap[iThreadCount];

	// ˫����
	// XXX delete������
	// ���� delete loadedFile[1] ���������������,���512��Ϊ�˷�ֹ�׶����Զ����ó�����һ�οռ�
	streamsize llMaxSize = llLoadSize + 512;
	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];

	clock_t t_start, t_end;
	t_start = clock();


	// ��ȡ�ļ�
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
		/* ȷ���ļ���С bytes */
		streamoff llStart = 0;
		streamoff llFileSize; // �ļ���С
		file.seekg(0, ios::end);
		streamoff llFileLen = file.tellg();
		streamoff llThreadIndex, llThreadPart;
		streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
		bool bBufferIndex = 0; // �����±�
		bool bNeedWait = false;
		thread *threads = new thread[iThreadCount];

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
		cout << "�ļ���С: " << llFileSize << endl; // 123789

		while (llFileSize)
		{
			llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, llLoadSize) : llFileSize;
			llThreadIndex = 0;
			llThreadPart = llRealSize / iThreadCount;

			cout << "\n\n\n�������ݵ� loadedFile[bBufferIndex]: " << bBufferIndex << endl;
			cout << "�ƻ�һ�ζ����С llLoadSize: " << llLoadSize << "\tʵ��һ�ζ����С llRealSize: " << llRealSize << endl;
			cout << "���뿪ʼλ��:llStart: " << llStart << "\t���ļ�ʣ���ȡ��С: llFileSize: " << llFileSize << "\t\t�ƻ�ÿ���̶߳����СllThreadPart: " << llThreadPart << endl << endl << endl;

			/* ���� realsize ��С���ļ����ݵ����� loadedFile[step] �� */
			readLoad(bBufferIndex, &file, llStart, llRealSize);
			llStart += llRealSize;
			llFileSize -= llRealSize;


			/* �������߳�,�ȴ���һ�����ݿ��������,�ٶ���һ���ݿ���з���*/
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
					llThreadIndex += 1; // ��һ���̶߳�ȡ�Ŀ�ʼλ��, ����\n�ַ�
				}

				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
				// XXX
				// ���ֻ������, ��������? ����Ҫ�ж�һ���߳��Ѵ�����ַ�����һ�����ݵĴ�С
				if (llFileLen + llThreadIndex < llRealSize)
				{
					cout << "�߳� " << i << " ��ʼ����λ��llThreadIndex: " << llThreadIndex << "\tʵ���߳� " << i << " �����СllFileLen:" << llFileLen << endl;
					vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
					cout << "�߳� " << i << " ������: " << vecThreadLines.size() << "��" << endl << endl << endl;
					// threads[i] = thread(ParseMsgLine, std::ref(mymap), vecThreadLines, "MsgId");
					threads[i] = thread(ParseMsgLine, vecThreadLines, i, "MsgId");
					llThreadIndex += llFileLen;
				}
				else
				{
					break;
				}
			}

			cout << "�߳� 4 ��ʼ����λ�� llThreadIndex: " << llThreadIndex << "\t; 4 �̶߳����С llRealSize - llThreadIndex: " << llRealSize - llThreadIndex << endl;
			if ((llRealSize - llThreadIndex) > 70)
			{
				vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
				// threads[0] = thread(ParseMsgLine, std::ref(mymap), vecEndThreadLines, "MsgId");
				threads[0] = thread(ParseMsgLine, vecEndThreadLines, 0,"MsgId");
				cout << "�߳�: 4 " << " ������: " << vecEndThreadLines.size() << "��" << endl;
			}

			bBufferIndex = !bBufferIndex; // �л� Buffer װ����
			cout << "�ļ�ʣ���С: llFileSize: " << llFileSize << endl << endl << endl << endl;
		}

		// ����
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

		// ���map
		// ÿ���̶߳Ը��Ե�map���в������,������releaseģʽ������, debugģʽ��ż���߳�langס������
		// ����֮ǰʱ��600ms���ң�������1000ms���ҡ�����
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
	// XXX ���з�
	while (file->get() != '\n')
	{
		++llSize;
	}
	return llSize;
}


streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = loadedFile[iStep] + llStart + llSize;
	// XXX ���з�
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

	// XXX ���з�
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
// ����һ������ָ���е����, �ɷ�ͨ���������������ʵ��, �ж��еĿ�ʼ�ͽ�����������־������ȡ��Ҫ��ײ��map��
void ParseMsgLine( vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	auto end = map->end();
	if (!vecStr.empty())
	{
		// �����־����û��MsgId��?
		for (string::size_type i = 0; i < vecStr.size(); ++i)
		{
			MsgId = clUtils.GetMsgValue(vecStr[i], strKey);
			if (MsgId.empty())
			{
				// TODO
				// ���ڷ�LBM req��ans����־��
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
			}

		}
	}
}


// TODO ��ʱ����map
// ɨ��map,�ҳ���ʱ��lbm
// ɨ���ʱ���Ƿ���Ҫ�����߳�ͬ��
void TimeoutScan(unordered_multimap<string, string> mymap)
{
	// �����ٱ�������map, ��ͰԪ������С��2�ģ���Ϊȷʵreq��ans
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\nȱʧӦ�𴮵�MsgId�� \t" << clUtils.GetMsgValue(begin->second, "MsgId") << endl << endl;
			// cout << endl << begin->second << endl << endl;
		}
	}
}


