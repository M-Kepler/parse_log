#include "log.h"

int InitGlog(char* argv[])
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
	clIniFile.getValue(Section, Key_Log_Lvl, Value_Log_Lvl);

	if ((iRetCode = clIniFile.getValue(Section, Key_Log_Lvl, Value_Log_Lvl)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Path, Value_Log_Path)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_LogFile_Max_Size, Value_LogFile_Max_Size)) != RET_OK
		|| (iRetCode = clIniFile.getValue(Section, Key_Log_Buf_Secs, Value_Log_Buf_Secs)) != RET_OK
		)
	{
		return RET_ERR;
	}

	// FIXME
	// 全局初始化glog
	google::SetLogFilenameExtension(".log");
	FLAGS_logbufsecs = stoi(Value_Log_Buf_Secs);
	FLAGS_max_log_size = stoi(Value_LogFile_Max_Size);
	google::InitGoogleLogging(argv[0]);

	// 设置INFO/WARNING/ERROR级别以上的信息log文件的路径和前缀名
	google::SetLogDestination(google::GLOG_INFO, (Value_Log_Path + "\\INFO").c_str());
	google::SetLogDestination(google::GLOG_WARNING, (Value_Log_Path + "\\WARNING").c_str());
	google::SetLogDestination(google::GLOG_ERROR, (Value_Log_Path + "\\ERROR").c_str());
	google::SetLogDestination(google::GLOG_FATAL, (Value_Log_Path + "\\FATAL").c_str());


	// 设置glog的输出级别，这里的含义是输出INFO级别以上的信息
	google::SetStderrLogging(google::GLOG_INFO);

	// FLAGS_colorlogtostderr = true;                     // 开启终端颜色区分
	// 当磁盘被写满时，停止日志输出
	FLAGS_stop_logging_if_full_disk = true;

	return RET_OK;
}


char * GetConfigPath()
{
	char* ConfigPath = (char*)"./runlog_config.ini";
	return ConfigPath;
}


void CloseGlog()
{
	google::ShutdownGoogleLogging();
}
