/**********************************************************************
* 功能描述： 把ip协议处理完的包对外输出
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/

#ifndef _OUTPROTPKG_H_
#define _OUTPROTPKG_H_
#include "CThread.h"
#include "TaskQueue.h"
#include<vector>
#include <map>
#include <string>

#define FILE_PATH "../log/pkg.log"

using namespace std;
typedef DWORD (*DealPkgFunction)(AnalContent&ac);
class COutProtPkg :public CThread
{
public:
    COutProtPkg();
    virtual ~COutProtPkg();
    virtual void PthreadExit();
    virtual void SetDealPkgFunc(DealPkgFunction );
private:
    virtual void Run();
//    void ShowAc(AnalContent&ac);
    void WriteAcToFile(AnalContent&ac);
    void WriteStrValueByString(const BYTE* byValue,int len);
private:
    bool m_bExitFlg; //线程退出标志true 退出
    FILE *m_fp; //把解析结果输出到文件中
    map<int, string>m_ProtoType;
    vector<string> m_VecCharHex;
    vector<DealPkgFunction> m_vecDealPkgFun;
    pthread_mutex_t *m_mutex;
};




#endif
