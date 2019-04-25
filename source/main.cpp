/**********************************************************************
* 功能描述：抓包

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/8         V1.0             ypj            创建
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <pcap.h>
#include <string>
#include <iostream>
#if 1

#include "MyPcapDump.h"
#include "DealProtPkg.h"
#include "OutProtPkg.h"
#include "SFTool.h" 
using namespace std;

int main()
{
    //线程池处理包
    CDealProtPkg dealProtPkg;
    dealProtPkg.Start();
    CDealProtPkg dealProtPkg1;
    dealProtPkg1.Start();
    CDealProtPkg dealProtPkg2;
    dealProtPkg2.Start();


    //输出处理好的结果
    COutProtPkg outProtPkg;
    outProtPkg.Start();


    //采集线程
    CMyPcapDump myPcapDump;
    myPcapDump.MyPcapDumpInit();
    string strFilter = CAnalIni::Instance()->m_strRule;
    myPcapDump.SetFilter(strFilter);
    #if 0
    myPcapDump.StartPcap();
    #else
    myPcapDump.Start();
    while(1)
    {
        sleep(5);
    }
    #endif
    myPcapDump.ClosePcap();
}



#else
using namespace std;

#define MAX_PACKET_SIZE 1500
 
 
typedef struct mac_header
{
    u_char dstmacaddress[6];
    u_char srcmacaddress[6];
    u_short type;
}mac_header;
 
typedef struct ip_address
{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;
 
typedef struct ip_header
{
    u_char ver_ihl;
    u_char tos;
    u_short tlen;
    u_short identification;
    u_short flags_fo;
    u_char ttl;
    u_char proto;
    u_short crc;
    ip_address saddr;
    ip_address daddr;
}ip_header;
 
typedef struct tcp_header
{
    u_short sourport;
    u_short destport;
    unsigned int sequnum;
    unsigned int acknowledgenum;
    u_short headerlenandflag;
    u_short windowsize;
    u_short checksum;
    u_short urgentpointer;
}tcp_header;
 
typedef struct udp_header
{
    u_short sourport;
    u_short destport;
    u_short length;
    u_short checksum;
}udp_header;
void showValue(const u_char* value,int len)
{
    int i = 0;
    int m = 1;
    for (; i < len ; i++, m++)
    {
        printf("%02x ", value[i]);
        if(m%16==0)  
            printf("\n");  
    }
    printf("\n");
    return;
}
void ip_analyse(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)
{
    ip_header *ipheader;
    string reader;
    mac_header *macheader;
    int protoHeaderLen = 0;
    u_char * chrPlay = NULL;
    const u_char* tmp= packet_content;
    fprintf(stderr, "packetLen = %d\n",packet_header->len); 
    fprintf(stderr, "caplen = %d\n",packet_header->caplen); 
    printf("=====================================================\n");
    showValue(tmp, packet_header->len);
    printf("=====================================================\n");
    macheader = (struct mac_header *)packet_content;
    fprintf(stderr, "SourceMac:%02x:%02x:%02x:%02x:%02x:%02x\n",macheader->srcmacaddress[0],macheader->srcmacaddress[1],macheader->srcmacaddress[2],macheader->srcmacaddress[3],macheader->srcmacaddress[4],macheader->srcmacaddress[5]);
    fprintf(stderr, "DestinationMac:%02x:%02x:%02x:%02x:%02x:%02x\n",macheader->dstmacaddress[0],macheader->dstmacaddress[1],macheader->dstmacaddress[2],macheader->dstmacaddress[3],macheader->dstmacaddress[4],macheader->dstmacaddress[5]);
     
    ipheader = (struct ip_header *)&packet_content[sizeof(struct mac_header)];
    fprintf(stderr, "SourceIP:%d.%d.%d.%d\n",ipheader->saddr.byte1,ipheader->saddr.byte2,ipheader->saddr.byte3,ipheader->saddr.byte4);
    fprintf(stderr, "DestinationIP:%d.%d.%d.%d\n",ipheader->daddr.byte1,ipheader->daddr.byte2,ipheader->daddr.byte3,ipheader->daddr.byte4);
    if(6 == (int)ipheader->proto)
    {
        fprintf(stderr, "TCP Packet\n");
        reader = packet_content[sizeof(struct mac_header)+sizeof(struct ip_header)+sizeof(struct ip_header)+sizeof(struct tcp_header)];
        protoHeaderLen = sizeof(struct mac_header) + sizeof(struct ip_header) + sizeof(struct tcp_header);
        chrPlay = const_cast<u_char*>(&packet_content[protoHeaderLen]);
    }
    else if( 17 == (int)ipheader->proto)
    {
        fprintf(stderr, "UDP Packet\n");
        reader = packet_content[sizeof(struct mac_header)+sizeof(struct ip_header)+sizeof(struct ip_header)+sizeof(struct udp_header)];
    }
    
    printf("【value】:");
    showValue(chrPlay, packet_header->len - protoHeaderLen);
    fprintf(stderr, "【Packet】:%s\n", &packet_content[protoHeaderLen]);
//  size_t position = 0;
//  char str[5] = {'\r','\n','\r','\n','\0'};
//  position = reader.find(str,0,4);
//  printf("Position of \"\\r\\n\\r\\n\" at %lu\n",position);
//  return ;
}
 
int main()
{
    pcap_t *descr = NULL;
    struct bpf_program fp;
    //设置过滤规则，只获取tcp和udp报文   
    char filter_exp[] = "port 8889";
    bpf_u_int32 net = 0;
    bpf_u_int32 mask = 0; /* 所在网络的掩码 */
     
    char *dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    memset(errbuf, '\0', PCAP_ERRBUF_SIZE);
 
    dev = pcap_lookupdev(errbuf);
    if(dev == NULL)
    {
        fprintf(stderr, "Could't find default device:%s\n", errbuf);
        return 2;
    }

    /* 探查设备属性 */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Could't get device pro:%s\n", errbuf);
        return 2;
    }

    /* 以混杂模式打开会话 */
    descr = pcap_open_live(dev, MAX_PACKET_SIZE, 1, 1000, errbuf);
    if(descr == NULL)
    {
        fprintf(stderr, "Couldn't open device %s:%s", dev,errbuf);
        return 2;
    }

    printf("Interface is:%s\n",dev);  
    if(pcap_compile(descr, &fp, filter_exp, 0, net) == -1)
    {
        fprintf(stderr, "Couldn't install parse filter %s:%s", filter_exp, pcap_geterr(descr));
        return 2;
    }
    
    if (pcap_setfilter(descr, &fp) == -1)
    {
        fprintf(stderr, "Couldn't set filter %s:%s", filter_exp, pcap_geterr(descr));
        return 2;
    }

    pcap_loop(descr, -1, ip_analyse, NULL);
    pcap_close(descr);
    return  0;
}

#endif

