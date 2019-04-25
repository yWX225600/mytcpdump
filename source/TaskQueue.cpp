#include "TaskQueue.h"

CTaskQueue* CTaskQueue::m_taskQueue = 0;
CTaskQueue::CTaskQueue(){
    pthread_mutex_init(&m_mutexProducer,0);
    pthread_mutex_init(&m_mutexConsumer,0);
    
    sem_init(&m_semProducer,0,TASK_QUEUE_LEN);
    sem_init(&m_semConsumer,0,0);
    
    m_cacheHead = (unsigned long*)malloc(TASK_QUEUE_LEN * sizeof(unsigned long));
    m_cacheTail = m_cacheHead + TASK_QUEUE_LEN - 1;
    m_cacheConsumer = m_cacheHead;
    m_cacheProducer = m_cacheHead;
}

CTaskQueue::~CTaskQueue(){
    pthread_mutex_destroy(&m_mutexProducer);
    pthread_mutex_destroy(&m_mutexConsumer);
    
    sem_destroy(&m_semProducer);
    sem_destroy(&m_semConsumer);

    //begin:add by ypj at 2016/4/20
    if (m_cacheHead != NULL)
    {
        free(m_cacheHead);
        m_cacheHead = NULL;
        m_cacheTail = NULL;
        m_cacheProducer = NULL;
        m_cacheConsumer = NULL;
    }

    if (CTaskQueue::m_taskQueue != NULL)
    {
        delete CTaskQueue::m_taskQueue;
        CTaskQueue::m_taskQueue = NULL;
    }
    //end
    
}

CTaskQueue* CTaskQueue::Instance(){
    if(m_taskQueue == 0)
        m_taskQueue = new CTaskQueue();
    
    return m_taskQueue;
}

bool CTaskQueue::IsEmpty(){
    return ((m_cacheConsumer == m_cacheProducer) ?
            true : false);
}

void CTaskQueue::Push(AnalContent*& ac){
    sem_wait(&m_semProducer);
    pthread_mutex_lock(&m_mutexProducer);
    
    *m_cacheProducer = (unsigned long)ac;
    if(m_cacheProducer == m_cacheTail)
        m_cacheProducer = m_cacheHead;
    else
        m_cacheProducer ++;

    pthread_mutex_unlock(&m_mutexProducer);
    sem_post(&m_semConsumer);
}

void CTaskQueue::Pop(AnalContent* &ac){
    sem_wait(&m_semConsumer);
    pthread_mutex_lock(&m_mutexConsumer);
    
    ac = (AnalContent*)(*m_cacheConsumer);
    if(m_cacheConsumer == m_cacheTail)
        m_cacheConsumer = m_cacheHead;
    else
        m_cacheConsumer ++;

    pthread_mutex_unlock(&m_mutexConsumer);
    sem_post(&m_semProducer);
}
