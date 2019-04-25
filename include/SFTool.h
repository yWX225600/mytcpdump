/**********************************************************************
* 功能描述： CSFTool类中是一下功能性的函数
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#ifndef _SFTOOL_H_
#define _SFTOOL_H_

#include "BasicTypeDefine.h"
#include "iniconfile.h"
#include <string>
//该类用于显示
class CSFTool
{
public:
    static void ShowStrValueByDecimal(const BYTE* byValue,int len);
    static void ShowStrValueByHex(const BYTE* byValue,int len);
    static void ShowStrValueByString(const BYTE* byValue,int len);


};



/**********************************************************************
* 功能描述： 解析config目录下面的配置文件config.ini
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/

#define INI_FILE_PATH "../config/config.ini"
class CAnalIni
{
public:
    virtual ~CAnalIni();
    static CAnalIni * Instance();
public:
    DWORD m_dwMaxMtu;
    DWORD m_TimeOut;
    bool m_bWritePkg;
    bool m_bShowPkg;
    bool m_bUseNullPkg; //空包是否接收
    string m_strLogPath;
    string m_strNetName;
    string m_strRule;
    
private:
    CAnalIni();
private:    
    static CAnalIni* m_analIni;
    
};

#endif

