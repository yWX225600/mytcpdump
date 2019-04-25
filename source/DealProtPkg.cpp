/**********************************************************************
* 功能描述：多线程处理pcap抓到的包

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "DealProtPkg.h"
#include "TaskQueue.h"
#include "ACQueue.h"
#include "ProtocolAnalysis.h"
#include "SFTool.h"

CDealProtPkg::CDealProtPkg()
{
    
}


CDealProtPkg::~CDealProtPkg()
{

}

/**********************************************************************
* 功能描述： 输出线程处理线程函数
* 输入参数：无
* 输出参数：无
* 返 回 值： void
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CDealProtPkg::Run()
{
    printf("CDealProtPkg::Run......\n");
    pthread_detach(pthread_self());
    AnalContent*ac = NULL;
    bool bUseNullPkg = CAnalIni::Instance()->m_bUseNullPkg;
    while(1)
    {
        CTaskQueue::Instance()->Pop(ac);
        if (ac == NULL)
        {
            printf("[%u]获取到数据为空\n", GetTID());
            continue;
        }
        EthHeader * pEthHeader = NULL;
        pEthHeader = (EthHeader *)ac->pbySrcContent;
        int EthHeaderLen = sizeof(EthHeader);
        
        pEthHeader->Eth_type = ntohs(pEthHeader->Eth_type);
        DWORD dwRet = 0;
        switch(pEthHeader->Eth_type)
        {
            //case ETH_TYPE_RARP://rarp解析
            //    break;

            //case ETH_TYPE_ARP://arp协议解析

           //     break;

            case ETH_TYPE_IP://ip协议解析
            {
                BYTE *byIpContent = (BYTE*)(ac->pbySrcContent + EthHeaderLen);
                dwRet = CProtocolAnalysis::Instance()->IpProtoAnal(ac, byIpContent, ac->dwSrcContLen - EthHeaderLen);
                break;
            }    
            default:
            {
                printf("未知协议 %d", pEthHeader->Eth_type);
                dwRet = 0xff;
                break;
            }
        }
        if (dwRet == 0)
        {
            if (bUseNullPkg)
            {
                CACQueue::Instance()->Push(*ac);
            }
            else
            {
                if (ac->dwAnalContentLen)
                    CACQueue::Instance()->Push(*ac);
            }
        }
        
        delete ac;
        ac = NULL;
    }
    return;
}


