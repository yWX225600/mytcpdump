/**********************************************************************
* 功能描述： 用于存放AnalContent处理过后的队列,给消费者提供接口
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "ACQueue.h"


CACQueue* CACQueue::m_CACQueue = NULL;
CACQueue::CACQueue(int max_size)
{
    if(max_size <= 0)
    {
        cout <<"BlockQueue class max_size = " << max_size << endl;
        exit(-1);
    }
    
    m_max_size = max_size;
    m_array = new AnalContent[max_size];
    m_size = 0;
    m_front = -1;
    m_back = -1;
    
    m_mutex = new pthread_mutex_t;
    m_cond = new pthread_cond_t;
    pthread_mutex_init(m_mutex, NULL);
    pthread_cond_init(m_cond, NULL);

}

CACQueue::~CACQueue()
{

}

CACQueue* CACQueue::Instance()
{
    if (m_CACQueue == NULL)
    {
        m_CACQueue = new CACQueue(100);
    }
    return m_CACQueue;
}

void CACQueue::Clear()
{
    pthread_mutex_lock(m_mutex);
    m_size = 0;
    m_front = -1;
    m_back = -1;
    pthread_mutex_unlock(m_mutex);
}

bool CACQueue::IsFull()const
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

bool CACQueue::IsEmpty()const
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
//bool Front(T& value)const
DWORD CACQueue::Size()
{
    DWORD tmp = 0;
    pthread_mutex_lock(m_mutex);
    tmp = m_size;
    pthread_mutex_unlock(m_mutex);
    return tmp;
}

DWORD CACQueue::MaxSize()
{
    DWORD tmp = 0;
    pthread_mutex_lock(m_mutex);
    tmp = m_max_size;
    pthread_mutex_unlock(m_mutex);
    return tmp;
}

bool CACQueue::Push(AnalContent& ac)
{
    pthread_mutex_lock(m_mutex);
    if(m_size >= m_max_size)
    {
        pthread_cond_broadcast(m_cond);
        pthread_mutex_unlock(m_mutex);
        return false;
    }
    
    m_back = (m_back + 1) % m_max_size;
    m_array[m_back] = ac;
    
    m_size++;
    
    pthread_cond_broadcast(m_cond);
    pthread_mutex_unlock(m_mutex);
    
    return true;
}

bool CACQueue::Pop(AnalContent& ac)
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
    ac = m_array[m_front];
    m_array[m_front].acInit();
    m_size--;
    pthread_mutex_unlock(m_mutex);
    return true;
}

bool CACQueue::Pop(AnalContent& ac, int ms_timeout)
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
    ac = m_array[m_front];
    m_array[m_front].acInit();
    m_array[m_front];

    m_size--;
    pthread_mutex_unlock(m_mutex);
}


