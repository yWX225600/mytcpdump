#include "MyPcapDump.h"
#include "ProtocolAnalysis.h"
#include <arpa/inet.h>
#include "TaskQueue.h"
#include "SFTool.h"

void CallBack(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    //这里申请ac内存，在使用处释放
    AnalContent * ac = new AnalContent(packet_header->ts, (BYTE*)packet_content, packet_header->len);
    if (ac)
    {
        //printf("有数据来-------------------------\n");
        //CSFTool::ShowStrValueByHex(ac->pbySrcContent,ac->dwSrcContLen);
        CTaskQueue::Instance()->Push(ac);
    }
    return;
        
    //if (argument == NULL)
    //    return;
    //CMyPcapDump* tmp = (CMyPcapDump*)argument;
    //tmp->CallBackFunc(NULL, packet_header, packet_content);
}

CMyPcapDump::CMyPcapDump()
{
    m_Handle = NULL;
    memset(&m_bfp, 0, sizeof(struct bpf_program));
    m_strFilterExp = "";
    m_net = 0;
    m_mask = 0;
    memset(m_ErrBuf, 0, DW_ERRBUF_LEN);
    m_alldev = NULL;
    m_PacketCnt = -1;
    m_bExitFlg = false;

    memset(&m_LocalIpAddr, 0, sizeof(struct in_addr));
    memset(&m_LocalMaskAddr, 0, sizeof(struct in_addr));

}


CMyPcapDump::~CMyPcapDump()
{

}

void CMyPcapDump::CallBackFunc(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{

    const BYTE* tmp = (BYTE*)packet_content;
    /*
    printf("=====================================================\n");
    fprintf(stderr, "packetLen = %d\n",packet_header->len); 
    fprintf(stderr, "caplen = %d\n",packet_header->caplen); 
    m_sfTool.ShowStrValueByHex(tmp, packet_header->len);
    printf("=====================================================\n");
    */
    EthHeader * pEthHeader = NULL;
    pEthHeader = (EthHeader *)packet_content;
    int EthHeaderLen = sizeof(EthHeader);
    pEthHeader->Eth_type = ntohs(pEthHeader->Eth_type);
    printf("ethtype:%x\n",pEthHeader->Eth_type);
    switch(pEthHeader->Eth_type)
    {
        case ETH_TYPE_RARP://rarp解析
            break;

        case ETH_TYPE_ARP://arp协议解析

            break;

        case ETH_TYPE_IP://ip协议解析
        {
            //BYTE *byIpContent = (BYTE*)(packet_content + EthHeaderLen);
            //CProtocolAnalysis::Instance()->IpProtoAnal(byIpContent,packet_header->len - EthHeaderLen);
            //AnalIpPro(byIpContent, packet_header->len - EthHeaderLen);
            break;
        }    
        default:
            printf("未知协议 %d", pEthHeader->Eth_type);
            return;
    }
    return;
}


DWORD CMyPcapDump::MyPcapDumpInit()
{
    pcap_if_t *p;  
    struct in_addr net_ip_addr;  
    struct in_addr net_mask_addr;  
  
    char *net_ip_string;  
    char *net_mask_string;  
    char *interface = NULL;  
    u_int32_t net_ip;  
    u_int32_t net_mask;  
  
    struct pcap_pkthdr pack;   
    const u_char *content;  
  
    int i=0,num;  
    if(pcap_findalldevs(&m_alldev,m_ErrBuf)==-1)  
    {  
        printf("find all devices is error\n");  
        return 0;  
    }  
    for(p=m_alldev;p;p=p->next)  
    {  
        printf("%d:%s\n",++i,p->name);  
        if(p->description)  
        {  
            printf("%s\n",p->description);  
        }  
    }  
    if(i==1)  
        interface=p->name;  
    else  
    {   
        string strName = CAnalIni::Instance()->m_strNetName;
        if (strName.length() == 0)
        {
            printf("please input which interface you want to use\n");  
            scanf("%d",&num);  
            if(num<1||num>i)  
            {  
                printf("interface is unavillible\n");  
                return 0;  
            }  
            for(p=m_alldev,i=1;i<=num;p=p->next,i++)  
                interface=p->name;  
        }
        else
        {
            
            for(p=m_alldev;p;p=p->next)  
            {  
                printf("%d:%s\n",++i,p->name);  
                if (strName.compare(p->name) == 0)
                {
                    interface=p->name;
                    break;
                }
            } 
            if (interface == NULL)
            {
                //printf("配置文件中的networkName字段错误\n");
                _SYS_DEBUG(__FILE__, __LINE__, __FUNCTION__,"%s","配置文件中的networkName字段错误");
                OPLOG_ERROR(__FILE__, __LINE__, __FUNCTION__,"%s","配置文件中的networkName字段错误");
                exit(0);
            }
        }
    }  
    /* 
    if((interface=pcap_lookupdev(error))==NULL) 
    { 
        printf("%s\n",error); 
        return 0; 
    }*/  
    DWORD dwMaxMtu =  CAnalIni::Instance()->m_dwMaxMtu;
    DWORD dwTimeOut =  CAnalIni::Instance()->m_TimeOut;
    if((m_Handle = pcap_open_live(interface, dwMaxMtu, 1, dwTimeOut, m_ErrBuf))==NULL)  
    {  
        printf("%s\n",m_ErrBuf);  
        return 0;  
    }

    //获取该网络设备的IP和mask
    if(pcap_lookupnet(interface,&net_ip,&net_mask,m_ErrBuf)==-1)   
    {  
        printf("%s\n",m_ErrBuf);  
        return 0;  
    }  
    printf("Interface is:%s\n",interface);  
    m_LocalIpAddr.s_addr = net_ip;  
    net_ip_string = inet_ntoa(m_LocalIpAddr);
    m_strLocalIp.copy(net_ip_string, strlen(net_ip_string),0);
    printf("The ip is:%s\n",net_ip_string);  
    
    m_LocalMaskAddr.s_addr = net_mask;  
    net_mask_string = inet_ntoa(m_LocalMaskAddr);  
    m_strLocalMask.copy(net_mask_string, strlen(net_mask_string),0);
    printf("The mask is:%s\n",net_mask_string);  

    

}


/**********************************************************************
* 功能描述： 设置pcap抓包表达式
* 输入参数：string 表达式
* 输出参数： 无
* 返 回 值    ： DWORD 成功返回0
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/

DWORD CMyPcapDump::SetFilter(string &strFilter)
{
    if (m_Handle == NULL)
    {
        //log
        OPLOG_ERROR(__FILE__, __LINE__, __FUNCTION__,"%s","pcap句柄为空，请先初始化");
        return 0;
    }
    m_strFilterExp = strFilter;
    const char * chrFilter = m_strFilterExp.c_str();
    OPLOG_DEBUG(__FILE__, __LINE__, __FUNCTION__,"parse filter: %s",chrFilter);
    _SYS_DEBUG(__FILE__, __LINE__, __FUNCTION__,"parse filter: %s",chrFilter);
    if(pcap_compile(m_Handle, &m_bfp, chrFilter, 0, m_LocalMaskAddr.s_addr) == -1)
    {
        OPLOG_ERROR(__FILE__, __LINE__, __FUNCTION__,"Couldn't install parse filter %s:%s",chrFilter,pcap_geterr(m_Handle));
        fprintf(stderr, "Couldn't install parse filter %s:%s", chrFilter, pcap_geterr(m_Handle));
        return 2;
    }
    
    if (pcap_setfilter(m_Handle, &m_bfp) == -1)
    {
        OPLOG_ERROR(__FILE__, __LINE__, __FUNCTION__,"Couldn't set filter %s:%s",chrFilter, pcap_geterr(m_Handle));
        fprintf(stderr, "Couldn't set filter %s:%s", chrFilter, pcap_geterr(m_Handle));
        return 2;
    }
    return 0;
}

/**********************************************************************
* 功能描述： 启动pacp抓包
* 输入参数：无
* 输出参数： 无
* 返 回 值    ： DWORD, 成功返回抓包个数，失败返回0,
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
DWORD CMyPcapDump::StartPcap()
{
    m_bExitFlg = false;
    //Handle->break_loop = 0; //防止之前调用了AbortPcap()
    int iRet = pcap_loop(m_Handle,m_PacketCnt,CallBack,(u_char*)this);  
    m_bExitFlg = true;
    if (iRet < 0)
    {
        OPLOG_ERROR(__FILE__, __LINE__, __FUNCTION__,"pcap_loop failed,return value is %d",iRet);
        return 0;
    }
    return iRet;
}

/**********************************************************************
* 功能描述： 退出pcap抓包
* 输入参数：无
* 输出参数： 无
* 返 回 值    ： void
* 其它说明： 建议在退出采集时候使用,退出过后不能再次启动
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CMyPcapDump::AbortPcap()
{
    if (m_Handle == NULL)
        return;
    pcap_breakloop(m_Handle);
    return;
}

/**********************************************************************
* 功能描述： pcap_loop退出回收pcap句柄相关资源
* 输入参数： 无
* 输出参数： 无
* 返 回 值：     void
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CMyPcapDump::ClosePcap()
{
    while (m_bExitFlg == false);
    pcap_close(m_Handle);
    pcap_freealldevs(m_alldev);
    m_Handle = NULL;
    m_alldev = NULL;
    m_bExitFlg = false;
}

/**********************************************************************
* 功能描述： 线程处理函数
* 输入参数： 无
* 输出参数： 无
* 返 回 值：     void
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CMyPcapDump::Run()
{
    pthread_detach(pthread_self());
    StartPcap();

}


