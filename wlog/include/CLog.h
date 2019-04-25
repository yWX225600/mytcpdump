/********************************************************
function:log class
version:1.0
author:ypj
date:2016.5.25
*********************************************************/

#ifndef _CLOG_H_
#define _CLOG_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "BlockQueue.h"
using namespace std;

//日志存放路径以及文件名(没有加时间之前)
#define _CLOG_OPERATE_PATH "../log/oplog.log"
#define _CLOG_CASH_PATH "../log/cashlog.log"
const int LOG_LINE_NUN = 2048;  //日志一次性写入最多2048个字节
const int LOGFILE_MAX_LINE = 2000000; //日志最大行数
const int LOG_SAVE_DAY = 15; //日志保留天数

typedef enum CLOG_LEVEL{
    LOG_DUBUG = 00,        //调试日志
    LOG_INFO ,         //普通记录
    LOG_WARNING,          //告警
    LOG_ERR,          //系统错误
    LOG_CRASH ,         //系统崩溃
} LOGLEVEL;

typedef enum LOG_TYPE{
    LOG_OPERAT_TYPE = 0, //系统日志
    LOG_DATABASE_TYPE,  //数据库日志
    LOG_CASH_TPYE,  //系统崩溃日志
}LOGTYPE;


class Log
{
public:
    Log();
    virtual ~Log();
   
    static void *FlushLogThread(void* args)
    {
        if (args)
        {
            (static_cast<Log*>(args))->async_write_log();
        }
        return 0;
    }
    inline int Detach(){return pthread_detach(m_tid);}    
    inline int Cancel(){return pthread_cancel(m_tid);}
    inline int Join(void **retval = NULL){return pthread_join(m_tid,retval);}
    
    bool Init(const char* file_name, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0);

    void WriteLog(int level, const char* format, ...);

    void FlushMem(void);

    void FlushQueue(void);

    void CloseLog(void);

    bool OpenLog(void);

private:
    virtual void async_write_log();
    void DelLog(int days);
    
private:
    pthread_mutex_t *m_mutex;
    char dir_name[128];
    char log_name[128];
    int m_split_lines;
    int m_log_buf_size;
    long long  m_count;
    int m_today;
    FILE *m_fp;
    char *m_buf;
    BlockQueue<string> *m_log_queue;
    bool m_is_async;
    pthread_t m_tid;
    char m_FileName[256];
};




class CWriteLog
{
public:
    virtual ~CWriteLog();
    static CWriteLog* GetInstance();
    void WriteLog(LOGTYPE LogType ,int level, const char *file,const int line, const char *function, const char *fmt,...);
    void SysWriteLog(const int level,const char* log);
    void CashWriteLog(const int level,const char* log);
    void CloseFileLog();
    void OpenFileLog();
    void FlushLog();
protected:
    CWriteLog();
    
    
private:
    static CWriteLog * m_wLog;
    Log *m_psysLog;//记录系统操作日志
    Log *m_pcash;//记录程序崩溃时的日志
};
#if 0
//关闭日志文件
#define LOG_CLOSEFILE() CWriteLog::GetInstance()->CloseFileLog();
//打开日志文件
#define LOG_OPENFILE() CWriteLog::GetInstance()->OpenFileLog();


//数据库日志
#define DBLOG_DEBUG(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_DUBUG, File, Line, Fuction, format, __VA_ARGS__);
#define DBLOG_INFO(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_INFO, File, Line, Fuction,format, __VA_ARGS__);
#define DBLOG_WARN(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_WARNING, File, Line, Fuction,format, __VA_ARGS__);
#define DBLOG_ERROR(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_ERR, File, Line, Fuction,format, __VA_ARGS__);

//系统操作日志
#define OPLOG_DEBUG(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_DUBUG, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_INFO(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_INFO, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_WARN(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_WARNING, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_ERROR(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_ERR, File, Line, Fuction, format, __VA_ARGS__);

#define _LOG_DEBUG(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_DUBUG, LOG);
#define _LOG_INFO(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_INFO, LOG);
#define _LOG_WARN(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_WARNING, LOG);
#define _LOG_ERROR(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_ERR, LOG);
#endif

#endif 

