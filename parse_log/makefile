# /*
# **********************************************************
# * Author       : M_Kepler
# * EMail        : m_kepler@foxmail.com
# * Last modified: 2018-08-30 15:43:36
# * Filename     : makefile
# * Description  :
# ***********************************************************
# */

# 平台
PLATFORM = OS_IS_LINUX

# Debug模式
DEBUG_FLAG = _DEBUG

# INC_PATH = /usr/local/include
# LIB_PATH = /usr/local/lib

.PHONY: all parse_log thread multi_thread clean

all: parse_log.out multi_thread.out

parse_log: parse_log.cpp log.cpp mylibcurl.cpp utils.cpp inifile.cpp multi_thread.cpp
	g++ --std=c++11 -pthread log.cpp mylibcurl.cpp utils.cpp inifile.cpp multi_thread.cpp parse_log.cpp -lglog -lcurl -o parse_log.out


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

