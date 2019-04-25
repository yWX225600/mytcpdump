/**********************************************************************
* 功能描述： 网络协议解析接口类(单粒)

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _PROTOCOLANALYSIS_H_
#define _PROTOCOLANALYSIS_H_
#include "BasicTypeDefine.h"
#include "IpProtoAnal.h"
#include "TaskQueue.h"

class CProtocolAnalysis
{
public:
    CProtocolAnalysis();
    virtual ~CProtocolAnalysis();
    static CProtocolAnalysis* Instance();
    
    DWORD IpProtoAnal(AnalContent* &ac, const BYTE* IpPacketContent, int len);

private:
    static CProtocolAnalysis* m_pCProtoAnal;
    CIpProtoAnal * m_pCipProtoAnal;

};
    
    
#endif


