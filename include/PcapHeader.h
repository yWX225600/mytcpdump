/**********************************************************************
* 功能描述： 定义相关协议头

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/9                        V1.0              ypj                            创建
*********************************************************************/
#ifndef _PCAPHEADER_H_
#define _PCAPHEADER_H_

#include <netinet/in.h>
#include "BasicTypeDefine.h"
#include <sys/time.h>

#ifndef 	ETHER_ADDR_LEN
#define	ETHER_ADDR_LEN		6
#endif

#pragma pack( push, 2) 
typedef enum ethtype{
    ETH_TYPE_IP = 0x0800,
    ETH_TYPE_ARP = 0x0806,
    ETH_TYPE_RARP = 0x0835
}EthType;

typedef enum protocol_type{
    PRO_TYPE_ICMP = 1,
    PRO_TYPE_IGMP = 2,
    PRO_TYPE_TCP = 6,
    PRO_TYPE_UDP  = 17,
    PRO_TYPE_ERR
}ProType;

//以太帧头
typedef struct	eth_header {
	BYTE		Eth_dhost[ETHER_ADDR_LEN];
	BYTE		Eth_shost[ETHER_ADDR_LEN];
	WORD	Eth_type;
}EthHeader;

typedef struct ip_header{
        BYTE     ip_vhl;     /* header length, version */
        #define IP_V(ip)	(((ip)->ip_vhl & 0xf0) >> 4)
        #define IP_HL(ip)	((ip)->ip_vhl & 0x0f)
        BYTE     ip_tos;     /* type of service */
        WORD    ip_len;     /* total length */
        WORD    ip_id;      /* identification  标识*/
        WORD    ip_off;     /* fragment offset field  偏移(13位)*/
        #define	IP_DF 0x4000			/* dont fragment flag */
        #define	IP_MF 0x2000			/* more fragments flag */
        #define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
        BYTE     ip_ttl;     /* time to live */
        BYTE     ip_p;       /* protocol 协议类型(tcp/udp...)*/
        WORD    ip_sum;     /* checksum 头部校验和 */
        struct in_addr ip_src;  /* source  address */
        struct in_addr ip_dst; /*dest address*/
}IpHeader;

//TCP协议头
typedef struct tcp_header{
	WORD	th_sport;		/* source port */
	WORD	th_dport;		/* destination port */
	DWORD		th_seq;			/* sequence number */
	DWORD		th_ack;			/* acknowledgement number */
	BYTE		th_offx2;		/* data offset, rsvd */
	BYTE		th_flags;
    #define TH_FIN	    0x01
    #define TH_SYN	    0x02
    #define TH_RST	    0x04
    #define TH_PUSH	0x08
    #define TH_ACK	    0x10
    #define TH_URG	    0x20
	WORD	th_win;			/* window */
	WORD	th_sum;			/* checksum */
	WORD	th_urp;			/* urgent pointer */
}TcpHeader;

//udp协议头
typedef struct udp_header{
	WORD	uh_sport;		/* source port */
	WORD	uh_dport;		/* destination port */
	WORD	uh_ulen;		/* udp length */
	WORD	uh_sum;	    /* udp checksum */
}UdpHeader;



#pragma pack( pop)  

#endif
