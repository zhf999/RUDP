//
// Created by hfzhou on 24-3-6.
//
#include "pthread.h"
#include <cstdio>
#include <pthread.h>

#ifndef RUDP_UTILS_H
#define RUDP_UTILS_H

pthread_t CreateThread(void*(*func)(void*), void* args);
void MutexInit(pthread_mutex_t *mutex);
void MutexWait(pthread_mutex_t *mutex);
void MutexRelease(pthread_mutex_t *mutex);
void MutexDestroy(pthread_mutex_t *mutex);

#endif //RUDP_UTILS_H
