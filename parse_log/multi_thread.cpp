/*
 * �����һ�ο�ʼ����ʱ, �Ѵ��ڵ���־���ݲ���Ҫ��ȡ;
 * ���������ʼ����ʱ, �Ѵ��ڵ���־�����һ����ans��,��Ӧ�ö���������
 *
 * ����Ա��¼��ʱ����һ�����������в˵�ʱ, ���صĴ������Ǻܳ��ܳ���
 *
 * ��û�Թ���־�������������
 *
*/
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
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << utilsError << endl;
		// FIXME
		// �쳣�׳�������
		abort();
	}
	iThreadCount = stoi(strThreadCount);
	llLoadSize = stoll(strLoadSize);
	iLineBuffer = stoi(strLineBuffer);

	const char* filename = "runlog0-3.1.log";
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
		file.seekg(0, ios::end); // �ļ�ָ��ָ���ļ�ĩβ
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
		cout << "�ļ���С: " << llFileSize << endl; // debug

		while (llFileSize)
		{
			llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoi(strLoadSize)) : llFileSize;
			llThreadIndex = 0;
			llThreadPart = llRealSize / iThreadCount;

			cout << "\n\n\n�������ݵ� loadedFile[bBufferIndex]: " << bBufferIndex << endl; // debug
			cout << "�ƻ�һ�ζ����С llLoadSize: " << llLoadSize << "\tʵ��һ�ζ����С llRealSize: " << llRealSize << endl; // debug
			cout << "���뿪ʼλ��:llStart: " << llStart << "\t���ļ�ʣ���ȡ��С: llFileSize: " << llFileSize << "\t\t�ƻ�ÿ���̶߳����СllThreadPart: " << llThreadPart << endl << endl << endl; // debug

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

	cout << "\r\nAll completed in " << t_end - t_start << "ms." << endl; // debug
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


// TODO
// ����
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
			// XXX
			char* pMsgData = (char*)begin->second.c_str();
			utilsError = clUtils.DoPost(pMsgData, strResponse);
			if (UTILS_RTMSG_OK != utilsError)
			{
				LOG(ERROR) << "����POST����ʧ��! ������: " << utilsError << endl;
			}
			else
			{
				cout << "\nȱʧӦ�𴮵�MsgId�� \t" << clUtils.GetMsgValue(begin->second, "MsgId") << endl; // debug
				LOG(INFO) << "\n==========================================��POST����ɹ�! " << "===========================================" << endl;
				LOG(INFO) << "\n==========================================���͵�����Ϊ:   " << "===========================================" << endl;
				LOG(INFO) << pMsgData << endl;
				LOG(INFO) << "\n==========================================�յ��Ļظ�Ϊ:   " << "===========================================" << endl;
				LOG(INFO) << strResponse << endl;
				LOG(INFO) << "\n==========================================================================================================" << endl;
			}
		}
	}
}
