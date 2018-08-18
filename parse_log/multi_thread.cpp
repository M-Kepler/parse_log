
#include "multi_thread.h"
#include "utils.h"

bool words[128]; // ascii��
int threadCount = 4;

streamsize loadsize = 8000; // XXX ���� 
char *loadedFile[2]; // ���ָ�� char* ���͵�ָ�������
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

	// ˫����
	streamsize maxsize = loadsize + 512; // XXX ���� delete loadedFile[1] ���������������,���512��Ϊ�˷�ֹ�׶����Զ����ó�����һ�οռ�
	loadedFile[0] = new char[maxsize];
	loadedFile[1] = new char[maxsize];

	time_t t_start, t_end;
	t_start = time(NULL);

	/*
	memset(words, false, 128);
	// ascii��97��a, 122��z
	for (char c = 97; c != 123; ++c)
	{
		words[int(c)] = true;
	}
	// ascii��65��A, 90��Z
	for (char c = 65; c != 91; ++c)
	{
		words[int(c)] = true;
	}
	// ascii��48��0, 57��9
	for (char c = 48; c != 58; ++c)
	{
		words[int(c)] = true;
	}
	*/

	// ��ȡ�ļ�
	ifstream file;
	file.open(filename, ios::binary | ios::in);

	if (!file)
	{
		cout << "Error: File \"" << filename << "\" do not exist!" << endl;
		exit(1);
	}
	else
	{
		/* ȷ���ļ���С bytes */
		streamoff start = 0;
		streamoff size; // �ļ���С
		file.seekg(0, ios::end);
		streamoff len = file.tellg();

		/* ���ݴ� bom �� utf8 �����ʽ */
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
		cout << "�ļ���С" << size  << endl; // 123789

		thread *threads = new thread[threadCount];
		streamoff index, part;
		streamsize realsize; // ʵ�ʶ����С(��Ϊ����������Ҫ�ĵ��ʱ���λ)
		bool step = 0; // �����±�
		bool needWait = false;

		while (size)
		{
			realsize = size > maxsize ? getRealSize(&file, start, loadsize) : size;
			index = 0;
			part = realsize / threadCount;

			cout << "\n\n\n�������ݵ�loadedFile[step]: " << step << endl;
			cout << "�ƻ�һ�ζ����Сloadedsize: " << loadsize << "\tʵ��һ�ζ����Сrealsize: " << realsize << endl;
			cout << "���뿪ʼλ��:start: " << start << "\t���ļ�ʣ���ȡ��С: size: " << size << "\t\t�ƻ�ÿ���̶߳����Сpart: " << part << endl << endl << endl;


			/* ���� realsize ��С���ļ����ݵ����� loadedFile[step] �� */
			readLoad(step, &file, start, realsize);
			start += realsize;
			size -= realsize;


			/* �������߳�,�ȴ���һ�����ݿ��������,�ٶ���һ���ݿ���з���*/
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
					index += 1; // ��һ���̶߳�ȡ�Ŀ�ʼλ������\n�ַ�
				}

				len = getBlockSize(step, index, part);
				// TODO
				// ���ֻ������, ��������? ����Ҫ�ж�һ���߳��Ѵ�����ַ�����һ�����ݵĴ�С
				if (len + index < realsize)
				{
					cout << "�߳� " << i << " ��ʼ����λ��index: " << index << "\tʵ���߳� " << i << " �����Сlen:" << len << endl;
					sevc = ReadLineToVec(step, index, len, i);
					cout << "�߳� " << i << " ������: " << sevc.size() << "��" << endl << endl << endl;
					threads[i] = thread(ParseMsgLine, sevc, "MsgId");
					index += len;
				}
				else
				{
					break;
				}
			}

			cout << "�߳� 4 ��ʼ����λ�� index: " << index << "\t; 4 �̶߳����С real-index: " << realsize - index << endl;
			if ((realsize - index) > 70)
			{
				sevc0 = ReadLineToVec(step, index, realsize - index);
				threads[0] = thread(ParseMsgLine, sevc0, "MsgId");
				cout << "�߳�: 4 " << " ������: " << sevc0.size() << "��" << endl;
			}

			step = !step; // �л� Buffer װ����
			cout << "�ļ�ʣ���С: size: " << size << endl << endl << endl << endl;
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
	// �������ļ���ȡ
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
	// �зָ��\r\n \n ; windowsϵͳ��unixϵͳ�ķָ����һ��; ����ͨ�������ļ����Զ�֪��, Ҳ����ͳһ��\nΪ�ָ���, �����ʱ�����\r 
	// ������������,ÿ��vectorԪ��ĩβ����һ��'\r'
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

	// TODO �����ٱ�������map, ��ͰԪ������С��2�ģ���Ϊȷʵreq��ans
	/*
	auto begin = mymap.begin();
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\nȱʧӦ�𴮵�MsgId�� \t" << GetMsgValue(begin->second, "MsgId") << endl << endl;
			cout << endl << begin->second << endl << endl;
		}
	}
	*/
}

