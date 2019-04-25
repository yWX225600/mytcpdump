/**********************************************************************
* 功能描述： CSFTool类中是一下功能性的函数
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
#include "SFTool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**********************************************************************
* 功能描述： 以十进制显示指定buf中的值,16字节为一行
* 输入参数： BYTE* byValue:要现实buf地址
                                    int len:buf的长度
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CSFTool::ShowStrValueByDecimal(const BYTE* byValue,int len)
{
    if (byValue == NULL)
    {
        return;
    }
    int i = 0;
    int m = 1;
    for (; i < len ; i++, m++)
    {
        printf("%02u ", byValue[i]);
        if(m%16==0)  
            printf("\n");  
    }
    printf("\n");
    return;
}

/**********************************************************************
* 功能描述： 以十六进制显示指定buf中的值,16字节为一行
* 输入参数： BYTE* byValue:要现实buf地址
                                    int len:buf的长度
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* --------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*********************************************************************/
void CSFTool::ShowStrValueByHex(const BYTE* byValue,int len)
{
    if (byValue == NULL)
    {
        return;
    }
    int i = 0;
    int m = 1;
    for (; i < len ; i++, m++)
    {
        printf("%02x ", byValue[i]);
        if(m%16==0)  
            printf("\n");  
    }
    printf("\n");
    return;
}

/****************************************************************************
* 功能描述： 以字符串显示指定buf中的值,内容为'\0'则显示NULL
* 输入参数： BYTE* byValue:要现实buf地址
                                    int len:buf的长度
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* ---------------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*****************************************************************************/
void CSFTool::ShowStrValueByString(const BYTE* byValue,int len)
{
    if (byValue == NULL)
    {
        return;
    }
    BYTE * byTmp = NULL;
    byTmp = (BYTE*)malloc(len + 1);
    if (byTmp == NULL)
        return;
    
    //memset(byTmp, 0, len + 1);
	byTmp[len] = '\0';
    memcpy(byTmp, byValue, len);
    int i = 0;
    for (; i < len ; )
    {
        if (byTmp[i] == 0)
        {
            printf(" ");
            i++;
            continue;
        }
        printf("%s ", &byTmp[i]);
        i += strlen((char*)&byTmp[i]);
    }
    printf("\n");
    free(byTmp);
    byTmp = NULL;
    return;
}




#include <iostream>
using namespace std;
CAnalIni* CAnalIni::m_analIni = NULL;

/****************************************************************************
* 功能描述： 把config.ini配置文件中的数据解析到类对应的数据中
* 输入参数： BYTE* byValue:要现实buf地址
                                    int len:buf的长度
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期         版本号          修改人          修改内容
* ---------------------------------------------------------------------------
* 2016/8/10         V1.0            ypj             创建
*****************************************************************************/
CAnalIni::CAnalIni()
{
    INI_CONFIG* m_config;
    m_config=ini_config_create_from_file(INI_FILE_PATH,0);
    if (m_config == NULL)
    {
        m_dwMaxMtu = 65535;
        m_TimeOut = 0;
        m_bWritePkg = true;
        m_bShowPkg = true;
        m_bUseNullPkg = 1;
        m_strLogPath = "../log/pkg.log";
        m_strNetName = "";
        m_strRule = "";
        return;
    }
    m_dwMaxMtu = ini_config_get_int(m_config,"config", "maxMtu", 65535);
    m_TimeOut = ini_config_get_int(m_config,"config", "timeOut", 0);
    m_strLogPath = ini_config_get_string(m_config,"config", "logPath", "../log/pkg.log");
    m_strNetName = ini_config_get_string(m_config,"config", "networkName", "eth0");

    m_bUseNullPkg = ini_config_get_int(m_config,"seting", "useNullPkg", 1);
    m_bWritePkg = ini_config_get_int(m_config,"seting", "writePkg", 1);
    m_bShowPkg = ini_config_get_int(m_config,"seting", "showPkg", 1);
    m_strRule = ini_config_get_string(m_config,"seting", "listenRule", "");
    ini_config_destroy(m_config);
    /*
    cout <<"m_dwMaxMtu=" << m_dwMaxMtu<<endl;
    cout <<"m_TimeOut=" << m_TimeOut<<endl;
    cout <<"m_strLogPath=" << m_strLogPath<<endl;
    cout <<"m_strNetName=" << m_strNetName<<endl;
    cout <<"m_bWritePkg=" << m_bWritePkg<<endl;
    cout <<"m_bShowPkg=" << m_bShowPkg<<endl;
    cout <<"m_strRule=" << m_strRule<<endl;
    cout <<"m_bUseNullPkg=" << m_bUseNullPkg<<endl;
    */
}

CAnalIni::~CAnalIni()
{

}

CAnalIni* CAnalIni::Instance()
{
    if(m_analIni == NULL)
    {
        m_analIni = new CAnalIni();
    }
    return m_analIni;
}




