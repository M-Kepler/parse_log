#include "multi_thread.h"

char* loadedFile[2]; // ���ָ�� char* ���͵�ָ�������
typedef unordered_multimap<string, string> LogMap;
typedef unordered_multimap<string, string>::iterator LogMapKeySet;
LogMap *pLogMaps;

LogMap allLogMap;
LogMap *ThreadMap;

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

	// ��ȡ����
	if ((utilsError = clUtils.GetConfigValue(strLoadSize, "LoadSize")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strThreadCount, "ThreadCount")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strLineBuffer, "LineBuffer")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strAnsNum, "AnsNum")) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(strScanTime, "ScanTime")) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << utilsError << endl;
		// TODO �쳣�׳�
		abort();
	}
	iThreadCount = stoi(strThreadCount);
	llLoadSize = stoll(strLoadSize);
	iLineBuffer = stoi(strLineBuffer);
	iAnsNum = stoi(strAnsNum);
	iScanTime = stoi(strScanTime);

	// TODO
	// ʵ�����ļ�ʱÿ��һ����
	// const char* filename = "runlog0-3.1.log";
	// const char* filename = "test.log";
	// const char* filename = "test2.log";
	ios::sync_with_stdio(false);
	// bak
	pLogMaps = new LogMap[iThreadCount];

	// ˫����
	llMaxSize = llLoadSize + iLineBuffer;

	loadedFile[0] = new char[llMaxSize];
	loadedFile[1] = new char[llMaxSize];



	// ��ȡ�ļ�
	ifstream file;
	utilsError = clUtils.LoadFile(file);
	if (UTILS_RTMSG_OK != utilsError)
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

		cout << "==============-------------------------==============-------------------------==============" << endl;
		cout <<  "���ļ���С: " << llFileSize << endl; // debug

		// ԭ�е���־����Ҫ����
		char* strReq = (char*)"Req:";

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
			// file.seekg(-(iLineLen + 1), ios::end); // XXX ����+1��Ϊ���������з�\n,���ǵ�ֻ��һ��ʱ, ������û��\n��
			file.seekg(-(iLineLen), ios::end);
			llLastLinePos = file.tellg();
		}
		else
		{
			file.clear();
			file.seekg(0, ios::end);
			llLastLinePos = file.tellg();
			Sleep(iScanTime); // XXX WIN
		}

		file.clear();
		file.seekg(0, ios::end);
		llEndFilePos = file.tellg();
		llFileSize = llEndFilePos - llLastLinePos;
		llStart = llLastLinePos; // ��ʼ����λ�û��˵���ĩβһ�е�����

		cout << "==============-------------------------==============-------------------------==============" << endl;
		cout << "���һ��Ϊ: " << strLastLine << endl;
		cout << "==============-------------------------==============-------------------------==============" << endl;

		cout << "==============-------------------------==============-------------------------==============" << endl;
		cout << "������ļ���С: " << llFileSize << "\t������ʼλ��: " << llStart << "\t��������λ��: " << llEndFilePos << endl; // debug

		ParseLog(file, llFileSize, pCurrPos, strLoadSize, llMaxSize, llStart, iThreadCount, iAnsNum, iScanTime);

		// ����
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

	}
	return 0;
}


streamsize inline getRealSize(ifstream *file, streamoff llStart, streamsize llSize)
{
	char chCurr[1] = { 0 };

	file->clear();

	file->seekg(llStart + llSize);
	// while (file->get() != '\n')
	// �ļ�����̫���ʱ��, ����������ѭ��
	// FIXME creator.out ��ÿ2��3��������������ݵ�ʱ�������
	// while ((file->peek() != EOF) && ((file->get()!= '\n') || (file->get()!= '\0')))
	/*
	file->read(chCurr, 1);
	while (chCurr[0] != '\n' || chCurr[0] != '\0')
	{
		file->read(chCurr, 1);
		cout << chCurr[0];
		llSize++;
	}
	*/
	while (file->get()!= '\n')
	{
		llSize++;
	}

	return llSize;
}


streamsize inline getBlockSize(int iStep, streamoff llStart, streamsize llSize)
{
	char *p = loadedFile[iStep] + llStart + llSize;
	// while (*p != '\n')// ����\n����Ϊû��\nʱ������ѭ��, ���羲̬�ļ�ֻ��һ������, ���Լ���\0һ�����ж�

	while (*p != '\n' && *p != '\0')
	{
		++llSize;
		++p;
	}
	// tset begin
	llSize += 1; // ��������\n�ַ�
	// test end
	return llSize;
}


void inline readLoad(int iStep, ifstream *file, streamoff llStart, streamsize llSize)
{
	// XXX һ��Ҫclear
	// file->clear();
	file->seekg(llStart);
	// file->seekg(llStart);
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
	pLineBuffer = (char*)sLineBuffer.c_str();
	vecStringLine.clear();

	// XXX ���з�
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


void ParseMsgLine(vector<string> vecStr, int id, string strKey)
{
	string MsgId;
	LogMap *map = pLogMaps + id;

	// XXX glog�����̰߳�ȫ��?
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
				// û��MsgId�Ĵ�Ϊ��req/ans ������Ҫ���뵽map��
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
	cout << "\n==============-------------------------==============-------------------------==============" << endl;
	cout << "allLogMap.size(): " << allLogMap.size() << endl;
	for (; begin != end; )
	{
		if (mymap.count(begin->first) < iAnsNum)
		{
			time_t CurrTimeMs = clUtils.GetCurrentTimeMs();
			time_t MsgTimeMs = clUtils.StringToMs(begin->second);
			
			// ��ȡ����
			if ((utilsError = clUtils.GetConfigValue(strLbmTimeOut, "LbmTimeOut")) != UTILS_RTMSG_OK )
			{
				LOG(ERROR) << "��ȡ����ʧ��, ������: " << utilsError << endl;
				// TODO �쳣�׳�
				abort();
			}
			iLbmTimeOut = stoi(strLbmTimeOut);

			if ((CurrTimeMs - MsgTimeMs) > iLbmTimeOut)
			{
				// ans����������һ���ڴ��,���Գ�ʱ��ɾ
				cout << "��ʱ+����+ɾ��\t" << CurrTimeMs << "\t" << MsgTimeMs << "\t" << CurrTimeMs - MsgTimeMs << "\t" << begin->second << endl;

				strPostData = clUtils.AssembleJson(begin->second);
				cout << "\n==============-------------------------==============-------------------------==============" << endl;
				cout << "\n==============-------------------------==============" << "���͵�����Ϊ:   " << "==============-------------------------==============" << endl;
				cout << strPostData << endl;

				// ���Ͳ�ɾ��
				/*
				char* pMsgData = (char*)strPostData.c_str();
				// utilsError = clUtils.DoPost(pMsgData, strResponse);
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
					LOG(ERROR) << "\n==============-------------------------==============" << "ȱʧ������Ϊ: " << "==============-------------------------==============" << endl;
					LOG(INFO) <<  clUtils.GetMsgValue(begin->second, "MsgId") << endl; // debug
					LOG(INFO) << "\n==============-------------------------==============" << "��POST����ɹ�! " << "==============-------------------------==============" << endl;
					LOG(INFO) << "\n==============-------------------------==============" << "�յ��Ļظ�Ϊ:   " << "==============-------------------------==============" << endl;
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
			// TODO �����ҵ��˳�ʱ�Ĵ�����Ҫ�ҵ��б���Ĵ�ѽ

		}
		else
		{
			// ɾ��ͬʱ����req��ans��
			string key = begin->first;
			mymap.erase(begin->first);
			begin = mymap.begin();
			// end = mymap.end();
		}

	}
	cout << "ɾ����allLogMap.size():   " << allLogMap.size() << endl;
}


void ParseLog(ifstream& file, streamsize llFileSize, streampos pCurrPos, string strLoadSize, streamsize llMaxSize, streamoff llStart, int iThreadCount, int iAnsNum, int iScanTime)
{

	clock_t t_start, t_end;

	// test begin
	pCurrPos = llStart + llFileSize;
	// test end

	bool bBufferIndex = 0; // �����±�
	bool bNeedWait = false;
	streamoff llThreadIndex, llThreadPart;
	streamsize llRealSize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
	streamsize llFileLen;
	thread *threads = new thread[iThreadCount];

	while (llFileSize)
	{
		t_start = clock();

		// test begin
		// bak
		// �� llFileSize > llMaxSize�����Ѿ������˽�λ�Ļ��棬��������
		llRealSize = llFileSize > llMaxSize ? getRealSize(&file, llStart, stoll(strLoadSize)) : llFileSize;
		// llRealSize = llFileSize;
		// test end

		llThreadIndex = 0;

		// llThreadIndex = llFileSize - iLineLen - 1; // ��ȥ���һ������һ��֮���\n�ַ�
		llThreadPart = llRealSize / iThreadCount;

		cout << "\n\n==============-------------------------==============-------------------------==============" << endl;
		cout << "�������ݵ� loadedFile[bBufferIndex]: " << bBufferIndex << endl; // debug
		cout << "\n�ƻ�һ�ζ����С llLoadSize: " << stoll(strLoadSize) << "\tʵ��һ�ζ����С llRealSize: " << llRealSize << endl; // debug
		// cout << "\n���뿪ʼλ�� llStart: " << llStart << "\t���ļ�ʣ���ȡ��С llFileSize: " << llFileSize << endl;
		cout << "\n���뿪ʼλ�� llStart: " << llStart << "\t���ļ�ʣ���ȡ��С llFileSize: " << llFileSize << endl;
		cout << "\n�ƻ�ÿ���̶߳����С llThreadPart: " << llThreadPart ; // debug
		cout << "\n==============-------------------------==============-------------------------==============" << endl << endl;


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
				LogMapKeySet p = (pLogMaps + i)->begin();
				LogMapKeySet end = (pLogMaps + i)->end();
				for (; p != end; ++p)
				{
					allLogMap.insert(pair<string, string>(p->first, p->second));
				}
				// ������̵߳���map
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
		cout << "Scan Complete in : " << t_end - t_start << "ms." << endl; // debug



		cout << "\n==============-------------------------==============-------------------------==============" << endl;
		for (int i = 1; i < iThreadCount; ++i)
		{
			// test begin
			// llThreadIndex += 1; // ��һ���̶߳�ȡ�Ŀ�ʼλ��, ����\n�ַ�
			// test end

			/*
			if (llThreadIndex != 0)
			{
				llThreadIndex += 1; // ��һ���̶߳�ȡ�Ŀ�ʼλ��, ����\n�ַ�
			}
			*/

			if (llThreadIndex != llRealSize) // �����ļ�ֻ��һ��ʱgetBlockSize����
			{
				llFileLen = getBlockSize(bBufferIndex, llThreadIndex, llThreadPart);
			}

			// ��ʣ�����������߳���,����Ҫ�ж�һ���߳��Ѵ�����ַ��͸ÿ����ݵĴ�С
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

		// test begin
		// llThreadIndex += 1; // ��һ���̶߳�ȡ�Ŀ�ʼλ��, ����\n�ַ�
		// test end

		cout << "�߳� 4 ��ʼ����λ�� llThreadIndex: " << llThreadIndex << "\tʵ���߳� 4 �����С llRealSize-llThreadIndex: " << llRealSize - llThreadIndex << endl; // debug
		// XXX
		// һ�����󲻿�������70���ַ���
		if ((llRealSize - llThreadIndex) > 70)
		{
			vecEndThreadLines = ReadLineToVec(bBufferIndex, llThreadIndex, llRealSize - llThreadIndex);
			threads[0] = thread(ParseMsgLine, vecEndThreadLines, 0, "MsgId");
			cout << "�߳� 4 " << "������: " << vecEndThreadLines.size() << "��" << endl; // debug
		}

		bBufferIndex = !bBufferIndex; // �л� Buffer װ����
		cout << "\n==============-------------------------==============-------------------------==============" << endl;
		cout << "�ļ�ʣ���С: llFileSize: " << llFileSize << endl << endl << endl << endl; // debug

		// �ļ���������
		if (llFileSize != 0)
		{
			continue;
		}
		else
		{
			cout << "\n============== ----------------------- ============== ----------------------- ==============";
			cout << "\n============== ----------------------- ������һ���ݿ� ----------------------- ==============";
			cout << "\n============== ----------------------- ============== ----------------------- ==============" << endl;
			for (int i = 0; i < iThreadCount; ++i)
			{
				if (threads[i].joinable())
				{
					threads[i].join();
				}
			}

			Sleep(iScanTime); // XXX WIN
			
			file.clear();
			file.seekg(0, ios::end); // �ļ�ָ��ָ���ļ�ĩβ
			streampos pNewPos = file.tellg(); // �µ��ļ�ָ��

			// FIXME !!!!!!!
			// XXX ��Ҫ����λ�ж�,����ֻ��һ�е�һ����
			// begin
			/*
			int increSize = 0;
			char chCurr[1] = { 0 };
			file.read(chCurr, 1);
			while (chCurr[0] != '\n' && chCurr[0] != '\0')
			{
				file.read(chCurr, 1);
				cout << chCurr[0];
				increSize++;
			}
			if (increSize > 0)
			{
				abort();
			}
			// pNewPos += increSize;
			// end
			*/

			llFileSize = pNewPos - pCurrPos;

			cout << "\n==============-------------------------==============-------------------------==============" << endl;
			cout << "��ǰָ��λ��: " << pNewPos << "\t�ϴ�ָ��λ��: " << pCurrPos << "\t�����ļ���С: " << llFileSize << endl; // debug

			if (llFileSize < 0)
			{
				llFileSize = 0;
			}
			pCurrPos = pNewPos;
		}
	}

}

