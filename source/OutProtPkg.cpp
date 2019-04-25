/**********************************************************************
* 功能描述： 把ip协议处理完的包对外输出
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "OutProtPkg.h"
#include "ACQueue.h"
#include <time.h>
#include "PcapHeader.h"
#include "SFTool.h"

DWORD ShowAc(AnalContent&ac)
{
    printf("协议类型:%d\n",(int)ac.protype);
    time_t t = ac.tm.tv_sec;
    struct tm *ptm = localtime(&t);
    printf("接收包时间:%d/%d/%d %d:%d:%d(%d)\n", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)ac.tm.tv_usec);
    printf("源地址:%s:%d\n", ac.bySrcIp,ac.wSrcPort);
    printf("目标地址:%s:%d\n", ac.byDstIp,ac.wDstPort);
    printf("标识:%d\n", ac.wIpId);
    printf("偏移量:%d\n", ac.wIpOffset);
    if (ac.protype == PRO_TYPE_TCP)
    {
        printf("序列号:%u\n", ac.dwSeq);
        printf("确认号:%u\n", ac.dwAck);
    }
    printf("网卡原包内容:\n");
    const BYTE* byTmp = ac.pbySrcContent;
    CSFTool::ShowStrValueByHex(byTmp,ac.dwSrcContLen);

    printf("解析后的包内容:\n");
    CSFTool::ShowStrValueByHex(ac.pbyAnalContent, ac.dwAnalContentLen);
    printf("解析后的包字符串显示:\n");
    CSFTool::ShowStrValueByString(ac.pbyAnalContent, ac.dwAnalContentLen);
    printf("\n\n");
    return 0;
}

COutProtPkg::COutProtPkg()
{
    m_bExitFlg = false;
    
    m_mutex = new pthread_mutex_t;
    pthread_mutex_init(m_mutex, NULL);
    
    m_ProtoType.insert(make_pair((int)PRO_TYPE_ICMP,"ICMP"));
    m_ProtoType.insert(make_pair((int)PRO_TYPE_IGMP,"IGMP"));
    m_ProtoType.insert(make_pair((int)PRO_TYPE_TCP,"TCP"));
    m_ProtoType.insert(make_pair((int)PRO_TYPE_UDP,"UDP"));
    m_ProtoType.insert(make_pair((int)PRO_TYPE_ERR,"ERR"));

    //把256个ascII码的值转换成16进制的字符串
    char  str[] = "0123456789ABCDEF";
    string strTmp = "";
    int i = 0;
    int j = 0;
    for (;i < 16; i++)
    {
        for (j = 0; j < 16; j++)
        {
            strTmp +=str[i];
            strTmp +=str[j];
            m_VecCharHex.push_back(strTmp);
            strTmp = "";
        }
    }

    //设置默认操作函数

    //是否显示
    if (CAnalIni::Instance()->m_bShowPkg)
    {
        SetDealPkgFunc(ShowAc);
    }

    //是否写日志
    if (CAnalIni::Instance()->m_bWritePkg)
    {
        m_fp = fopen(CAnalIni::Instance()->m_strLogPath.c_str(), "a");
    }
}

COutProtPkg::~COutProtPkg()
{
    if (m_fp)
        fclose(m_fp);

    if (m_mutex)
    {        
        pthread_mutex_destroy(m_mutex);
        delete m_mutex;
    }
}

void COutProtPkg::Run()
{
    printf("COutProtPkg::Run()......\n");
    pthread_detach(pthread_self());
    if (m_fp == NULL)
    {
        printf("记录网络包文件没有打开\n");
        exit(0);
    }
    m_bExitFlg = false;
    AnalContent ac;
    
    while(m_bExitFlg == false)
    {
        CACQueue::Instance()->Pop(ac);
        //ShowAc(ac);
        //WriteAcToFile(ac);
        pthread_mutex_lock(m_mutex);
        vector<DealPkgFunction>::iterator itr=m_vecDealPkgFun.begin();
        for(;itr != m_vecDealPkgFun.end(); ++itr)
        {
            if (*itr == NULL)
                continue;

            (void)(*itr)(ac);
        }
        pthread_mutex_unlock(m_mutex);
        
        if (CAnalIni::Instance()->m_bWritePkg)
        {
            WriteAcToFile(ac);
        }
    }
    return;
}

void COutProtPkg::PthreadExit()
{
    m_bExitFlg = true;
}

void COutProtPkg::SetDealPkgFunc(DealPkgFunction  dealFunction)
{
    if (dealFunction == NULL)
    {
        return;
    }
    
    pthread_mutex_lock(m_mutex);
    m_vecDealPkgFun.push_back(dealFunction);
    pthread_mutex_unlock(m_mutex);
    return;
}


void COutProtPkg::WriteStrValueByString(const BYTE* byValue,int len)
{
    if (byValue == NULL)
    {
        return;
    }
    BYTE * byTmp = NULL;
    byTmp = (BYTE*)malloc(len + 1);
    if (byTmp == NULL)
        return;
    
    //memset(byTmp, 0, len + 1);
	byTmp[len] = '\0';
    memcpy(byTmp, byValue, len);
    int i = 0;
    string str ="";
    for (; i < len ; )
    {
        if (byTmp[i] == 0)
        {
            fprintf(m_fp, " ");
            i++;
            continue;
        }
        fprintf(m_fp, "%s", &byTmp[i]);
        i += strlen((char*)&byTmp[i]);
    }
    fprintf(m_fp, "\n");
    free(byTmp);
    byTmp = NULL;
    return;
}


void COutProtPkg::WriteAcToFile(AnalContent&ac)
{
    if (m_fp == NULL)
        return;
    string strType = m_ProtoType[(int)ac.protype];
    if (strType.length() == 0)
    {
        strType = "Unknown protocol";
    }
    time_t t = ac.tm.tv_sec;
    struct tm *ptm = localtime(&t);

    fprintf(m_fp, "protocol type:%s\n",strType.c_str());
    fprintf(m_fp, "time:%d/%02d/%02d %02d:%02d:%02d(%d)\n", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)ac.tm.tv_usec);
    fprintf(m_fp, "Src Addr:%s:%d\n", ac.bySrcIp,ac.wSrcPort);
    fprintf(m_fp, "Dst Addr:%s:%d\n", ac.byDstIp,ac.wDstPort);
    fprintf(m_fp, "Identification:%d\n", ac.wIpId);
    fprintf(m_fp, "Fragmentation offset:%d\n", ac.wIpOffset);
    if (ac.protype == PRO_TYPE_TCP)
    {
        fprintf(m_fp, "SEQ:%u\n", ac.dwSeq);
        fprintf(m_fp, "ACK:%u\n", ac.dwAck);
    }
    fprintf(m_fp, "Src package length:%d\n", ac.dwSrcContLen);
    fprintf(m_fp, "Src package content:\n"); //写入源包内容
    DWORD dwLen = 0;
    string str = "";
    for ( ; dwLen < ac.dwSrcContLen; dwLen++)
    {
        str += m_VecCharHex[(int)ac.pbySrcContent[dwLen]] + " ";
        if ((dwLen+1) % 32 == 0)//32个字符一行
        {
            str +="\n";
        }
    }
    fprintf(m_fp, "%s\n",str.c_str());
    
    fprintf(m_fp, "User content length:%d\n", ac.dwAnalContentLen);
    fprintf(m_fp, "User content:\n");//写入解析后的内容
    str = "";
    for (dwLen = 0 ; dwLen < ac.dwAnalContentLen; dwLen++)
    {
        str += m_VecCharHex[(int)ac.pbyAnalContent[dwLen]] + " ";
        if ((dwLen+1) % 32 == 0)//32个字符一行
        {
            str +="\n";
        }
    }
    fprintf(m_fp, "%s\n",str.c_str());
    
    fprintf(m_fp, "User content String display:\n");//写入解析后的内容字符串显示
    WriteStrValueByString(ac.pbyAnalContent, ac.dwAnalContentLen);
}

