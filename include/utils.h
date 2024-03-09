//
// Created by hfzhou on 24-3-6.
//
#include "pthread.h"
#include <cstdio>
#include <pthread.h>
#include <cstring>
#include <ctime>

#ifndef RUDP_UTILS_H
#define RUDP_UTILS_H

enum LogLevel
{
    DEBUG,INFO,WARNING,ERROR
};

pthread_t CreateThread(void*(*func)(void*), void* args);
void MutexInit(pthread_mutex_t *mutex);
void MutexWait(pthread_mutex_t *mutex);
void MutexRelease(pthread_mutex_t *mutex);
void MutexDestroy(pthread_mutex_t *mutex);

void SetTerminalLogLevel(LogLevel level);
void SetFileLogLevel(LogLevel level,bool isClient);
void MyLog(LogLevel level,const char* str, ...);
void LogClose();

#endif //RUDP_UTILS_H
