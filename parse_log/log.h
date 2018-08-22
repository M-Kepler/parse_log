#pragma once
#ifndef __LOG_H_
#define __LOG_H_

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <glog/logging.h>
#include <string>
#include "inifile.h"
using namespace std;
using namespace inifile;

char * GetConfigPath();
int InitGlog(char *argv[]);
void CloseGlog();



#undef GOOGLE_GLOG_DLL_DECL
#undef GLOG_NO_ABBREVIATED_SEVERITIES

#endif
