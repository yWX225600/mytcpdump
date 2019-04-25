/**********************************************************************
* 功能描述：解析UDP协议
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "UdpProtoAnal.h"

CUdpProtoAnal::CUdpProtoAnal()
{
    m_IpProtoType = PRO_TYPE_UDP;
}

DWORD CUdpProtoAnal::ProtocolAnalsis(AnalContent* &ac, const BYTE * IpPacketContent,int len)
{
    if (ac == NULL)
        return 0xffff;
    ac->protype = PRO_TYPE_UDP;
    
    UdpHeader *pUdpHheader = (UdpHeader*)IpPacketContent;
    ac->wSrcPort = ntohs(pUdpHheader->uh_sport);
    ac->wDstPort = ntohs(pUdpHheader->uh_dport);

    DWORD dwUdpHeaderLen = sizeof(UdpHeader);
    BYTE *pbyTmp = (BYTE*)(IpPacketContent + dwUdpHeaderLen);
    ac->dwAnalContentLen = len - dwUdpHeaderLen;
    ac->pbyAnalContent = (BYTE*)malloc(ac->dwAnalContentLen);
    if (ac->pbyAnalContent)
        memcpy(ac->pbyAnalContent, pbyTmp, ac->dwAnalContentLen);
    else //内存分配失败
        ac->dwAnalContentLen = 0;

    return 0;
}


