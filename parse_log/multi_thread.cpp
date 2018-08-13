#include <iostream>
#include <fstream>
#include <istream>
#include <unordered_map>
#include <time.h>
#include <thread>
#include <cstring>
using namespace std;

struct CharCmp
{
	bool operator()(const char *str1, const char *str2) const
	{
		return strcmp(str1, str2) == 0;
	}
};

struct WordHash
{
	// BKDR hash algorithm
	int operator()(char *str) const
	{
		int seed = 131; // 31 131 1313 131313 etc..
		int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return hash & (0x7FFFFFFF);
	}
};

typedef unordered_map<char *, unsigned int, WordHash, CharCmp> HashMap;
typedef unordered_map<char *, unsigned int, WordHash, CharCmp>::iterator KeySet;

bool words[128]; // ascii表
int threadCount = 4;

// streamsize loadsize = 536870912;    // 1024*1024*1024  1879048192 1610612736 1073741824 536870912 268435456
streamsize loadsize = 700; // 一次加载的文件大小（比特）
char *loadedFile[2]; // 存放指向 char* 类型的指针的数组
HashMap *wordMaps;

// 声明
void readBlock(int, int, streamoff, streamsize);
streamsize inline getRealSize(ifstream *, streamoff, streamsize);
void inline readLoad(int, ifstream *, streamoff, streamsize);
streamsize inline getBlockSize(int, streamoff, streamsize);

int multi_thread()
{

	ios::sync_with_stdio(false);
	wordMaps = new HashMap[threadCount];
	char filename[] = "runlog0.log";
	// 双缓冲
	streamsize maxsize = loadsize + 256;
	loadedFile[0] = new char[maxsize];
	loadedFile[1] = new char[maxsize];

	time_t t_start, t_end;
	t_start = time(NULL);

	/* 初始化可识别字符, 给127个ascii字符维护一个bool值, 把需要的ascii码的bool设置为true */
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

	// 读取文件
	ifstream file;
	file.open(filename, ios::binary | ios::in);
	if (!file)
	{
		cout << "Error: file \"" << filename << "\" do not exist!" << endl;
		exit(1);
	}
	else
	{
		/* 确认文件大小 bytes */
		/*
		 * 这三个都是整形, streampos 当前流的位置、streamoff 流的偏移、streamsize 流的大小
		 * seekg seekp分别表示读指针和写指针的位置, seek get、seek put
		 */
		streamoff start = 0;
		streamoff size; // 文件大小
		file.seekg(0, ios::end); // seek get 对输入流操作, seekg(0,ios::end)设置streampos到文件末尾
		streamoff len = file.tellg(); // tellg: 返回当前 streampos, 常用来计算长度

		/* 通过前3个字符兼容带 bom 的 utf8 编码格式 */
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

		thread *threads = new thread[threadCount];
		streamoff index, part;
		streamsize realsize; // 实际读入大小(因为可能遇到需要的单词被截位)
		bool step = 0; // 缓存编号
		bool needWait = false;

		while (size)
		{
			realsize = size > maxsize ? getRealSize(&file, start, loadsize) : size;
			index = 0;
			part = realsize / threadCount;

			/* 读入 realsize 大小的文件数据到缓存 loadedFile[step] 中 */
			// TODO 按行读入数据到缓存
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

			// 多线程处理到map中
			for (int i = 1; i < threadCount; ++i)
			{
				len = getBlockSize(step, index, part);
				threads[i] = thread(readBlock, step, i, index, len);
				index += len;
			}

			// cout << "\npart: " << part << "\tindex :" << index << "\trealsize: " << realsize << "\trealsize - index: " << realsize - index << endl;
			// 留一个线程来读取多于 realsize - (part * (threadCound - 1)) 的数据
			// 为了防止单词被截断, 指针会出现偏移,所以用一个线程读取指针偏移后到末尾的数据
			threads[0] = thread(readBlock, step, 0, index, realsize - index);
			step = !step; // IO分离, 切换 Buffer 装数据
		}

		// 清理
		for (int i = 0; i < threadCount; ++i)
		{
			threads[i].join();
		}
		delete loadedFile[0];
		delete loadedFile[1];
		file.close();

	    // 结算累加
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
	}
	t_end = time(NULL);

	cout << "\r\nAll completed in " << difftime(t_end, t_start) << "s." << endl;
	return 0;
}

// 文件获取临界不截断的真正大小
// 从最大加载大小的位置, 往后读一个字符，遇到文件末尾返回空
streamsize inline getRealSize(ifstream *file, streamoff start, streamsize size)
{
	file->seekg(start + size);
	// get() 从流中读入一个字符, 如果到文件末尾, 则返回空字符,
	// 如果从指针位置往后有需要留下的字符，则加上
	while (words[file->get()])
	{
		++size;
	}
	return size;
}

// 单词截断检查,往后多读一点, 避免把一个单词拆开
streamsize inline getBlockSize(int step, streamoff start, streamsize size)
{
	char *p = loadedFile[step] + start + size;
	while (words[int(*p)])
	{
		++size;
		++p;
	}
	return size;
}

// 文件读入到堆
// void inline readLoad(int step, ifstream *file, streamoff start, streamsize size)
void inline readLoad(int step, ifstream *file, streamoff start, streamsize size)
{
	file->seekg(start);
	file->read(loadedFile[step], size);
}


// 分块读取
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
		c = filebuffer[index];
		if (c > 0 && words[int(c)])
		{
			word[i++] = c;
		}
		else if (i > 0)
		{
			word[i++] = '\0';
			// 先判断map中有没有
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
