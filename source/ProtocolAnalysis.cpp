/**********************************************************************
* 功能描述： 网络协议解析接口类(单粒)

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "ProtocolAnalysis.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TcpProtoAnal.h"
#include "UdpProtoAnal.h"

//#include "SFTool.h"

CProtocolAnalysis *CProtocolAnalysis::m_pCProtoAnal = NULL;

CProtocolAnalysis::CProtocolAnalysis()
{
}

CProtocolAnalysis::~CProtocolAnalysis()
{
};

CProtocolAnalysis* CProtocolAnalysis::Instance()
{
    if (m_pCProtoAnal == NULL)
    {
        m_pCProtoAnal = new CProtocolAnalysis();
    }
    return m_pCProtoAnal;
}


DWORD CProtocolAnalysis::IpProtoAnal(AnalContent* &ac, const BYTE * IpPacketContent,int len)
{
    if (ac == NULL)
    {
        return 0xffff;
    }
    
    if (IpPacketContent == NULL)
    {
        return 0xffff;
    }
    
    IpHeader *pIpHeader = (IpHeader*)IpPacketContent;
    #if 0
    printf("---------IP协议---------\n");
    printf("版本号:%d\n", IP_V(pIpHeader));
    printf("首部长度:%d\n",IP_HL(pIpHeader));
    printf("服务类型:%d\n",pIpHeader->ip_tos);
    pIpHeader->ip_len = ntohs(pIpHeader->ip_len);
    printf("总长度:%d\n",pIpHeader->ip_len);
    //printf("标识:%d\n",ntohs(ip_protocol->ip_id));
    //printf("偏移:%d\n",(offset & 0x1fff) * 8);
    printf("生存时间:%d\n",pIpHeader->ip_ttl);
    printf("协议类型:%d\n",pIpHeader->ip_p);
    pIpHeader->ip_sum = ntohs(pIpHeader->ip_sum);
    printf("检验和:%d\n",pIpHeader->ip_sum);
    #endif
    ac->wIpId = ntohs(pIpHeader->ip_id);
    ac->wIpOffset = (ntohs(pIpHeader->ip_off)) & IP_OFFMASK;
    ac->ip_src.s_addr = pIpHeader->ip_src.s_addr;
    strcpy((char*)ac->bySrcIp, inet_ntoa(pIpHeader->ip_src));

    ac->ip_dst.s_addr = pIpHeader->ip_dst.s_addr;
    strcpy((char*)ac->byDstIp, inet_ntoa(pIpHeader->ip_dst));

    //printf("源IP地址:%s\n", (char*)ac->bySrcIp);
    //printf("目的地址:%s\n", (char*)ac->byDstIp);

    DWORD dwIpHeaderLen = sizeof(IpHeader);
    BYTE * pbyTmp = (BYTE*)(IpPacketContent + dwIpHeaderLen);

    switch(pIpHeader->ip_p)
    {
    case PRO_TYPE_ICMP:

        break;
    case PRO_TYPE_IGMP:

        break;
    case PRO_TYPE_UDP:
    {
        CUdpProtoAnal udpProAnal;
        udpProAnal.ProtocolAnalsis(ac, pbyTmp, len - dwIpHeaderLen);
        break;
    }
    case PRO_TYPE_TCP:
    {
        CTcpProtoAnal tcpProAnal;
        
        //printf("TCP协议调试-----------------------------\n");
        //CSFTool::ShowStrValueByHex(pbyTmp,(int)(len - dwIpHeaderLen));
        tcpProAnal.ProtocolAnalsis(ac, pbyTmp, len - dwIpHeaderLen);
        break;
    }
    default:
        printf("other ip protocol:%d\n",pIpHeader->ip_p);
        break;
            
    }
    return 0;

}

