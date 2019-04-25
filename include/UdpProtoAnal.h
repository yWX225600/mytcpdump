/**********************************************************************
* 功能描述：解析UDP协议
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _UDPPROTOANAL_H_
#define _UDPPROTOANAL_H_
#include "IpProtoAnal.h"

class CUdpProtoAnal :public CIpProtoAnal
{
public:
    CUdpProtoAnal();
    virtual ~CUdpProtoAnal(){;}
    virtual DWORD ProtocolAnalsis(AnalContent* &ac, const BYTE * IpPacketContent,int len);
    

};

#endif

