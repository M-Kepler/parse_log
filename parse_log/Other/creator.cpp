/*
 **********************************************************
 * Author       : M_Kepler
 * EMail        : m_kepler@foxmail.com
 * Last modified: 2018-08-20 14:06:23
 * Filename     : creator.cpp
 * Description  :
 ***********************************************************
 */

#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
using namespace std;

int main(int argc, const char *argv[]) 
{ 
    FILE *file; 
    struct tm *t1; 
    time_t t; 
    char buf[100]; 
    int line = 1; 
    int c; 

    memset(buf, 0, sizeof(buf)); 
    if(argc !=2 )
    {
        printf("请输入待处理的文件. e.g.: ./main file\n");
        return -1;
    }

    if ((file = fopen(argv[1], "a+")) < 0) 
    {
        perror("failed to open test.txt"); 

        exit(-1); 
    } 

    while ((c = getc(file)) != EOF) //计算行数，用于下次打开时能够衔接上之前的行数 
        if (c == '\n') 
            line++; 

    while (1) 
    { 
        time(&t); 
        t1 = localtime(&t); //获取当前时间
        sprintf(buf, "%d, %d-%d-%d %d:%d:%d\n", line++, t1->tm_year + 1900, t1->tm_mon + 1, t1->tm_mday, t1->tm_hour, t1->tm_min, t1->tm_sec); 
        fwrite(buf, sizeof(char), strlen(buf), file); 
        fflush(file); 
        // sleep(1); 
        usleep(100); 
    } 

    return 0; 
}
