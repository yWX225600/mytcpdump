#ifndef _C_THREAD_H_
#define _C_THREAD_H_


#include <pthread.h>

class CThread{
    public:
        CThread(){};
        virtual ~CThread(){};
        
    public:
        inline void Start(pthread_attr_t *tattr = NULL){pthread_create(&ct_tID, tattr, ThreadFunc, this);}
        inline static void* ThreadFunc(void* pthreadOBJ){(static_cast<CThread*>(pthreadOBJ))->Run();}
        inline pthread_t GetTID(){return ct_tID;}
        inline int Join(void **retval = NULL){return pthread_join(ct_tID,retval);}
        inline int Detach(){return pthread_detach(ct_tID);}
        // inline void Exit(void *retval = NULL){pthread_exit(retval);}
        inline virtual void Cancel(){pthread_cancel(ct_tID);}
        
    private:
        virtual void Run() = 0;
        
    private:
        pthread_t ct_tID;
};




#endif


