﻿/********************************************
function: thread safe blocking queue.
author: ypj
date: 2016.5.25
version: 1.0
********************************************/

#ifndef _BLOCK_QUEUE_H_
#define _BLOCK_QUEUE_H_

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
using namespace std;

template<class T>
class BlockQueue
{
public:
    BlockQueue(int max_size = 1000)
    {
        if(max_size <= 0)
        {
            cout <<"BlockQueue class max_size = " << max_size << endl;
            exit(-1);
        }
       
        m_max_size = max_size;
        m_array = new T[max_size];
        m_size = 0;
        m_front = -1;
        m_back = -1;

        m_mutex = new pthread_mutex_t;
        m_cond = new pthread_cond_t;
        pthread_mutex_init(m_mutex, NULL);
        pthread_cond_init(m_cond, NULL);
    }

    void clear()
    {
        pthread_mutex_lock(m_mutex);
        m_size = 0;
        m_front = -1;
        m_back = -1;
        pthread_mutex_unlock(m_mutex);
    }

    ~BlockQueue()
    {
        pthread_mutex_lock(m_mutex);
        if(m_array != NULL)
        {
            delete[]  m_array;
        }
        
        pthread_mutex_unlock(m_mutex);

        pthread_mutex_destroy(m_mutex);
        pthread_cond_destroy(m_cond);

        delete m_mutex;
        delete m_cond;
        
    }

    bool full()const
    {
        pthread_mutex_lock(m_mutex);
        if(m_size >= m_max_size)
        {
            pthread_mutex_unlock(m_mutex);
            return true;
        }
        pthread_mutex_unlock(m_mutex);
        return false;
    }

    bool empty()const
    {
        pthread_mutex_lock(m_mutex);
        if(0 == m_size)
        {
            pthread_mutex_unlock(m_mutex);
            return true;
        }
        pthread_mutex_unlock(m_mutex);
        return false;
    }
    
    bool front(T& value)const
    {
        pthread_mutex_lock(m_mutex);
        if(0 == m_size)
        {
            pthread_mutex_unlock(m_mutex);
            return false;
        }
        value = m_array[m_front];
        pthread_mutex_unlock(m_mutex);
        return true;
    }
    
    bool back(T& value)const
    {
        pthread_mutex_lock(m_mutex);
        if(0 == m_size)
        {
            pthread_mutex_unlock(m_mutex);
            return false;
        }
        value = m_array[m_back];
        pthread_mutex_unlock(m_mutex);
        return true;
    }

    int size()const
    {
        int tmp = 0;
        pthread_mutex_lock(m_mutex);
        tmp = m_size;
        pthread_mutex_unlock(m_mutex);
        return tmp;
    }

    int max_size()const
    {
        int tmp = 0;
        pthread_mutex_lock(m_mutex);
        tmp = m_max_size;
        pthread_mutex_unlock(m_mutex);
        return tmp;
    }

    bool push(const T& item)
    {
        pthread_mutex_lock(m_mutex);
        if(m_size >= m_max_size)
        {
            pthread_cond_broadcast(m_cond);
            pthread_mutex_unlock(m_mutex);
            return false;
        }
        
        m_back = (m_back + 1) % m_max_size;
        m_array[m_back] = item;

        m_size++;
        
        pthread_cond_broadcast(m_cond);
        pthread_mutex_unlock(m_mutex);

        return true;
    }
    

    bool pop(T& item)
    {
        pthread_mutex_lock(m_mutex);
        while(m_size <= 0)
        {
            if(0 != pthread_cond_wait(m_cond, m_mutex))
            {
                pthread_mutex_unlock(m_mutex);
                return false;
            }
            
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        pthread_mutex_unlock(m_mutex);
        return true;
    }

    bool pop(T& item, int ms_timeout)
    {
        struct timespec t = {0,0};
        struct timeval now = {0,0};
        gettimeofday(&now, NULL);
        
        pthread_mutex_lock(m_mutex);
        if(m_size <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout/1000;
            t.tv_nsec = (ms_timeout % 1000)*1000;
            if(0 != pthread_cond_timedwait(m_cond, m_mutex, &t))
            {
                pthread_mutex_unlock(m_mutex);
                return false;
            }
        }

        if(m_size <= 0)
        {
            pthread_mutex_unlock(m_mutex);
            return false;
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];m_size--;
        pthread_mutex_unlock(m_mutex);
        return true;
    }

private:
    pthread_mutex_t *m_mutex;
    pthread_cond_t *m_cond;
    T *m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;
};

#endif

