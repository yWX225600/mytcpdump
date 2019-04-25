/**********************************************************************
* 功能描述： 用于存放AnalContent处理过后的队列,给消费者提供接口
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _ACQUEUE_H_
#define _ACQUEUE_H_
//#include "BlockQueue.h"
#include "TaskQueue.h"
#include "BasicTypeDefine.h"

class CACQueue
{
public:
    virtual ~CACQueue();
    static CACQueue* Instance();

    void Clear();
    bool IsFull()const;
    bool IsEmpty()const;
    //bool Front(T& value)const
    DWORD Size();
    DWORD MaxSize();
    bool Push(AnalContent&);
    
    bool Pop(AnalContent& ac);
    bool Pop(AnalContent& ac, int ms_timeout);
    

protected:
    CACQueue(int max_size);

private:
    static CACQueue *m_CACQueue;
    //BlockQueue<AnalContent> m_BQueue;

    pthread_mutex_t *m_mutex;
    pthread_cond_t *m_cond;
    AnalContent *m_array;
    DWORD m_size;
    DWORD m_max_size;
    DWORD m_front;
    DWORD m_back;

};


#endif
