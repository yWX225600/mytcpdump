/**********************************************************************
* 功能描述：多线程处理pcap抓到的包

* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _DEALPROTPKG_H_
#define _DEALPROTPKG_H_
#include "CThread.h"
#include "BasicTypeDefine.h"

class CDealProtPkg :public CThread
{
public:
    virtual ~CDealProtPkg();
    CDealProtPkg();

private:
    virtual void Run();

};



#endif



