/**********************************************************************
* 功能描述：解析tcp协议
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "TcpProtoAnal.h"
#include "BasicTypeDefine.h"
#include "SFTool.h"

CTcpProtoAnal::CTcpProtoAnal()
{
    m_IpProtoType = PRO_TYPE_TCP;
}

/**********************************************************************
* 功能描述： TCP协议解析
* 输入参数：const BYTE * IpPacketContent:要解析的数据
                                int len:要解析数据长度
* 输出参数： AnalContent* &ac 把解析后的数据出口参数
* 返 回 值： DWORD 0成功,非0失败
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
DWORD CTcpProtoAnal::ProtocolAnalsis(AnalContent* &ac, const BYTE * IpPacketContent,int len)
{
    if (ac == NULL)
        return 0xffff;
    ac->protype = PRO_TYPE_TCP;
    TcpHeader *pTcpHheader = (TcpHeader*)IpPacketContent;
    ac->wSrcPort = ntohs(pTcpHheader->th_sport);
    ac->wDstPort = ntohs(pTcpHheader->th_dport);
    ac->dwSeq = ntohl(pTcpHheader->th_seq);
    ac->dwAck = ntohl(pTcpHheader->th_ack);
    #if 0
    printf("---------TCP协议---------\n");
    printf("源端口:%u\n", ac->wSrcPort );
    printf("目的端口:%u\n", ac->wDstPort);
    printf("序列号SEQ:%u\n",ac->dwSeq);
    printf("确认号ACK SEQ:%u\n", ac->dwAck);
    pTcpHheader->th_sum =ntohs(pTcpHheader->th_sum);
    printf("TCP校验和:%u\n",pTcpHheader->th_sum);
    #endif
    DWORD dwTcpHeaderLen = sizeof(TcpHeader);
    BYTE *pbyTmp = (BYTE*)(IpPacketContent + dwTcpHeaderLen);
    ac->dwAnalContentLen = len - dwTcpHeaderLen;
    ac->pbyAnalContent = (BYTE*)malloc(ac->dwAnalContentLen);
    if (ac->pbyAnalContent)
        memcpy(ac->pbyAnalContent, pbyTmp, ac->dwAnalContentLen);
    else
        ac->dwAnalContentLen = 0;
    
    //printf("=============TCP=内容=十六进制=====================================\n");
    //CSFTool::ShowStrValueByHex(ac->pbyAnalContent, ac->dwAnalContentLen);
    //printf("=============TCP=内容=字符串=======================================\n");
    //CSFTool::ShowStrValueByString(ac->pbyAnalContent, ac->dwAnalContentLen);
    //printf("\n\n");
    return 0;
}

