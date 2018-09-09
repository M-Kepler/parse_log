

# ReadMe

## Ĭ�ϲ���˵��

* ���� StringToMs

```c++
iStart, iEnd����Ĭ�ϲ����ֱ��ʾʱ�����ַ����еĿ�ʼ�ͽ���λ��
time_t StringToMs(string strOrig, int iStart = 0, int iEnd = 19);
��ʱ���ʽΪ: formate = (char*)"%4d%2d%2d-%2d%2d%2d";
��:
u��runlog
20180202-090551-318351 18207    99 Req: LBM=L0106002,MsgI
20180202-090551-318 ʱ�俪ʼ����λ��Ϊ0 - 19
```

* ���� GetConfigValue

```
Ĭ��ȡ����[CONFIG]�ڵ��µ�������Ϣ
```

* bCheckData����windows��ͷ�ļ�





# �����б�


## FIXME

[ ] �ҵ��쳣Ӧ����ô������׳�?
[ ] �ļ�����д�롢multi_thread��Ҫ�ع���װ����
[ ] ��ʱɨ����Ż�

## TODO

[*] **��ʱ**ɨ�蹦��
[*] �ļ���ʵʱ����
	[��] ��������: һ�ζ�ȡ���ļ�β,Ȼ�������ڵ��߼�ȥ������һ��allmap��, Ȼ����ļ�βָ�뵱�ļ�ͷָ��, �ٻ�ȡһ���ļ�βָ��,�������ָ�벻һ��
		˵���ļ���������, �ٴΰ��������ļ��ŵ����ڵ��߼���ȥ����(�������ö���ʱ��, ��ʱʱ��һ���Ͱ�����ļ���ȥ����<����ȷʵû���������Ǹ�ʵʱ>)
	[*] �״ζ�ȡ���ļ�β, ����ans����
	[*] ans������cost�ֶ�
[*] MFC ǰ�˽���
[*] ����ÿ��һ���ļ���, Ҳû�д���
[ ] һ������ָ���е����
[ ] KCXP����־? �������ص���־?
[ ] HTTP


## XXX

[ ] ��� iThreadCount ����ɨ, ��Ϊ�����ʱ��ͽ����ж�



# ��������


## GLog

### ���밲װ

* [glog���غ�ʹ��](https://blog.csdn.net/Pig_Pig_Bang/article/details/81632962)

* [glog unix�±���ʹ��](https://blog.csdn.net/csm201314/article/details/75094527)

```
1. ��װ����
sudo apt-get install autoconf automake libtool

2. ���밲װ
����Դ���Ŀ¼��glog�ļ��У� 
./autogen.sh 
./configure --prefix=/path/to/install 
make -j 24 
sudo make install

3. ���û�������
�޸�$HOME/.bash_profile

# parse_log
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$HOME/parse/glog/unix/include
export LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/parse/glog/unix

```


* glog�ļ����Ǳ������ļ�,
��ǰ��debug�汾,�����ڹ�������������һ��
```
1. �Ҽ���Ŀ����
2. ��������-->C/C++-->����-->���Ӱ���Ŀ¼-->��� <glog��װ·��>/include
3. Releaseģʽ����: ��������-->C/C++-->��������-->���п�-->���߳�DLL(/MD)
> ��Ϊglog�Ŀ����������������, ����������ᵼ�´���
> 3'. Debugģʽ����: ��������-->C/C++-->��������-->���п�-->���̵߳��� DLL (/MDd)
4. ��������-->������-->����-->���ӿ�Ŀ¼-->���<glog��װ·��>/lib
5. ��������-->������-->����-->����������-->���glogd.lib�������ĳ�����Release�汾��дglog.lib��
```

## libcurl

### ���밲װ

* [Visual Studio(VS2017)���벢����C/C++-libcurl��������](https://blog.csdn.net/DaSo_CSDN/article/details/77587916)
  * �������������� `ENABLE_IDN=no`

* [unix�±��밲װlibcurl](https://blog.csdn.net/qianghaohao/article/details/51684862)

./configure --prefix=/path/to/install 

3. ���û�������
�޸�$HOME/.bash_profile
CPLUS_INCLUDE_PATH=/path/to/libcurl/include
LIBRARY_PATH=/path/to/libcurl/lib

```

* libcurl �ļ����Ǳ������ļ�,
```
1. �Ҽ���Ŀ����
2. �������� --> C/C++ --> ����-->���Ӱ���Ŀ¼ --> ��� <curl·��>/include
3. Releaseģʽ����: �������� --> C/C++ --> �������� --> ���п� --> ���߳�DLL(/MD)
4. �������� --> ������ --> ���� --> ���ӿ�Ŀ¼ --> ��� <curl��װ·��>/lib
5. �������� --> ������ --> ���� --> ���������� --> ��� libcurl_a_debug.lib;Ws2_32.lib;Wldap32.lib;winmm.lib;Crypt32.lib
   �����ĳ�����Release�汾��д libcurl_a.lib
6. �������� --> C/C++ --> Ԥ������ --> Ԥ���������� --> �༭�������: CURL_STATICLIB 




## Q&A

1. ������һ�������ܺ�����д��, ���Ǻ���������������, Ҫ����oop�ķ��������

2. �������ʱ��, �漰������������; ����glog��ʹ��֮ǰ��Ҫ��ʼ��, ʹ�ý���֮����Ҫ�ͷ�...
   �������ܷ�װ����������, �����һ��ȫ�ֱ���, �����Ͳ���Ҫÿ��ʹ��֮ǰ������һ�γ�ʼ��
   2.1 ��һ���뷨��ʹ��ȫ�ֱ���; �ѳ�ʼ�����ͷ�д����CGlog��, Ȼ�󹹽�һ��ȫ�ֵĶ���, �����������ļ���externһ�¾�����
	   ����, ����ֶ��init��glog, ����û��ȫ��shutdown��������
   2.2 �ڶ����뷨��ʹ����ľ�̬��Ա;
       ����, ����Ҫlog�ĵط����ܲ���Ҫutils��, ����ÿ�ζ���utils����þ�̬��Ա����Ҳûʲô��˼, ����������д�������ļ�
   2.3 ��glog�ĳ�ʼ�����ͷ�д������cpp�ļ���, ����Ҫ��¼��־�ĵط�����log.h, ֱ�ӵ��ó�ʼ��, ���ļ�β�ٵ���shutdown
	   ����, ����Ҳ�Ǻ������, ������n���ļ���Ҫ��¼��־, ����Ҫ��ʼ��n��....�����һ��ȫ�ֱ���ͨ��
   2.4 ʹ�õ���ģʽ

3. ��λ���ļ����һ��,�ж���req����ans�Ŀ�
   ԭ���߼�:
   ��getline�����ļ�,ѭ��������õ��ľ������һ����
   �����req��, ���ָ���Ƶ����һ�е���β, Ȼ���ٻ�ȡһ�´˿̵�ָ����
   ```
   		while (file.peek() != EOF)
		{
			getline(file, strLastLine);
		}
		iLineLen = strLastLine.length();

		if (strstr(strLastLine.c_str(), strReq) != NULL)
		{
			file.clear();
			// file.seekg(-(iLineLen + 1), ios::end); // XXX ����+1��Ϊ���������з�\n,���ǵ�ֻ��һ��ʱ, ������û��\n��
			file.seekg(-(iLineLen), ios::end);
			llLastLinePos = file.tellg();
			if (llLastLinePos < 0)
			{
				llLastLinePos = 0;
			}
			file.clear();
			file.seekg(0, ios::end);
			llEndFilePos = file.tellg();
			llFileSize = llEndFilePos - llLastLinePos;
			llStart = llLastLinePos; // ��ʼ����λ�û��˵���ĩβһ�е�����
		}

   ```