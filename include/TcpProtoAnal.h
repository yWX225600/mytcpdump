/**********************************************************************
* 功能描述：解析tcp协议
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _TCPPROTOANAL_H_
#define _TCPPROTOANAL_H_
#include "IpProtoAnal.h"

class CTcpProtoAnal :public CIpProtoAnal
{
public:
    CTcpProtoAnal();
    virtual ~CTcpProtoAnal(){;}
    virtual DWORD ProtocolAnalsis(AnalContent* &ac, const BYTE * IpPacketContent,int len);
    

};

#endif
