#ifndef _STATUS_CHANGE_QUEUE_H_
#define _STATUS_CHANGE_QUEUE_H_

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "BasicTypeDefine.h"
#include "PcapHeader.h"

const WORD STR_IP_MAX_LEN = 16; 
#define TASK_QUEUE_LEN 1000
//解析后的输出接口
typedef struct anal_content{
    BYTE* pbySrcContent;//原始数据
    DWORD dwSrcContLen; //原始数据长度
    ProType protype; //IP协议上承载的协议类型
    struct timeval tm;//接收到数据时的时间
    WORD wSrcPort;//原始端口
    WORD wDstPort;//目的端口
    WORD wIpId; //标识
    WORD wIpOffset; // 偏移(13位)
    struct in_addr ip_src;  /* source  address */
    struct in_addr ip_dst; /*dest address*/
    BYTE bySrcIp[STR_IP_MAX_LEN];
    BYTE byDstIp[STR_IP_MAX_LEN];
    DWORD dwSeq; //TCP序列号
    DWORD dwAck; //TCP 确认号
    BYTE *pbyAnalContent; //解析后到应用层的内容
    DWORD dwAnalContentLen; //解析协议内容长度
    anal_content()
    {
        pbySrcContent = 0;
        dwSrcContLen = 0;
        tm.tv_sec = 0;
        tm.tv_usec = 0;
        protype = PRO_TYPE_ERR;
        wSrcPort = 0;
        wDstPort = 0;
        wIpId = 0;
        wIpOffset = 0;
        ip_src.s_addr = 0;
        ip_dst.s_addr = 0;
        dwSeq = 0;
        dwAck = 0;
        pbyAnalContent = NULL;
        dwAnalContentLen = 0;
        memset(bySrcIp, 0, STR_IP_MAX_LEN);
        memset(byDstIp, 0, STR_IP_MAX_LEN);
    }
    anal_content(const struct timeval ts, const BYTE* bySrcContent, DWORD dwSrcLen)
    {
        pbySrcContent = (BYTE*)malloc(dwSrcLen);
        //memset(pbySrcContent, 0, dwSrcLen);
        memcpy(pbySrcContent, bySrcContent, dwSrcLen);
        dwSrcContLen = dwSrcLen;
        //gettimeofday(&tm,NULL);
        tm.tv_sec = ts.tv_sec;
        tm.tv_usec = ts.tv_usec;
        protype = PRO_TYPE_ERR;
        wSrcPort = 0;
        wDstPort = 0;
        wIpId = 0;
        wIpOffset = 0;
        ip_src.s_addr = 0;
        ip_dst.s_addr = 0;
        dwSeq = 0;
        dwAck = 0;
        pbyAnalContent = NULL;
        dwAnalContentLen = 0;
        memset(bySrcIp, 0, STR_IP_MAX_LEN);
        memset(byDstIp, 0, STR_IP_MAX_LEN);        
    }

    ~anal_content()
    {
        if (pbySrcContent)
        {
            free(pbySrcContent);
            pbySrcContent = NULL;
        }

        if (pbyAnalContent)
        {
            free(pbyAnalContent);
            pbyAnalContent = NULL;
        }
    }

    anal_content& operator= (const anal_content& in)
    {
        if (pbySrcContent)
            free(pbySrcContent);
        pbySrcContent = NULL;

        if (in.dwSrcContLen)
        {
            pbySrcContent = (BYTE*)malloc(in.dwSrcContLen);
            memcpy(pbySrcContent, in.pbySrcContent,in.dwSrcContLen);
        }
        dwSrcContLen = in.dwSrcContLen;

        if (pbyAnalContent)
            free(pbyAnalContent);
        pbyAnalContent = NULL;

        if (in.dwAnalContentLen)
        {
            pbyAnalContent = (BYTE*)malloc(in.dwAnalContentLen);
            memcpy(pbyAnalContent, in.pbyAnalContent,in.dwAnalContentLen);
        }
        dwAnalContentLen = in.dwAnalContentLen;

        protype = in.protype; //IP协议上承载的协议类型
        tm = in.tm;//接收到数据时的时间
        wSrcPort = in.wSrcPort;//原始端口
        wDstPort = in.wDstPort;//目的端口
        wIpId = in.wIpId;
        wIpOffset = in.wIpOffset;
        ip_src = in.ip_src;  /* source  address */
        ip_dst = in.ip_dst; /*dest address*/
        memset(bySrcIp, 0, STR_IP_MAX_LEN);
        strcpy((char*)bySrcIp, (char*)in.bySrcIp);
        memset(byDstIp, 0, STR_IP_MAX_LEN);
        strcpy((char*)byDstIp, (char*)in.byDstIp);
        dwSeq = in.dwSeq; //TCP序列号
        dwAck = in.dwAck; //TCP 确认号
        return *this;
    }

    void acInit()
    {
        if (pbySrcContent)
            free(pbySrcContent);
        pbySrcContent = NULL;

        dwSrcContLen = 0;

        if (pbyAnalContent)
            free(pbyAnalContent);
        pbyAnalContent = NULL;
        dwAnalContentLen = 0;
        
        protype = PRO_TYPE_ERR;
        wSrcPort = 0;
        wDstPort = 0;
        ip_src.s_addr = 0;
        ip_dst.s_addr = 0;
        dwSeq = 0;
        dwAck = 0;
        memset(bySrcIp, 0, STR_IP_MAX_LEN);
        memset(byDstIp, 0, STR_IP_MAX_LEN);        
    }
    
}AnalContent;

class CTaskQueue{
	public:
		virtual ~CTaskQueue();
	public:
		static CTaskQueue* Instance();
		void Push(AnalContent*&);
		void Pop(AnalContent*&);
		bool IsEmpty();
		
	protected:
		CTaskQueue();
		
	private:
		static CTaskQueue* m_taskQueue;
		pthread_mutex_t m_mutexProducer;
		pthread_mutex_t m_mutexConsumer;
		sem_t m_semProducer;
		sem_t m_semConsumer;
		unsigned long* m_cacheHead;
		unsigned long* m_cacheTail;
		unsigned long* m_cacheProducer;
		unsigned long* m_cacheConsumer;
		
};








#endif

