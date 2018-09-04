#include "multi_thread.h"
#include "utils.h"

char* loadedFile[2]; // ���ָ�� char* ���͵�ָ�������
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
	char* strReq;
	int iLineLen;
	int iThreadCount;
	int iLineBuffer;
	string strLastLine;
	string strLineBuffer;
	string strLoadSize;
	string strThreadCount;
	streamsize llLoadSize;
	streamsize llMaxSize;


	if ((utilsError = clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << utilsError << endl;
		// FIXME
		// �쳣�׳�������
		abort();
	}
	iThreadCount = stoi(strThreadCount);
	llLoadSize = stoll(strLoadSize);
	iLineBuffer = stoi(strLineBuffer);

	const char* filename = "runlog0-3.1.log";
	// const char* filename = "test.log";
	ios::sync_with_stdio(false);
	pLogMaps = new LogMap[iThreadCount];

	// ˫����
	llMaxSize = llLoadSize + iLineBuffer;

	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];

	clock_t t_start, t_end;
	t_start = clock();


	// ��ȡ�ļ�
	ifstream file;
	utilsError = clUtils.LoadFile(file, filename);
	if (UTILS_OPEN_SUCCESS != utilsError)
	{
		return utilsError;
	}
	else
	{
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
		thread *threads = new thread[iThreadCount];

		// TODO ����
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
		cout << "�ļ���С: " << llFileSize << endl; // debug


		// TODO
		// ԭ�е���־����Ҫ����
		strReq = (char*)"Req:";
		while (file.peek() != EOF)
		{
			getline(file, strLastLine);
		}
		iLineLen = strLastLine.length();
		if (strstr(strLastLine.c_str(), strReq) != NULL)
		{
			file.seekg(-(iLineLen + 1), ios::end);
			llLastLinePos = file.tellg();
			file.seekg(0, ios::end);
			llEndFilePos = file.tellg();

			llFileSize = llEndFilePos - llLastLinePos;
			llStart = llLastLinePos; // ��ʼ����λ�û��˵���ĩβһ�е�����
		}
		else
		{
			// FIXME 
			// �ȴ��ļ�����
		}

		ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount);

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
// ��һ�����
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
	return vecStringLine;
}


void ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	// FIXME
	// ��������, ż�ֳ�ʱ�����������
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
				// ORA����
				// ����һ������ָ���е����, �ɷ�ͨ���������������ʵ��, �ж��еĿ�ʼ�ͽ�����������־������ȡ��Ҫ��ײ��map��
				// �����־����û��MsgId��?
			}
			else
			{
				map->insert(pair<string, string>(MsgId, vecStr[i]));
			}
		}
	}
}


// TODO ��ʱ����map
// ɨ��map, �ҳ���ʱ��lbm
// ɨ��map, �ҳ�Ӧ������lbm
// ɨ���ʱ���Ƿ���Ҫ�����߳�ͬ��
void TimeoutScan(unordered_multimap<string, string> mymap)
{
	// ����map, ��ָ������Ԫ����С��2�ģ���Ϊȱʧreq��ans
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			char* pMsgData = (char*)begin->second.c_str();
			utilsError = clUtils.DoPost(pMsgData, strResponse);
			LOG(INFO) << "\n==============-------------------------==============" << "���͵�����Ϊ:   " << "==============-------------------------==============" << endl;
			LOG(INFO) << pMsgData << endl;

			if (UTILS_RTMSG_OK != utilsError)
			{
				LOG(ERROR) << "\n==============-------------------------==============" << "��POST����ʧ��! " << "==============-------------------------==============" << endl;
				LOG(ERROR) << "������: " << utilsError << endl;
			}
			else
			{
				cout << "\nȱʧӦ�𴮵�MsgId�� \t" << clUtils.GetMsgValue(begin->second, "MsgId") << endl; // debug
				LOG(INFO) << "\n==============-------------------------==============" << "��POST����ɹ�! " << "==============-------------------------==============" << endl;
				LOG(INFO) << "\n==============-------------------------==============" << "�յ��Ļظ�Ϊ:   " << "==============-------------------------==============" << endl;
				LOG(INFO) << strResponse << endl;
			}
		}
	}
}


void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount)
{
	bool bBufferIndex = 0; // �����±�
	bool bNeedWait = false;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
	streamsize llFileLen;
	thread *threads = new thread[iThreadCount];

	while (llFileSize)
	{
		// llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, llLoadSize) : llFileSize;
		llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoll(strLoadSize)) : llFileSize;
		llThreadIndex = 0;
		// llThreadIndex = llFileSize - iLineLen - 1; // ��ȥ���һ������һ��֮���\n�ַ�
		llThreadPart = llRealSize / iThreadCount;

		cout << "\n\n\n�������ݵ� loadedFile[bBufferIndex]: " << bBufferIndex << endl; // debug
		cout << "�ƻ�һ�ζ����С llLoadSize: " << stoll(strLoadSize) << "\tʵ��һ�ζ����С llRealSize: " << llRealSize << endl; // debug
		cout << "���뿪ʼλ��:llStart: " << llStart << "\t���ļ�ʣ���ȡ��С: llFileSize: " << llFileSize << "\t\t�ƻ�ÿ���̶߳����СllThreadPart: " << llThreadPart << endl << endl << endl; // debug

		// ���� realsize ��С���ļ����ݵ����� loadedFile[step] ��
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

			if (llThreadIndex != llRealSize) // �����ļ�ֻ��һ��ʱgetBlockSize����
			{
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// ��ʣ�����������߳���
			// ����Ҫ�ж�һ���߳��Ѵ�����ַ��͸ÿ����ݵĴ�С
			if (llFileLen + llThreadIndex < llRealSize)
			{
				cout << "�߳� " << i << " ��ʼ����λ��llThreadIndex: " << llThreadIndex << "\tʵ���߳� " << i << " �����СllFileLen:" << llFileLen << endl; // debug
				vecThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llFileLen);
				cout << "�߳� " << i << " ������: " << vecThreadLines.size() << "��" << endl << endl << endl; // debug
				threads[i] = thread(ParseMsgLine, vecThreadLines, i, "MsgId");
				llThreadIndex += llFileLen;
			}
			else
			{
				break;
			}
		}

		cout << "�߳� 4 ��ʼ����λ�� llThreadIndex: " << llThreadIndex << "\t; ʵ���߳� 4 �����С llRealSize - llThreadIndex: " << llRealSize - llThreadIndex << endl; // debug
		if ((llRealSize - llThreadIndex) > 70)
		{
			vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
			threads[0] = thread(ParseMsgLine, vecEndThreadLines, 0, "MsgId");
			cout << "�߳� 4 " << "������: " << vecEndThreadLines.size() << "��" << endl; // debug
		}

		bBufferIndex = !bBufferIndex; // �л� Buffer װ����
		cout << "�ļ�ʣ���С: llFileSize: " << llFileSize << endl << endl << endl << endl; // debug

		// �ļ���������
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

			Sleep(3000);// �ļ�ɨ��ʱ��
			file.seekg(0, ios::end); // �ļ�ָ��ָ���ļ�ĩβ
			streampos pNewPos = file.tellg(); // �µ��ļ�ָ��

			llFileSize = pNewPos - pCurrPos;
			pCurrPos = pNewPos;
			// streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
			cout << "==============-------------------------==============" << endl;
			cout << "�����ļ���С: " << llFileSize << endl; // debug
			cout << "==============-------------------------==============" << endl;
		}
	}

}