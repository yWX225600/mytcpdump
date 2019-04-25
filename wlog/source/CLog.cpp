/********************************************************
function：log class
author:ypj
version:1.0
date:2016.5.25
*********************************************************/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "CLog.h"
#include <pthread.h>
#include <signal.h>
#include <execinfo.h>

using namespace std;

void _signal_handler(int signo)
{    
    string strLog = "";
    switch(signo)
    {
        case SIGPIPE:
            strLog = ">>>>>>>>>>>>>>>>SIGPIPE,管道破裂<<<<<<<<<<<<<<<<<<<<<<<<<<";
            break;
            
        case SIGSEGV:
            strLog = ">>>>>>>>>>>>>>>>SIGSEGV,非法内存访问<<<<<<<<<<<<<<<<<<<<<<";
            break;
            
        case SIGFPE:
            strLog = ">>>>>>>>>>>>>>>>SIGFPE,数学相关的异常，如被0除，浮点溢出，等等<<<<<<<<<<<<<<<<<<<<<<";
            cout << strLog;
            break;

        case SIGABRT:
            strLog = ">>>>>>>>>>>>>>>>SIGABRT,由调用abort函数产生，进程非正常退出<<<<<<<<<<<<<<<<<<<<<<";
            break;
            
        case SIGBUS:
            strLog = ">>>>>>>>>>>>>>>>SIGBUS,总线错误<<<<<<<<<<<<<<<<<<<<<<";
            break;

        case SIGINT:
            strLog = ">>>>>>>>>>>>>>>>SIGINT,CTRL+C<<<<<<<<<<<<<<<<<<<<<<";
            cout << strLog;
            break;

        case SIGURG:
            strLog = ">>>>>>>>>>>>>>>>SIGBUS, 紧急套接字条件 <<<<<<<<<<<<<<<<<<<<<<";
            break;

        case SIGKILL:
            strLog = ">>>>>>>>>>>>>>>>SIGKILL, 杀死进程 <<<<<<<<<<<<<<<<<<<<<<";
            break;

        case SIGUSR2://手动刷新系统日志
            CWriteLog::GetInstance()->FlushLog();
            return;
            
        default:
            return;
    }

    
    CWriteLog::GetInstance()->CashWriteLog(LOG_CRASH,strLog.c_str());
    cout << strLog << endl;
    
    int size = 512;
    void * array[size];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
        CWriteLog::GetInstance()->CashWriteLog(LOG_CRASH,stacktrace[i]);
    }
    free(stacktrace);

    CWriteLog::GetInstance()->CloseFileLog();

    exit(0);
}


Log::Log()
{
    m_count = 0;
    m_tid = 0;
    m_mutex = new pthread_mutex_t;
    m_split_lines = 0;
    m_log_buf_size = 0;
    m_today = 0;
    m_is_async = false;
    m_log_queue = NULL;
    m_fp = NULL;
    m_buf = NULL;
    m_log_queue = NULL;
    memset(dir_name, 0, sizeof(dir_name));
    memset(log_name, 0, sizeof(log_name));
    memset(m_FileName, 0, sizeof(m_FileName));
    pthread_mutex_init(m_mutex, NULL);
    return;
}

Log::~Log()
{
    if (m_is_async)
    {
        
        m_is_async = false;
        string str = "日志程序退出!\n";
        m_log_queue->push(str);//作用是唤醒正在睡眠的日志队列，防止死锁
        //Detach();
        Join();
        FlushQueue();
        if (m_log_queue)
        {
            delete m_log_queue;
            m_log_queue = NULL;
        }
        
    }
    
    if(m_fp != NULL)
    {
        FlushMem();
        fclose(m_fp);
    }
    
    pthread_mutex_destroy(m_mutex);
    
    delete[] m_buf;
    if(m_mutex != NULL)
    {
        delete m_mutex;
        m_mutex = NULL;
    }
    return;
    
}

bool Log::Init(const char* file_name, int log_buf_size, int split_lines, int max_queue_size)
{
    if(max_queue_size >= 1)
    {
        m_is_async = true;
        m_log_queue = new BlockQueue<string>(max_queue_size);
        pthread_create(&m_tid, NULL, FlushLogThread, this);
    }

    m_log_buf_size = log_buf_size;
    m_buf = new char[m_log_buf_size];
    memset(m_buf, '\0', sizeof(m_buf));
    m_split_lines = split_lines;

    time_t t = time(NULL);
    struct tm* sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    const char *p = strrchr(file_name, '/');
    char log_full_name[256] = {0};
    if(p == NULL)
    {
        snprintf(log_full_name, 255, "%d_%02d_%02d_%s",my_tm.tm_year+1900, my_tm.tm_mon+1, my_tm.tm_mday, file_name);   
    }
    else
    {
        strcpy(log_name, p+1);
        strncpy(dir_name, file_name, p - file_name + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s",dir_name, my_tm.tm_year+1900, my_tm.tm_mon+1, my_tm.tm_mday, log_name ); 
    }

    m_today = my_tm.tm_mday;

    m_fp = fopen(log_full_name, "a");
    if(m_fp == NULL)
    {
        return false;
    }
    strncpy(m_FileName, log_full_name, strlen(log_full_name));
    return true;
}

void Log::WriteLog(int level, const char* format, ...)
{
    struct timeval now = {0,0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm* sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    char s[16] = {0};
    switch(level)
    {
        case 0 : strcpy(s, "[debug]:"); break;
        case 1 : strcpy(s, "[ info]:"); break;
        case 2 : strcpy(s, "[ warn]:"); break;
        case 3 : strcpy(s, "[ erro]:"); break;
        case 4 : strcpy(s, "[crash]:"); break;//程序崩溃
        default:
               strcpy(s, "[ info]:"); break;
    }

    pthread_mutex_lock(m_mutex);
    m_count++;
    if(m_today != my_tm.tm_mday || m_count % m_split_lines == 0) //everyday log
    {
        char new_log[256] = {0};
        fflush(m_fp);
        fclose(m_fp);
        char tail[16] = {0};
        snprintf(tail, 16,  "%d_%02d_%02d_", my_tm.tm_year+1900, my_tm.tm_mon+1, my_tm.tm_mday);
        if(m_today != my_tm.tm_mday)
        {  
            snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
            m_today = my_tm.tm_mday;
            m_count = 0;
            memset(m_FileName, 0, sizeof(m_FileName));
            strncpy(m_FileName,new_log,strlen(new_log));
        }
        else
        {   //超出行数
            snprintf(new_log, 255, "%s%s%s.%d", dir_name, tail, log_name, (int)m_count/m_split_lines);
            rename(m_FileName, new_log);         
        }

        //删除LOG_SAVE_DAY天之前的日志文件
        DelLog(LOG_SAVE_DAY);

        //创建当天的文件
        m_fp = fopen(m_FileName, "w+");
    }
    pthread_mutex_unlock(m_mutex);
    
    va_list valst;
    va_start(valst, format);
    
    pthread_mutex_lock(m_mutex);
    int len = snprintf(m_buf, m_log_buf_size, "%d-%02d-%02d %02d:%02d:%02d %s ",
            my_tm.tm_year+1900, my_tm.tm_mon+1, my_tm.tm_mday,
            my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, s);
    len += vsnprintf(m_buf + len, m_log_buf_size - len -1, format, valst);
    va_end(valst);
    
    m_buf[len] = '\n';
    len++;

    
    string strLog(m_buf,len);
    pthread_mutex_unlock(m_mutex);

    if(m_is_async && !m_log_queue->full())
    {
        m_log_queue->push(strLog);
    }
    else
    {
        pthread_mutex_lock(m_mutex);
        if (m_fp)
            fwrite(strLog.c_str(),strLog.length(),1,m_fp);
        pthread_mutex_unlock(m_mutex);
    }

    return;    
}

//删除指定days天之前的日志
void Log::DelLog(int days)
{
    time_t tms = time(NULL);
    tms = tms - days * 24 * 60 * 60;//得到days天前日期到1970年1月1日的秒数
    struct tm *ptm = localtime(&tms);
    if (ptm == NULL)
        return;

    //得到days天前日志名称
    char tail[16];
    memset(tail, 0, sizeof(tail));
    snprintf(tail, 16,  "%d_%02d_%02d_", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);
    char delLogName[256];
    memset(delLogName, 0, sizeof(delLogName));
    snprintf(delLogName, 255, "%s%s%s", dir_name, tail, log_name);
    
    int i = 1;//用于删除一天之类产生的多个文件
    //判断该文件是否存在是则删除
    while(1)
    {
        if((access(delLogName,F_OK)) == 0)
        {
            remove(delLogName);
        }
        else
        {
            //当该文件不存在时则跳出循环
            break;
        }

        snprintf(delLogName, 255, "%s%s%s.%d", dir_name, tail, log_name, i++);
    }
    return;
}

void Log::async_write_log()
{
    string strLog = "";
    while(m_is_async)
    {
        if (m_is_async && m_log_queue->pop(strLog))
        {
            pthread_mutex_lock(m_mutex);
            fwrite(strLog.c_str(),strLog.length(),1,m_fp);
            pthread_mutex_unlock(m_mutex);
        }
    }
    pthread_exit (NULL); 
}


void Log::FlushMem(void)
{
    pthread_mutex_lock(m_mutex);
    fflush(m_fp);
    pthread_mutex_unlock(m_mutex);
    return;
}

void Log::FlushQueue(void)
{
    string strLog = "";
    //如果不支持队列缓存日志则直接返回
    if (!m_is_async)
        return;
    while(m_log_queue->pop(strLog,20))
    {
        pthread_mutex_lock(m_mutex);
        if (m_fp)
            fwrite(strLog.c_str(),strLog.length(),1,m_fp);
        pthread_mutex_unlock(m_mutex);
    }
    return;
}


void Log::CloseLog(void)
{
    if (m_fp == NULL)
        return;
    
    if (m_is_async)
    {
        
        m_is_async = false;
        string str = "日志程序退出!\n";
        m_log_queue->push(str);//作用是唤醒正在睡眠的日志队列，防止死锁
        Join();
        FlushQueue();
        if (m_log_queue)
        {
            delete m_log_queue;
            m_log_queue = NULL;
        }
    }
    
    FlushMem();
    fclose(m_fp);
    m_fp = NULL;
    return;
}

bool Log::OpenLog(void)
{
    DelLog(LOG_SAVE_DAY);
    if (m_fp != NULL)
        return true;

    m_fp = fopen(m_FileName, "a");
    if (m_fp == NULL)
        return false;
    return true;
}


CWriteLog* CWriteLog::m_wLog = NULL;

CWriteLog::CWriteLog()
{
    m_psysLog = new Log();
    m_pcash = new Log();

    if (m_psysLog == NULL || m_pcash == NULL)
    {
        cout << "日志对象new 失败,请检查计算机内存，程序退出运行" << endl;
        exit(0);
    }
    bool bflg = m_psysLog->Init(_CLOG_OPERATE_PATH,LOG_LINE_NUN, LOGFILE_MAX_LINE,0);
    if (!bflg)
    {
        cout << "系统操作日志文件创建失败，程序退出运行" <<endl;
        exit(0);
    }

    bflg = m_pcash->Init(_CLOG_CASH_PATH,LOG_LINE_NUN, LOGFILE_MAX_LINE,0);
    if (!bflg)
    {
        cout << "系统崩溃日志文件创建失败，程序退出运行" <<endl;
        exit(0);
    }


    //异常信号注册
    signal(SIGPIPE, _signal_handler);    // SIGPIPE，管道破裂。
    signal(SIGSEGV, _signal_handler);    // SIGSEGV，非法内存访问
    signal(SIGFPE, _signal_handler);       // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    signal(SIGABRT, _signal_handler);     // SIGABRT，由调用abort函数产生，进程非正常退出
    signal(SIGBUS, _signal_handler);
    //signal(SIGINT, _signal_handler);
    signal(SIGURG, _signal_handler);
    signal(SIGKILL, _signal_handler);
    signal(SIGUSR2, _signal_handler);
    return;
}


CWriteLog::~CWriteLog()
{
    if (m_psysLog)
        delete m_psysLog;
    
    if (m_wLog)
        delete m_wLog;
    return;
}


CWriteLog* CWriteLog::GetInstance()
{
    if (m_wLog == NULL)
    {
        m_wLog = new CWriteLog();
    }
    return m_wLog;
}


void CWriteLog::WriteLog(LOGTYPE LogType ,int level, const char *file,const int line, const char *function, const char *fmt,...)
{
    char * buf = (char*)malloc(LOG_LINE_NUN);
    if (buf == NULL)
    {   
        cout << "写日志,申请堆空间失败" << endl;
        return;
    }
    memset(buf, 0 , LOG_LINE_NUN);

    //把文件路径去掉只留文件名
    file = strrchr(file, '/');
    if (file)
        file++;
    
    int len = snprintf(buf, LOG_LINE_NUN-1, "%s,%d,%s() <%d> ",file, line, function, getpid());
    va_list valst;
    va_start(valst, fmt);
    len += vsnprintf(buf + len, LOG_LINE_NUN - len -1, fmt, valst);
    va_end(valst);

    switch(LogType)
    {
        case LOG_OPERAT_TYPE:
            m_psysLog->WriteLog(level,buf);
            break;

        case LOG_CASH_TPYE:
            m_pcash->WriteLog(level,buf);
            break;
            
        default:
            m_psysLog->WriteLog(level,buf);
            break;
    }
    free(buf);
    buf = NULL;
    return;
}

void CWriteLog::SysWriteLog(const int level,const char* log)
{
    m_psysLog->WriteLog(level,log);
    return;
}

void CWriteLog::CashWriteLog(const int level,const char* log)
{
    m_pcash->WriteLog(level,log);
    return;
}


void CWriteLog::CloseFileLog()
{
    m_psysLog->CloseLog();
    m_pcash->CloseLog();
    return;
}

void CWriteLog::OpenFileLog()
{
    m_psysLog->OpenLog();
    m_pcash->OpenLog();
    return;
}


void CWriteLog::FlushLog()
{
    m_psysLog->FlushQueue();
    m_psysLog->FlushMem();

    m_pcash->FlushQueue();
    m_pcash->FlushMem();
    return;
}


