#ifndef _BASIC_TYPE_DEFINE_H_
#define _BASIC_TYPE_DEFINE_H_

#include <stdio.h>
#include <string.h>
#include <iostream>
#include "CLog.h"

#ifdef _OS_TYPE

#define _OS_BASE_MEM_UNIT_LEN    sizeof(int *)


    #if _OS_TYPE==64
        typedef unsigned long int        _uint64_t;
        typedef signed long int          _sint64_t;
    #elif _OS_TYPE==32
        typedef unsigned long long int   _uint64_t;
        typedef signed long long int     _sint64_t;
    #endif
        
        typedef unsigned int             _uint32_t;
        typedef signed int               _sint32_t;
        typedef unsigned short           _uint16_t;
        typedef signed short             _sint16_t;
        typedef unsigned char            _uint8_t;
        typedef signed char              _sint8_t;
        
        typedef float                    _ufloat32_t;
        typedef double                   _ufloat64_t;
        typedef long double              _uldouble_t;        //32位系统为12字节，96位；64位系统为16字节，128位

#endif
        //begin:add by ypj at 2016.4.29
        typedef unsigned int DWORD;
        typedef unsigned short WORD;
        typedef unsigned char BYTE;

        //判断大小端
        static union{		
            char c[4];
            unsigned long l;
        }endian_test = {{'l','?','?','b'}}; 
        #define ENDIANNESS ((char)endian_test.l)
        static bool CheckOsBigOrLittle()
        {
            if (ENDIANNESS == 'l')
                return false;//小端返回FALSE
            else
                return true; //大端返回true
        }
        
        typedef union dwrod_byte{
                  DWORD dwInt;
                  BYTE byStr[4];
                 } DW_BYTE;
        typedef union wrod_byte{
          WORD wInt;
          BYTE byStr[2];
         } W_BYTE;
        
        inline DWORD DW2BYTE(DWORD dwValue, BYTE *&byBuf)
        {
            if (byBuf == NULL)
                return 0xff;
            DW_BYTE tmp;
            tmp.dwInt = dwValue;
            byBuf[0] = tmp.byStr[0];
            byBuf[1] = tmp.byStr[1];
            byBuf[2] = tmp.byStr[2];
            byBuf[3] = tmp.byStr[3]; 
            return 0;
        }
        inline DWORD BYTE2DW(BYTE *byBuf,DWORD &dwValue)
        {
            if (byBuf == NULL)
                return 0xff;
            DW_BYTE tmp;
            if (CheckOsBigOrLittle() == false)//小端
            {
                tmp.byStr[0] = byBuf[0];
                tmp.byStr[1] = byBuf[1];
                tmp.byStr[2] = byBuf[2];
                tmp.byStr[3] = byBuf[3];
            }
            else //大端
            {
                tmp.byStr[0] = byBuf[3];
                tmp.byStr[1] = byBuf[2];
                tmp.byStr[2] = byBuf[1];
                tmp.byStr[3] = byBuf[0];
            }
            dwValue = tmp.dwInt;
            return 0;
        }

        inline DWORD W2BYTE(WORD wValue, BYTE *&byBuf)
        {
            if (byBuf == NULL)
                return 0xff;
            W_BYTE tmp;
            tmp.wInt = wValue;
            byBuf[0] = tmp.byStr[0];
            byBuf[1] = tmp.byStr[1];
        }

        inline DWORD BYTE2W(BYTE *byBuf,WORD &wValue)
        {
            if (byBuf == NULL)
                return 0xff;
            W_BYTE tmp;
            if (CheckOsBigOrLittle() == false)//小端
            {
                tmp.byStr[0] = byBuf[0];
                tmp.byStr[1] = byBuf[1];
            }
            else //大端
            {
                tmp.byStr[0] = byBuf[1];
                tmp.byStr[1] = byBuf[0];
            }
            wValue = tmp.wInt;
            return 0;
        }
        //end
        
//bool定义
typedef enum{
    _B_FALSE  = 0,
    _B_TRUE   = 1,
    _B_IGNORE = 2
} _TBOOL;

//typedef enum bl{
//    FALSE = 0,
//    TRUE =1
//}BOOL;
typedef bool BOOL;
#if 1
//日志初始化接口
#define LOG_INIT() CWriteLog::GetInstance();

//数据库日志接口
#define DBLOG_DEBUG(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_DUBUG, File, Line, Fuction, format, __VA_ARGS__);
#define DBLOG_INFO(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_INFO, File, Line, Fuction,format, __VA_ARGS__);
#define DBLOG_WARN(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_WARNING, File, Line, Fuction,format, __VA_ARGS__);
#define DBLOG_ERROR(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_DATABASE_TYPE, (int)LOG_ERR, File, Line, Fuction,format, __VA_ARGS__);

//系统操作日志接口
#define OPLOG_DEBUG(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_DUBUG, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_INFO(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_INFO, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_WARN(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_WARNING, File, Line, Fuction, format, __VA_ARGS__);
#define OPLOG_ERROR(File,Line,Fuction,format, ...) CWriteLog::GetInstance()->WriteLog(LOG_OPERAT_TYPE, (int)LOG_ERR, File, Line, Fuction, format, __VA_ARGS__);

#define _LOG_DEBUG(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_DUBUG, LOG);
#define _LOG_INFO(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_INFO, LOG);
#define _LOG_WARN(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_WARNING, LOG);
#define _LOG_ERROR(LOG) CWriteLog::GetInstance()->SysWriteLog((int)LOG_ERR, LOG);
#endif



#ifdef _DEBUG
#include <stdarg.h>
#endif
static inline void _SYS_DEBUG(const char *file,const int line, const char *function,const char *fmt,...){
#ifdef _DEBUG
    printf("\e[1;30m" "[%s %d] %s: " "\e[0m",file,line,function);
    char printf_buf[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(printf_buf, fmt, args);
    va_end(args);
    puts(printf_buf);
#endif
}

#if 0
#ifdef _LOG
#include "TLog.h"
    #define _LOG_OPERATE_PATH "../log/"
    #define _LOG_WARN_PATH    "../log/"
    #define _LOG_OPERATE_DB_PATH    "../log/db/"
#endif
static inline void _SYS_LOG(const char* log,const LOG_LEVEL level){
#ifdef _LOG
    switch(level){
        case LOG_OPERATE:
            TLog::InputLogS(_LOG_OPERATE_PATH,"",log,level);
            break;
        case LOG_WARN:
            TLog::InputLogS(_LOG_WARN_PATH,"",log,level);
            break;
        default:
            
            break;
    }
#endif
}
static inline void ALARM_LOG(const char *file,const int line, const char *function,const char *fmt,...){
#ifdef _LOG
    char log_buf[1024];
    memset(log_buf, 0, sizeof(log_buf));
    va_list args;
    va_start(args, fmt);
    vsprintf(log_buf, fmt, args);
    va_end(args);

    char w_buf[2048];
    memset(w_buf, 0, sizeof(log_buf));
    snprintf(w_buf,sizeof(w_buf)-1,"%s, %d, %s: %s",
                    file,line,function,log_buf);
    TLog::InputLogS(_LOG_WARN_PATH,"_ypj",w_buf,LOG_WARN);
#endif
}


static inline void DB_OPLOG(const char *file,const int line, const char *function,const char *fmt,...){
#ifdef _LOG
    char log_buf[1024];
    memset(log_buf, 0, sizeof(log_buf));
    va_list args;
    va_start(args, fmt);
    vsprintf(log_buf, fmt, args);
    va_end(args);

    char w_buf[2048];
    memset(w_buf, 0, sizeof(log_buf));
    char * chFile = NULL;
    //把路径中的路径去掉,只留文件名
    file= strrchr(file, '/');
    if (file)
        file += 1;
        
    snprintf(w_buf,sizeof(w_buf)-1,"%s, %d, %s: %s",
                    chFile,line,function,log_buf);
        
    #ifdef _DEBUG
    puts(w_buf);
    #endif
    TLog::InputLogS(_LOG_OPERATE_DB_PATH,"",w_buf,LOG_DBOPERATE);
#endif
}
#endif
#endif




