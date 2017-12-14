#include <System.h>
#include "ISPOSTv2.h"
#include <qsdk/tcpcommand.h>


using namespace std;


//#define ISPOSTV2_IQ_TUNING_DEBUG       1


int IPU_ISPOSTV2::GetDNParam(void* pData)
{
    TCPCommand * pTcp = (TCPCommand *)pData;
    ISPOST_MDLE_DN * pModDN = (ISPOST_MDLE_DN *)((unsigned char *)pData + sizeof(TCPCommand));
    ipu_ispost_3d_dn_param* pIspostDN = IspostGetDnParamList();
    int iRet = -1;
    int iParamCnt = ISPOST_DN_LEVEL_MAX;
    int i;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Get-ISPOSTv2_DN\n");
    }

    if (!pTcp || !pModDN || !pIspostDN) {
        LOGE("ISPOSTv2 get DN Param failed - Tcp=%p, pModDN=%p, pIspostDN=%p\n",
             pTcp, pModDN, pIspostDN);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_DN_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_DN_CMD_VER_V0:
                default:
                    if (iParamCnt != IspostGetDnParamCount())
                       iParamCnt = IspostGetDnParamCount();
                    pModDN->bEnable = m_IsDnManual;
                    pModDN->iDnUsedCnt = iParamCnt;
                    memcpy(pModDN->stDnParam, pIspostDN, (sizeof(ipu_ispost_3d_dn_param) * iParamCnt));
                    if (ISPOST_DN_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISPOST_MDLE_DN);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 get bEnable(%s)\n", (pModDN->bEnable ? "Enable" : "Disable"));
                        LOGE("ISPOSTv2 get iDnUsedCnt(%d), iParamCnt(%d)\n", pModDN->iDnUsedCnt, iParamCnt);
                        for (i=0; i < iParamCnt; i++) {
                            LOGE("ISPOSTv2 get stDnParam[%d]: %d %d %d %d\n",
                                i,
                                pModDN->stDnParam[i].iThresholdY,
                                pModDN->stDnParam[i].iThresholdU,
                                pModDN->stDnParam[i].iThresholdV,
                                pModDN->stDnParam[i].iWeight
                                );
                        }
                        LOGE("ISPOSTv2 get DN Param.datasize: %u\n", pTcp->datasize);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::SetDNParam(void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISPOST_MDLE_DN * pModDN = (ISPOST_MDLE_DN *)((unsigned char *)pData + sizeof(TCPCommand));
    ipu_ispost_3d_dn_param* pIspostDN = IspostGetDnParamList();
    int iRet = -1;
    int iParamCnt = ISPOST_DN_LEVEL_MAX;
    int i;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Set-ISPOSTv2_DN\n");
    }

    if (!pTcp || !pModDN || !pIspostDN) {
        LOGE("ISPOSTv2 set DN Param failed - Tcp=%p, pModDN=%p, pIspostDN=%p\n",
             pTcp, pModDN, pIspostDN);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_DN_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_DN_CMD_VER_V0:
                default:
                    if (iParamCnt != IspostGetDnParamCount())
                       iParamCnt = IspostGetDnParamCount();
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 set bEnable(%s)\n", (pModDN->bEnable ? "Enable" : "Disable"));
                        LOGE("ISPOSTv2 set iDnUsedCnt(%d), iParamCnt(%d)\n", pModDN->iDnUsedCnt, iParamCnt);
                        for (i=0; i < iParamCnt; i++) {
                            LOGE("ISPOSTv2 set stDnParam[%d]: %d %d %d %d\n",
                                i,
                                pModDN->stDnParam[i].iThresholdY,
                                pModDN->stDnParam[i].iThresholdU,
                                pModDN->stDnParam[i].iThresholdV,
                                pModDN->stDnParam[i].iWeight
                                );
                        }
                        LOGE("ISPOSTv2 set DN Param.datasize: %u\n", pTcp->datasize);
                    }

                    m_IsDnManual = pModDN->bEnable;
                    //iParamCnt = pModDN->iDnUsedCnt;
                    memcpy(pIspostDN, pModDN->stDnParam, (sizeof(ipu_ispost_3d_dn_param) * iParamCnt));
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::GetIspostVersion(void * pData)
{
    TCPCommand * pTcp = (TCPCommand *)pData;
    ISPOST_MDLE_VER * pModVer = (ISPOST_MDLE_VER *)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Get-ISPOSTv2_Version.\n");
    }

    if (!pTcp || !pModVer) {
        LOGE("ISPOSTv2 get Version parameters failed - Tcp=%p, pModVer=%p\n",
             pTcp, pModVer);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_VER_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_VER_CMD_VER_V0:
                default:
                    pModVer->iVersion = ISPOST_VERSION_V2;
                    if (ISPOST_VER_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISPOST_MDLE_VER);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 get Version.datasize: %u\n", pTcp->datasize);
                        LOGE("ISPOSTv2 get Version.iVersion = %d\n", pModVer->iVersion);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::GetIqTuningDebugInfo(void * pData)
{
    TCPCommand * pTcp = (TCPCommand *)pData;
    ISPOST_MDLE_DBG * pModDbg = (ISPOST_MDLE_DBG *)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Get-ISPOSTv2_Debug.\n");
    }

    if (!pTcp || !pModDbg) {
        LOGE("ISPOSTv2 get Debug parameters failed - Tcp=%p, pModDbg=%p\n",
             pTcp, pModDbg);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_DBG_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_DBG_CMD_VER_V0:
                default:
                    pModDbg->bDebugEnable = m_bIqTuningDebugInfoEnable;
                    if (ISPOST_DBG_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISPOST_MDLE_DBG);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 get Debug.datasize: %u\n", pTcp->datasize);
                        LOGE("ISPOSTv2 get Debug.bDebugEnable = %d\n", pModDbg->bDebugEnable);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::SetIqTuningDebugInfo(void * pData)
{
    TCPCommand * pTcp = (TCPCommand *)pData;
    ISPOST_MDLE_DBG * pModDbg = (ISPOST_MDLE_DBG *)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Set-ISPOSTv2_Debug.\n");
    }

    if (!pTcp || !pModDbg) {
        LOGE("ISPOSTv2 set Debug parameters failed - Tcp=%p, pModDbg=%p\n",
             pTcp, pModDbg);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_DBG_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_DBG_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 set Debug.datasize: %u\n", pTcp->datasize);
                        LOGE("ISPOSTv2 set Debug.bDebugEnable = %d\n", pModDbg->bDebugEnable);
                    }

                    m_bIqTuningDebugInfoEnable = pModDbg->bDebugEnable;
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::GetGridFilePath(void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISPOST_MDLE_GRID_FILE_PATH* pModGridFilePath = (ISPOST_MDLE_GRID_FILE_PATH*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Get-ISPOSTv2_GRID_PATH.\n");
    }

    if (!pTcp || !pModGridFilePath) {
        LOGE("ISPOSTv2 get grid file path failed - Tcp=%p, pModGridFilePath=%p\n",
             pTcp, pModGridFilePath);
        return iRet;
    }
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISPOST_GRID_PATH_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISPOST_GRID_PATH_CMD_VER_V0:
                default:
                    memcpy(pModGridFilePath->szGridFilePath[0], m_szGridFilePath[0], 256);
                    memcpy(pModGridFilePath->szGridFilePath[1], m_szGridFilePath[1], 256);
                    if (ISPOST_GRID_PATH_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISPOST_MDLE_GRID_FILE_PATH);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("ISPOSTv2 get GRID_PATH.datasize: %u\n", pTcp->datasize);
                        LOGE("ISPOSTv2 get GRID_PATH.szGridFilePath[0] = %s\n", pModGridFilePath->szGridFilePath[0]);
                        LOGE("ISPOSTv2 get GRID_PATH.szGridFilePath[1] = %s\n", pModGridFilePath->szGridFilePath[1]);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::GetIspost(void* pdata)
{
    TCPCommand* pTcp = (TCPCommand*)pdata;
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Ispostv2 get_ispost_mod: %u\n", pTcp->mod);
        LOGE("Ispostv2 get_ispost_cmd.ui3Type: %lu\n", pTcp->cmd.ui3Type);
    }

    switch (pTcp->mod - MODID_ISPOST_BASE) {
        case ISPOST_DN:
            iRet = GetDNParam(pdata);
            break;

        case ISPOST_VER:
            iRet = GetIspostVersion(pdata);
            break;

        case ISPOST_DBG:
            iRet = GetIqTuningDebugInfo(pdata);
            break;

        case ISPOST_GRID_PATH:
            iRet = GetGridFilePath(pdata);
            break;

        default:
            LOGE("Unknown module.\n");
            break;
    }

    return iRet;
}

int IPU_ISPOSTV2::SetIspost(void* pdata)
{
    TCPCommand* pTcp = (TCPCommand*)pdata;
    int iRet = -1;
    
    if (m_bIqTuningDebugInfoEnable) {
        LOGE("Ispostv2 set_ispost_mod: %u\n", pTcp->mod);
        LOGE("Ispostv2 set_ispost_cmd.ui3Type: %lu\n", pTcp->cmd.ui3Type);
    }

    switch (pTcp->mod - MODID_ISPOST_BASE) {
        case ISPOST_DN:
            iRet = SetDNParam(pdata);
            break;

        case ISPOST_VER:
            LOGE("Ispost does not support this command.\n");
            break;

        case ISPOST_DBG:
            iRet = SetIqTuningDebugInfo(pdata);
            break;

        case ISPOST_GRID_PATH:
            LOGE("Ispost does not support this command.\n");
            break;

        default:
            LOGE("Unknown module.\n");
            break;
    }

    return iRet;
}
