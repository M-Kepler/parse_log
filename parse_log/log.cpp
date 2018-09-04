#include "log.h"


int CGlog::InitGlog()
{
	IniFile clIniFile;
	int iRetCode;
	string Section;
	string Key_Log_Lvl;
	string Key_Log_Path;
	string Key_LogFile_Max_Size;
	string Key_Log_Buf_Secs;
	string Value_Log_Lvl;
	string Value_Log_Path;
	string Value_LogFile_Max_Size;
	string Value_Log_Buf_Secs;
	char* ConfigPath;

	ConfigPath = GetConfigPath();
	Key_Log_Lvl = "Log_Lvl";
	Key_LogFile_Max_Size = "LogFile_Max_Size";
	Key_Log_Path = "Log_Path";
	Key_Log_Buf_Secs = "Log_Buf_Secs";
	Section = "GLOG";

	clIniFile.load(ConfigPath);

	if ((iRetCode = clIniFile.getValue(Section, Key_Log_Lvl, Value_Log_Lvl)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Path, Value_Log_Path)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_LogFile_Max_Size, Value_LogFile_Max_Size)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Buf_Secs, Value_Log_Buf_Secs)) != RET_OK
		)
	{
		return RET_ERR;
	}

	// ����log·��
	SetLogDir((char*)Value_Log_Path.c_str());

	if (!DirExist())
	{
		// XXX
		// WARNING ��Log�ļ��в����ڵľ���
		cout << "logĿ¼������" << endl;
	}

	google::SetLogFilenameExtension(".log");
	FLAGS_logbufsecs = stoi(Value_Log_Buf_Secs);
	FLAGS_max_log_size = stoi(Value_LogFile_Max_Size);
	google::InitGoogleLogging("parse_runlog");

	// ����INFO/WARNING/ERROR�������ϵ���Ϣlog�ļ���·����ǰ׺��
	google::SetLogDestination(google::GLOG_INFO, (Value_Log_Path + "\\INFO").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (Value_Log_Path + "\\WARNING").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (Value_Log_Path + "\\ERROR").c_str());
	google::SetLogDestination(google::GLOG_FATAL, (Value_Log_Path + "\\FATAL").c_str());


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


char* CGlog::GetConfigPath()
{
	return ConfigPath;
}


void CGlog::CloseGlog()
{
	google::ShutdownGoogleLogging();
}

