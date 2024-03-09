//
// Created by hfzhou on 24-3-6.
//

#include "utils.h"
#include <stdarg.h>

LogLevel fileLogLevel,terminalLogLevel;
FILE* LogFp;

char TITLES[4][20]={"\033[36m[DEBUG]\033[37m","\033[32m[INFO]\033[37m",
                    "\033[33m[WARN]\033[37m","\033[31m[ERROR]\033[37m"};

pthread_t CreateThread(void*(*func)(void*), void* args)
{
    pthread_t pt;
    int res = pthread_create(&pt, nullptr,func,args);
    if(res!=0)
    {
        printf("Create thread error! return value=%d\n",res);
        return 0;
    }

//    res = pthread_detach(pt);
//    if(res!=0)
//    {
//        printf("Detach thread error! return value=%d\n",res);
//        return -1;
//    }

    return pt;
}

void MutexInit(pthread_mutex_t *mutex)
{
    pthread_mutex_init(mutex, nullptr);
}

void MutexWait(pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
}

void MutexRelease(pthread_mutex_t *mutex)
{
    pthread_mutex_unlock(mutex);
}

void MutexDestroy(pthread_mutex_t *mutex)
{
    pthread_mutex_destroy(mutex);
}

void SetTerminalLogLevel(LogLevel level)
{
    terminalLogLevel = level;
}
void SetFileLogLevel(LogLevel level,bool isClient=true)
{
    fileLogLevel = level;
    char filename[50];
    char my_date[20];
    time_t cur;
    time(&cur);
    strftime(my_date,sizeof(my_date),"%m月%d日 %H:%M:%S", localtime(&cur));
    sprintf(filename,"../Logs/%s/%s.log",isClient?"Client":"Server",my_date);
    LogFp = fopen(filename,"w+");
}
void MyLog(LogLevel level,const char* str, ...)
{
    char fmt[50];
    char head[20];
    time_t cur;
    time(&cur);
    strftime(head,sizeof(head),"%H:%M:%S", localtime(&cur));

    va_list args;
    va_start(args, str);
    vsprintf(fmt,str,args);
    va_end(args);

    char term[50],file[50];

    sprintf(term,"%s %s%s\n",head,TITLES[level],fmt);
    sprintf(file,"%s %s\n",head,fmt);
    if(level>=terminalLogLevel)
    {
        printf("%s",term);
    }
    if(level>=fileLogLevel)
    {
        fwrite(file, strlen(file),1,LogFp);
    }
}

void LogClose()
{
    fclose(LogFp);
}
