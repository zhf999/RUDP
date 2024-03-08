//
// Created by hfzhou on 24-3-6.
//

#include "utils.h"


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