
#include "multi_thread.h"
#include "utils.h"

bool words[128]; // ascii��
int threadCount = 4;

streamsize loadsize = 8000; // XXX ���� 
char *loadedFile[2]; // ���ָ�� char* ���͵�ָ�������
HashMap *wordMaps;


int multi_thread()
{

	ios::sync_with_stdio(false);
	wordMaps = new HashMap[threadCount];
	const char* filename = "runlog0-3.1.log";

	// ˫����
	streamsize maxsize = loadsize + 256;
	loadedFile[0] = new char[maxsize];
	loadedFile[1] = new char[maxsize];

	time_t t_start, t_end;
	t_start = time(NULL);

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

			/* ���� realsize ��С���ļ����ݵ����� loadedFile[step] �� */
			readLoad(step, &file, start, realsize);



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

			// ���̴߳���map��
			for (int i = 1; i < threadCount; ++i)
			{
				len = getBlockSize(step, index, part);
				// ����vector����
				vector<string>sevc = ReadLineToVec(step, start, len, i);
				cout << "������: " << sevc.size() << "��" << endl;
				// XXX ��ȡ�ֿ��С(����)
				// cout << "step: " << step << "\tindex: " << index << "\tlen: " << len << "\tpart: " << part << endl;
				// threads[i] = thread(readBlock, step, i, index, len);
				threads[i] = thread(ParseMsgLine, sevc, "MsgId" );
				index += len;
			}
			// threads[0] = thread(readBlock, step, 0, index, realsize - index);
			vector<string>sevc0 = ReadLineToVec(step, index, realsize - index);
			threads[0] = thread(ParseMsgLine, sevc0, "MsgId");

			start += realsize;
			size -= realsize;

			step = !step; // �л� Buffer װ����
		}

		// ����
		for (int i = 0; i < threadCount; ++i)
		{
			threads[i].join();
		}
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();



		// �����ۼ�
		/*
		HashMap *map = wordMaps;
		for (int i = 1; i < threadCount; ++i)
		{
			KeySet p = (wordMaps + i)->begin(), end = (wordMaps + i)->end();
			for (; p != end; ++p)
				(*map)[p->first] += p->second;
		}
		cout << "Done.\r\n\nDifferent words: " << map->size() << endl;
		KeySet p = map->begin();
		KeySet end = map->end();
		long total = 0;
		for (; p != end; ++p)
		{
			total += p->second;
		}
		cout << "Total words:" << total << endl;
		cout << "\nEach words count:" << endl;
		for (KeySet i = map->begin(); i != map->end(); ++i)
		{
			cout << i->first << "\t= " << i->second << endl;
		}
		*/
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
	char *pFileBuffer;
	char *pLineBuffer;
	streamsize llBuffSize;
	string sLineBuffer;
	// char *lineBuffer = NULL;
	string sFileBuffer;
	string sLine;
	vector<string> vStringLine;

	pFileBuffer = loadedFile[step];
	sFileBuffer = pFileBuffer;
	llBuffSize = start + size + 1; // ��+1�ᶪʧ���һ���ַ�
	sLineBuffer = sFileBuffer.substr(start, llBuffSize);
	pLineBuffer = (char*)sLineBuffer.data();
	vStringLine.clear();

	// FIXME
	// �зָ��\r\n \n ; windowsϵͳ��unixϵͳ�ķָ����һ��; ����ͨ�������ļ����Զ�֪��, Ҳ����ͳһ��\nΪ�ָ���, �����ʱ�����\r 
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
	return vStringLine;
}


void ParseMsgLine(vector<string> sevc, string KeyStr)
{
	string MsgId;
	unordered_multimap<string, string> mymap;
	unordered_multimap<string, string>::iterator curr;

	auto end = mymap.end();
	for (string::size_type i = 0; i < sevc.size(); ++i)
	{
		MsgId = GetMsgValue(sevc[i], KeyStr);
		mymap.insert(pair<string, string>(MsgId, sevc[i]));
	}
	auto begin = mymap.begin();

	// debug
	//  ��������map, ��ͰԪ������С��2�ģ���Ϊȷʵreq��ans
	for (; begin != mymap.end(); begin++)
	{
		if (mymap.count(begin->first) < 2)
		{
			cout << "\nȱʧӦ�𴮵�MsgId�� \t" << GetMsgValue(begin->second, "MsgId") << endl << endl;
			cout << endl << begin->second << endl << endl;
		}
	}
}


void readBlock(int step, int id, streamoff start, streamsize size)
{
	char c = '\0';
	char word[128];
	int i = 0;
	HashMap *map = wordMaps + id;
	KeySet curr, end = map->end();
	char *filebuffer = loadedFile[step];
	streamsize bfSize = start + size;
	for (streamoff index = start; index != bfSize; ++index)
	{
		// FIXME �����ܻ����Խ��
		c = filebuffer[index];
		if (c > 0 && words[int(c)])
		{
			word[i++] = c;
		}
		else if (i > 0)
		{
			word[i++] = '\0';
			// ���ж�map����û��
			if ((curr = map->find(word)) == end)
			{
				char *str = new char[i];
				memcpy(str, word, i);
				map->insert(pair<char *, unsigned int>(str, 1));
			}
			else
			{
				++(curr->second);
			}
			i = 0;
		}
	}

	if (i > 0)
	{
		word[i++] = '\0';
		if ((curr = map->find(word)) == end)
		{
			char *str = new char[i];
			memcpy(str, word, i);
			map->insert(pair<char *, unsigned int>(str, 1));
		}
		else
		{
			++(curr->second);
		}
	}
}

