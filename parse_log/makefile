# **********************************************************
# * Author       : M_Kepler
# * EMail        : m_kepler@foxmail.com
# * Last modified: 2018-08-30 15:43:36
# * Filename     : makefile
# * Description  :
# * 所有依赖: $^
# * 第一个依赖: $<
# * 目标文件: $@
# ***********************************************************


.PHONY: all

CURR_PWD = $(shell pwd)

PRJ_PATH = $(CURR_PWD)

CC = g++

C++STD = --std=c++11

PLATFORM = OS_IS_LINUX

GLOG_LIB_PATH = $(PRJ_PATH)/glog/unix/lib

DEBUG_FLAG = _DEBUG

# DEBUG_OPS = -Wall
DEBUG_OPS =

# -g -rdynamic

L_INCLUDE = -L$(GLOG_LIB_PATH) 

# **********************************************************
# Flags
# **********************************************************

# 头文件
I_INCLUDES = -I$(PRJ_PATH) -I$(PRJ_PATH)/glog/unix/include

# c编译器的选项
CFLAGS=

# cpp编译器的优化选项
CPPFLAGS= $(I_INCLUDES)

# 静态库名称
LIBS =-lglog

# DEBUG
DFLAGS = -D$(DEBUG_FLAG) -D$(PLATFORM) $(DEBUG_OPS) -DWITH_NONAMESPACES

# 库文件路径
LDFLAGS = $(L_INCLUDE)

# LFLAG = -shared $(L_INCLUDE)


all : main.out 


main.out : stdsoap2.o soapSoapServiceSoapBindingProxy.o soapC.o utils.o inifile.o log.o LbmRiskWarning.o parse_log.o
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS) -lpthread


parse_log.o : parse_log.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


LbmRiskWarning.o : LbmRiskWarning.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


stdsoap2.o : stdsoap2.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


soapC.o : soapC.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


soapSoapServiceSoapBindingProxy.o : soapSoapServiceSoapBindingProxy.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


utils.o : utils.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


log.o : log.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@ $(LDFLAGS) $(LIBS)


inifile.o : inifile.cpp
	$(CC) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@


clean :
	rm -f *.o *.out


