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

# **********************************************************
# 宏/变量
# **********************************************************

TARGET = LbmRiskWarning.out

SOURCES = $(wildcard *.c *.cpp)

OBJS = $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SOURCES)))

CURR_PWD = $(shell pwd)

PRJ_PATH = $(CURR_PWD)


CC = gcc

XX = g++

C++STD = --std=c++11

PLATFORM = OS_IS_LINUX

GLOG_LIB_PATH = $(PRJ_PATH)/glog/unix/lib

DEBUG_FLAG = _DEBUG

# DEBUG_OPS = -Wall -g
DEBUG_OPS = -g

# -g -rdynamic

L_INCLUDE = -L$(GLOG_LIB_PATH) 


# **********************************************************
# 选项
# **********************************************************

# 头文件
I_INCLUDES = -I$(PRJ_PATH) -I$(PRJ_PATH)/glog/unix/include

# c编译器的选项
CFLAGS=

# cpp编译器的优化选项
CPPFLAGS= -no-pie $(I_INCLUDES)

# 静态库名称
LIBS =-lglog

# DEBUG
DFLAGS = -D$(DEBUG_FLAG) -D$(PLATFORM) $(DEBUG_OPS) -DWITH_NONAMESPACES

# 库文件路径
LDFLAGS = $(L_INCLUDE)

# LFLAG = -shared $(L_INCLUDE)


# **********************************************************
# 编译
# **********************************************************

all : $(TARGET)


$(TARGET):$(OBJS)
	$(XX) $(C++STD) $(CPPFLAGS) $(DFLAGS) $^ -o $@ -fPIC $(LDFLAGS) $(LIBS) -lpthread


%.o : %.cpp
	$(XX) $(C++STD) $(CPPFLAGS) $(DFLAGS) -c $^ -o $@ $(LDFLAGS) $(LIBS)



clean :
	rm -f *.o *.out


