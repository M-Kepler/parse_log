#include "log.h"


int CGlog::InitGlog()
{
	IniFile clIniFile;
	int iRetCode;
	char szRetMsg[256 + 1];
	memset(szRetMsg, 0x00, sizeof(szRetMsg));
	string Section;
	string Key_Log_Lvl;
	string Key_Log_Path;
	string Key_LogFile_Max_Size;
	string Key_Log_Buf_Secs;
	string Value_Log_Lvl;
	string Value_Log_Path;
	string Value_LogFile_Max_Size;
	string Value_Log_Buf_Secs;

	Key_Log_Lvl = "Log_Lvl";
	Key_LogFile_Max_Size = "LogFile_Max_Size";
	Key_Log_Path = "Log_Path";
	Key_Log_Buf_Secs = "Log_Buf_Secs";
	Section = "GLOG";

	CUtils clUtils;
	UtilsError utilsError;
	// ��ȡ����
	if ((utilsError = clUtils.GetConfigValue(Value_Log_Lvl, Key_Log_Lvl, Section)) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(Value_Log_Path, Key_Log_Path, Section)) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(Value_LogFile_Max_Size, Key_LogFile_Max_Size, Section)) != UTILS_RTMSG_OK
		|| (utilsError = clUtils.GetConfigValue(Value_Log_Buf_Secs, Key_Log_Buf_Secs, Section)) != UTILS_RTMSG_OK
		)
	{
		LOG(ERROR) << "��ȡ����ʧ��, ������: " << utilsError << endl;
		// TODO �쳣�׳�
		abort();
		return utilsError;
	}

	// ����log·��
	SetLogDir((char*)Value_Log_Path.c_str());
#ifdef OS_IS_LINUX
	DIR *pDirExists = NULL;
	pDirExists = opendir(LogDir);
	if (!pDirExists && errno == ENOENT)
	{
		if (mkdir(LogDir, 0750) != 0)
		{
			LOG(ERROR) << "logĿ¼����ʧ��" << endl;
		}
	}
	if (pDirExists)
	{
		closedir(pDirExists);
	}
#else
	WIN32_FIND_DATA stFindData;
	HANDLE hFind = FindFirstFile(LogDir, &stFindData);
	if ((INVALID_HANDLE_VALUE == hFind) || !(stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		if (!CreateDirectory(LogDir, NULL))
		{
			snprintf(szRetMsg, sizeof(szRetMsg) - 1, "����Ŀ¼ʧ��! [%s]", LogDir);
			LOG(ERROR) << szRetMsg << endl;
		}
	}
	if (INVALID_HANDLE_VALUE != hFind)
	{
		FindClose(hFind);
	}
#endif


	/*
	if (!DirExist())
	{
		LOG(WARNING) << "logĿ¼������" << endl;
	}
	*/

	google::SetLogFilenameExtension(".log");
	FLAGS_logbufsecs = stoi(Value_Log_Buf_Secs);
	FLAGS_max_log_size = stoi(Value_LogFile_Max_Size);
	google::InitGoogleLogging("parse_runlog");

	// ����INFO/WARNING/ERROR�������ϵ���Ϣlog�ļ���·����ǰ׺��
	google::SetLogDestination(google::GLOG_INFO, (Value_Log_Path + PATHSPLIT + "INFO").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (Value_Log_Path + PATHSPLIT + "WARNING").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (Value_Log_Path + PATHSPLIT + "ERROR").c_str());
	google::SetLogDestination(google::GLOG_FATAL, (Value_Log_Path + PATHSPLIT + "FATAL").c_str());


	// ����glog�������������ĺ��������INFO�������ϵ���Ϣ
	google::SetStderrLogging(stoi(Value_Log_Lvl));

	// �����̱�д��ʱ��ֹͣ��־���
	FLAGS_stop_logging_if_full_disk = true;

	return RET_OK;
}


void CGlog::SetLogDir(char* logpath)
{
	LogDir = logpath;
}

bool CGlog::DirExist()
{
	int dir_exist = access(LogDir, 0);
	if (0 != dir_exist)
	{
		return false;
	}
	else
	{
		return true;
	}
}


void CGlog::CloseGlog()
{
	google::ShutdownGoogleLogging();
}

