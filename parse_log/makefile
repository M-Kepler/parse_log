# /*
# **********************************************************
# * Author       : M_Kepler
# * EMail        : m_kepler@foxmail.com
# * Last modified: 2018-08-30 15:43:36
# * Filename     : makefile
# * Description  :
# ***********************************************************
# */

.PHONY: all parse_log thread multi_thread clean

PARSE_PATH = $(HOME)/parse

CURR_PWD = $(shell pwd)


PLATFORM = OS_IS_LINUX

ifeq ($(PLATFORM), OS_IS_LINUX)
	GLOG_LIB_PATH = $(HOME)/parse/glog/unix/lib
	CURL_LIB_PATH = $(HOME)/parse/curl/unix/lib
endif

DEBUG_FLAG = _DEBUG

DEBUG_OPS = -g -rdynamic

# 头文件
I_INCLUDE = -I$(PARSE_PATH)

# 指定lib库的路径
L_INCLUDE = -L$(GLOG_LIB_PATH) -L$(CURL_LIB_PATH)

CFLAS = -D$(DEBUG_FLAG) -D$(PLATFORM) -DWITH_NONAMESPACES $(I_INCLUDE) $(DEBUG_OPS)

LFLAG = -shared $(L_INCLUDE)


all: parse_log multi_thread.out

parse_log: parse_log.cpp log.cpp utils.cpp inifile.cpp multi_thread.cpp
	g++ --std=c++11 $(CFLAS) -pthread log.cpp utils.cpp inifile.cpp multi_thread.cpp parse_log.cpp soapC.cpp stdsoap2.cpp soapSoapServiceSoapBindingProxy.cpp -lglog -lcurl -o parse_log


multi_thread: multi_thread.out
multi_thread.out: multi_thread.cpp
	g++ --std=c++11 -pthread $^ -o $@


# make install
# install:
	# cp slog.h ${INC_PATH}
	# cp libslog.a ${LIB_PATH}

# uninstall:
	# rm ${INC_PATH}/slog.h
	# rm ${LIB_PATH}/libslog.a

clean:
	rm -f *.o *.out

