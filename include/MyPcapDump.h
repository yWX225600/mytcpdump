/**********************************************************************
* 功能描述： 解析从网卡中抓到的包

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/9         V1.0              ypj            创建
*********************************************************************/

#ifndef _MYPCAPDUMP_H_
#define _MYPCAPDUMP_H_
#include "PcapHeader.h"
#include "BasicTypeDefine.h"
#include "pcap.h"
#include<sys/socket.h> 
#include "SFTool.h"
#include "CThread.h"


const DWORD DW_ERRBUF_LEN = 512;
//const DWORD DW_MAX_PACKET_SIZE = 65535;//最大包长度
class CMyPcapDump : public CThread
{
public:
    CMyPcapDump();
    virtual ~CMyPcapDump();
    DWORD MyPcapDumpInit();
    void AbortPcap();
    void ClosePcap();
    DWORD StartPcap();
    DWORD SetFilter(string &strFilter);

    //回调处理函数
    virtual void  CallBackFunc(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content);
private:
    virtual void Run();
  
private:
    
    pcap_t *m_Handle;
    struct bpf_program m_bfp;
    string m_strFilterExp;
    bpf_u_int32 m_net;
    bpf_u_int32 m_mask; 
    char m_ErrBuf[DW_ERRBUF_LEN];
    pcap_if_t *m_alldev; //存放网卡数据的容器
    int m_PacketCnt;//抓包数量-1表示一直抓包
    bool m_bExitFlg;//pcap_loop退出标志
    string m_strLocalIp;
    string m_strLocalMask;
    string m_strLocalMac;
    WORD m_wPort;
    struct in_addr m_LocalIpAddr;  
    struct in_addr m_LocalMaskAddr;  
    CSFTool m_sfTool;

};






#endif

