/**********************************************************************
* 功能描述： 解析基于IP协议的上层协议
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _IPPROTOANAL_H_
#define _IPPROTOANAL_H_
#include "PcapHeader.h"
#include "TaskQueue.h"

class CIpProtoAnal
{
public:
    CIpProtoAnal();
    virtual ~CIpProtoAnal(){;}
    virtual DWORD ProtocolAnalsis(AnalContent* &ac, const BYTE * IpPacketContent,int len) = 0;
    ProType GetProType(){return m_IpProtoType;}
    
protected:
    ProType m_IpProtoType;

};

#endif
