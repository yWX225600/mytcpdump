版本v1.0
该版本主要功能是用pcap采集网卡上数据并且解析出来对外处理(目前只实现了UDP和tcp协议解析，CDealProtPkg::Run() 调用CProtocolAnalysis类中解析)
对外提供的输出解析后的数据接口函数typedef DWORD (*DealPkgFunction)(AnalContent&ac);
在COutProtPkg类中的    virtual void SetDealPkgFunc(DealPkgFunction );可以注册接口函数
CAnalIni类解析在config目录中的config.ini配置文件，在inipares目录中
生成可执行文件在bin目录中
log为日志目录不可以删除
wlog为日志代码路径
处理流程
CMyPcapDump：：MyPcapDumpInit()初始化采集环境
	|
	v
CMyPcapDump::SetFilter() //设置采集规则
	|
	v
CMyPcapDump类中的线程函数Run()进行采集或者直接调用DWORD StartPcap()采集
	|
	|
	V
把采集好的数据包通过CTaskQueue类，单粒队列送给CDealProtPkg中的线程函数Run()进行解析
(根据采集数据多少设置合理处理线程)
	|
	|
	v
把解析好的内容通过CACQueue类中的队列给COutProtPkg中的线程处理函数Run来对外输出结果COutProtPkg::SetDealPkgFunc()注册的



例子
#include <stdio.h>
#include <string.h>
#include <pcap.h>
#include <string>
#include <iostream>

#include "MyPcapDump.h"
#include "DealProtPkg.h"
#include "OutProtPkg.h"
using namespace std;

//把内容打印出来
DWORD DealPkg(AnalContent&ac)
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

int main()
{
    //(3个线程组成的)线程池处来理包
    CDealProtPkg dealProtPkg;
    dealProtPkg.Start();
    CDealProtPkg dealProtPkg1;
    dealProtPkg1.Start();
    CDealProtPkg dealProtPkg2;
    dealProtPkg2.Start();


    //输出处理好的结果
    COutProtPkg outProtPkg;
	outProtPkg.SetDealPkgFunc(DealPkg);//注册接口函数
    outProtPkg.Start();


    //采集线程
    CMyPcapDump myPcapDump;
    myPcapDump.MyPcapDumpInit();
    string strFilter = prot 8889；
    myPcapDump.SetFilter(strFilter);
    #if 0
    myPcapDump.StartPcap();//主线程直接采集
    #else
    myPcapDump.Start(); //单独开现场采集
    while(1)
    {
        sleep(5);
    }
    #endif
    myPcapDump.ClosePcap();
}
