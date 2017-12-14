#include <System.h>
#include "V2500.h"
#include "ispc/ColorCorrection.h"
#include <ispc/TemperatureCorrection.h>
#include <ispc/ModuleBLC.h>
#include <qsdk/tcpcommand.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
//#include "ispc/ModuleLCA.h"
//#include "ispc/ModuleHIS.h"
//#include "ispc/ModuleRLT.h"
//#include "ispc/ModuleWBS.h"

using namespace std;

//#define V2500_IQ_TUNING_DEBUG       1


int IPU_V2500::GetBLCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_BLC* pModBLC = (ISP_MDLE_BLC*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleBLC *pIspBLC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleBLC>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModBLC || !pIspBLC) {
        LOGE("V2500 get BLC failed - Camera=%p, Sensor=%p, Tcp=%p, pModBLC=%p, pIspBLC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModBLC, pIspBLC);
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
            if ((ISP_BLC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_BLC_CMD_VER_V0:
                default:
                    pModBLC->iSensorBlack[0] = pIspBLC->aSensorBlack[0];
                    pModBLC->iSensorBlack[1] = pIspBLC->aSensorBlack[1];
                    pModBLC->iSensorBlack[2] = pIspBLC->aSensorBlack[2];
                    pModBLC->iSensorBlack[3] = pIspBLC->aSensorBlack[3];
                    pModBLC->uiSystemBlack = pIspBLC->ui32SystemBlack;
                    if (ISP_BLC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_BLC);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get BLC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get BLC.iSensorBlack[0]: %d\n", pModBLC->iSensorBlack[0]);
                        LOGE("V2500 get BLC.iSensorBlack[1]: %d\n", pModBLC->iSensorBlack[1]);
                        LOGE("V2500 get BLC.iSensorBlack[2]: %d\n", pModBLC->iSensorBlack[2]);
                        LOGE("V2500 get BLC.iSensorBlack[3]: %d\n", pModBLC->iSensorBlack[3]);
                        LOGE("V2500 get BLC.uiSystemBlack: %u\n", pModBLC->uiSystemBlack);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetBLCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_BLC* pModBLC = (ISP_MDLE_BLC*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleBLC *pIspBLC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleBLC>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModBLC || !pIspBLC) {
        LOGE("V2500 set BLC failed - Camera=%p, Sensor=%p, Tcp=%p, pModBLC=%p, pIspBLC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModBLC, pIspBLC);
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
            if ((ISP_BLC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_BLC_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set BLC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set BLC.iSensorBlack[0]: %d\n", pModBLC->iSensorBlack[0]);
                        LOGE("V2500 set BLC.iSensorBlack[1]: %d\n", pModBLC->iSensorBlack[1]);
                        LOGE("V2500 set BLC.iSensorBlack[2]: %d\n", pModBLC->iSensorBlack[2]);
                        LOGE("V2500 set BLC.iSensorBlack[3]: %d\n", pModBLC->iSensorBlack[3]);
                        LOGE("V2500 set BLC.uiSystemBlack: %u\n", pModBLC->uiSystemBlack);
                    }

                    pIspBLC->aSensorBlack[0] = pModBLC->iSensorBlack[0];
                    pIspBLC->aSensorBlack[1] = pModBLC->iSensorBlack[1];
                    pIspBLC->aSensorBlack[2] = pModBLC->iSensorBlack[2];
                    pIspBLC->aSensorBlack[3] = pModBLC->iSensorBlack[3];
                    pIspBLC->ui32SystemBlack = pModBLC->uiSystemBlack;
                    pIspBLC->requestUpdate();
                    pIspBLC->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetDNSParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DNS* pModDNS = (ISP_MDLE_DNS *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleDNS *pIspDNS = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModDNS || !pIspDNS) {
        LOGE("V2500 get DNS failed - Camera=%p, Sensor=%p, Tcp=%p, pModDNS=%p, pIspDNS=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModDNS, pIspDNS);
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
            if ((ISP_DNS_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DNS_CMD_VER_V0:
                default:
                    pModDNS->bCombine = pIspDNS->bCombine;
                    pModDNS->dStrength = pIspDNS->fStrength;
                    pModDNS->dGreyscaleThreshold = pIspDNS->fGreyscaleThreshold;
                    pModDNS->dSensorGain = pIspDNS->fSensorGain;
                    pModDNS->uiSensorBitdepth = pIspDNS->ui32SensorBitdepth;
                    pModDNS->uiSensorWellDepth = pIspDNS->ui32SensorWellDepth;
                    pModDNS->dSensorReadNoise = pIspDNS->fSensorReadNoise;
                    if (ISP_DNS_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_DNS);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get DNS.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get DNS.bCombine: %d\n", pModDNS->bCombine);
                        LOGE("V2500 get DNS.dStrength: %lf\n", pModDNS->dStrength);
                        LOGE("V2500 get DNS.dGreyscaleThreshold: %lf\n", pModDNS->dGreyscaleThreshold);
                        LOGE("V2500 get DNS.dSensorGain: %lf\n", pModDNS->dSensorGain);
                        LOGE("V2500 get DNS.uiSensorBitdepth: %u\n", pModDNS->uiSensorBitdepth);
                        LOGE("V2500 get DNS.uiSensorWellDepth: %u\n", pModDNS->uiSensorWellDepth);
                        LOGE("V2500 get DNS.dSensorReadNoise: %lf\n", pModDNS->dSensorReadNoise);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetDNSParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DNS* pModDNS = (ISP_MDLE_DNS *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleDNS *pIspDNS = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModDNS || !pIspDNS) {
        LOGE("V2500 set DNS failed - Camera=%p, Sensor=%p, Tcp=%p, pModDNS=%p, pIspDNS=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModDNS, pIspDNS);
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
            if ((ISP_DNS_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DNS_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set DNS.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set DNS.bCombine: %d\n", pModDNS->bCombine);
                        LOGE("V2500 set DNS.dStrength: %lf\n", pModDNS->dStrength);
                        LOGE("V2500 set DNS.dGreyscaleThreshold: %lf\n", pModDNS->dGreyscaleThreshold);
                        LOGE("V2500 set DNS.dSensorGain: %lf\n", pModDNS->dSensorGain);
                        LOGE("V2500 set DNS.uiSensorBitdepth: %u\n", pModDNS->uiSensorBitdepth);
                        LOGE("V2500 set DNS.uiSensorWellDepth: %u\n", pModDNS->uiSensorWellDepth);
                        LOGE("V2500 set DNS.dSensorReadNoise: %lf\n", pModDNS->dSensorReadNoise);
                    }

                    pIspDNS->bCombine = pModDNS->bCombine;
                    pIspDNS->fStrength = pModDNS->dStrength;
                    pIspDNS->fGreyscaleThreshold = pModDNS->dGreyscaleThreshold;
                    pIspDNS->fSensorGain = pModDNS->dSensorGain;
                    pIspDNS->ui32SensorBitdepth = pModDNS->uiSensorBitdepth;
                    pIspDNS->ui32SensorWellDepth = pModDNS->uiSensorWellDepth;
                    pIspDNS->fSensorReadNoise = pModDNS->dSensorReadNoise;
                    pIspDNS->requestUpdate();
                    pIspDNS->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetDPFParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DPF* pModDPF = (ISP_MDLE_DPF *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleDPF *pIspDPF = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModDPF || !pIspDPF) {
        LOGE("V2500 get DPF failed - Camera=%p, Sensor=%p, Tcp=%p, pModDPF=%p, pIspDPF=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModDPF, pIspDPF);
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
            if ((ISP_DPF_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DPF_CMD_VER_V0:
                default:
                    pModDPF->bDetect = pIspDPF->bDetect;
                    pModDPF->uiThreshold = pIspDPF->ui32Threshold;
                    pModDPF->dWeight = pIspDPF->fWeight;
                    pModDPF->uiNbDefects = pIspDPF->ui32NbDefects;
                    if (ISP_DPF_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_DPF);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get DPF.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get DPF.bDetect: %d\n", pModDPF->bDetect);
                        LOGE("V2500 get DPF.uiThreshold: %u\n", pModDPF->uiThreshold);
                        LOGE("V2500 get DPF.dWeight: %lf\n", pModDPF->dWeight);
                        LOGE("V2500 get DPF.uiNbDefects: %u\n", pModDPF->uiNbDefects);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetDPFParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DPF* pModDPF = (ISP_MDLE_DPF *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleDPF *pIspDPF = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModDPF || !pIspDPF) {
        LOGE("V2500 set DPF failed - Camera=%p, Sensor=%p, Tcp=%p, pModDPF=%p, pIspDPF=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModDPF, pIspDPF);
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
            if ((ISP_DPF_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DPF_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set DPF.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set DPF.bDetect: %d\n", pModDPF->bDetect);
                        LOGE("V2500 set DPF.uiThreshold: %u\n", pModDPF->uiThreshold);
                        LOGE("V2500 set DPF.dWeight: %lf\n", pModDPF->dWeight);
                        //LOGE("V2500 get DPF.uiNbDefects: %u\n", pModDPF->uiNbDefects);
                    }

                    pIspDPF->bDetect = pModDPF->bDetect;
                    pIspDPF->ui32Threshold = pModDPF->uiThreshold;
                    pIspDPF->fWeight = pModDPF->dWeight;
                    pIspDPF->ui32NbDefects = pModDPF->uiNbDefects;
                    pIspDPF->requestUpdate();
                    pIspDPF->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetGMAParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_GMA* pModGMA = (ISP_MDLE_GMA *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleGMA *pIspGMA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModGMA || !pIspGMA) {
        LOGE("V2500 get GMA failed - Camera=%p, Sensor=%p, Tcp=%p, pModGMA=%p, pIspGMA=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModGMA, pIspGMA);
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
            if ((ISP_GMA_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_GMA_CMD_VER_V0:
                default:
                    pModGMA->bBypass = pIspGMA->bBypass;
                    pModGMA->bUseCustomGma = pIspGMA->useCustomGam;
                    memcpy(
                        (void *)pModGMA->uiCustomGmaCurve,
                        (void *)pIspGMA->customGamCurve,
                        sizeof(pModGMA->uiCustomGmaCurve)
                        );
                    if (ISP_GMA_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_GMA);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get GMA.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get GMA.bBypass: %d\n", pModGMA->bBypass);
                        LOGE("V2500 get GMA.bUseCustomGma: %d\n", pModGMA->bUseCustomGma);
                        LOGE("V2500 get GMA.uiCustomGmaCurve[]:\n");
                        for (int iIdx = 0; iIdx < ISP_GMA_CURVE_PNT_CNT; iIdx++) {
                            if (((ISP_GMA_CURVE_PNT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%u\n", pModGMA->uiCustomGmaCurve[iIdx]);
                            } else {
                                LOGE("%u ", pModGMA->uiCustomGmaCurve[iIdx]);
                            }
                        }
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetGMAParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_GMA* pModGMA = (ISP_MDLE_GMA *)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleGMA *pIspGMA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    CI_MODULE_GMA_LUT glut;
    CI_CONNECTION *conn  = m_Cam[Idx].pCamera->getConnection();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModGMA || !pIspGMA) {
        LOGE("V2500 set GMA failed - Camera=%p, Sensor=%p, Tcp=%p, pModGMA=%p, pIspGMA=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModGMA, pIspGMA);
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
            if ((ISP_GMA_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_GMA_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set GMA.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set GMA.bBypass: %d\n", pModGMA->bBypass);
                        LOGE("V2500 set GMA.bUseCustomGma: %d\n", pModGMA->bUseCustomGma);
                        LOGE("V2500 set GMA.uiCustomGmaCurve[]:\n");
                        for (int iIdx = 0; iIdx < ISP_GMA_CURVE_PNT_CNT; iIdx++) {
                            if (((ISP_GMA_CURVE_PNT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%u\n", pModGMA->uiCustomGmaCurve[iIdx]);
                            } else {
                                LOGE("%u ", pModGMA->uiCustomGmaCurve[iIdx]);
                            }
                        }
                    }

                    pIspGMA->bBypass = pModGMA->bBypass;
                    pIspGMA->useCustomGam = pModGMA->bUseCustomGma;
                    memcpy(
                        (void *)pIspGMA->customGamCurve,
                        (void *)pModGMA->uiCustomGmaCurve,
                        sizeof(pModGMA->uiCustomGmaCurve)
                        );
                    memcpy(glut.aRedPoints, pIspGMA->customGamCurve, sizeof(pIspGMA->customGamCurve));
                    memcpy(glut.aGreenPoints, pIspGMA->customGamCurve, sizeof(pIspGMA->customGamCurve));
                    memcpy(glut.aBluePoints, pIspGMA->customGamCurve, sizeof(pIspGMA->customGamCurve));
                    if (CI_DriverSetGammaLUT(conn, &glut) != IMG_SUCCESS) {
                        LOGE("V2500 set GMA - CI_DriverSetGammaLUT() failed.\n");
                    }
                    pIspGMA->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetR2YParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_R2Y* pModR2Y = (ISP_MDLE_R2Y*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleR2Y *pIspR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModR2Y || !pIspR2Y) {
        LOGE("V2500 get R2Y failed - Camera=%p, Sensor=%p, Tcp=%p, pModR2Y=%p, pIspR2Y=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModR2Y, pIspR2Y);
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
            if ((ISP_R2Y_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_R2Y_CMD_VER_V0:
                default:
                    pModR2Y->dBrightness = pIspR2Y->fBrightness;
                    pModR2Y->dContrast = pIspR2Y->fContrast;
                    pModR2Y->dSaturation = pIspR2Y->fSaturation;
                    pModR2Y->dHue = pIspR2Y->fHue;
                    pModR2Y->dRangeMult[0] = pIspR2Y->aRangeMult[0];
                    pModR2Y->dRangeMult[1] = pIspR2Y->aRangeMult[1];
                    pModR2Y->dRangeMult[2] = pIspR2Y->aRangeMult[2];
                    pModR2Y->dOffsetU = pIspR2Y->fOffsetU;
                    pModR2Y->dOffsetV = pIspR2Y->fOffsetV;
                    pModR2Y->uiMatrix = (ISP_STD_MATRIX)pIspR2Y->eMatrix;
                    if (ISP_R2Y_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_R2Y);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get R2Y.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get R2Y.dBrightness: %lf\n", pModR2Y->dBrightness);
                        LOGE("V2500 get R2Y.dContrast: %lf\n", pModR2Y->dContrast);
                        LOGE("V2500 get R2Y.dSaturation: %lf\n", pModR2Y->dSaturation);
                        LOGE("V2500 get R2Y.dHue: %lf\n", pModR2Y->dHue);
                        LOGE("V2500 get R2Y.dRangeMult[0]: %lf\n", pModR2Y->dRangeMult[0]);
                        LOGE("V2500 get R2Y.dRangeMult[1]: %lf\n", pModR2Y->dRangeMult[1]);
                        LOGE("V2500 get R2Y.dRangeMult[2]: %lf\n", pModR2Y->dRangeMult[2]);
                        LOGE("V2500 get R2Y.dOffsetU: %lf\n", pModR2Y->dOffsetU);
                        LOGE("V2500 get R2Y.dOffsetV: %lf\n", pModR2Y->dOffsetV);
                        LOGE("V2500 get R2Y.eMatrix: %d\n", pModR2Y->uiMatrix);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetR2YParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_R2Y* pModR2Y = (ISP_MDLE_R2Y*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleR2Y *pIspR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModR2Y || !pIspR2Y) {
        LOGE("V2500 set R2Y failed - Camera=%p, Sensor=%p, Tcp=%p, pModR2Y=%p, pIspR2Y=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModR2Y, pIspR2Y);
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
            if ((ISP_R2Y_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_R2Y_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set R2Y.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set R2Y_dBrightness: %lf\n", pModR2Y->dBrightness);
                        LOGE("V2500 set R2Y_dContrast: %lf\n", pModR2Y->dContrast);
                        LOGE("V2500 set R2Y_dSaturation: %lf\n", pModR2Y->dSaturation);
                        LOGE("V2500 set R2Y_dHue: %lf\n", pModR2Y->dHue);
                        LOGE("V2500 set R2Y_dRangeMult[0]: %lf\n", pModR2Y->dRangeMult[0]);
                        LOGE("V2500 set R2Y_dRangeMult[1]: %lf\n", pModR2Y->dRangeMult[1]);
                        LOGE("V2500 set R2Y_dRangeMult[2]: %lf\n", pModR2Y->dRangeMult[2]);
                        LOGE("V2500 set R2Y_dOffsetU: %lf\n", pModR2Y->dOffsetU);
                        LOGE("V2500 set R2Y_dOffsetV: %lf\n", pModR2Y->dOffsetV);
                        LOGE("V2500 set R2Y.eMatrix: %d\n", pModR2Y->uiMatrix);
                    }

                    pIspR2Y->fBrightness = pModR2Y->dBrightness;
                    pIspR2Y->fContrast = pModR2Y->dContrast;
                    pIspR2Y->fSaturation = pModR2Y->dSaturation;
                    pIspR2Y->fHue = pModR2Y->dHue;
                    pIspR2Y->aRangeMult[0] = pModR2Y->dRangeMult[0];
                    pIspR2Y->aRangeMult[1] = pModR2Y->dRangeMult[1];
                    pIspR2Y->aRangeMult[2] = pModR2Y->dRangeMult[2];
                    pIspR2Y->fOffsetU = pModR2Y->dOffsetU;
                    pIspR2Y->fOffsetV = pModR2Y->dOffsetV;
                    pIspR2Y->eMatrix = (enum ISPC::ModuleR2Y::StdMatrix)pModR2Y->uiMatrix;
                    pIspR2Y->requestUpdate();
                    pIspR2Y->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetTNMParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_TNM* pModTNM = (ISP_MDLE_TNM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleTNM *pIspTNM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModTNM || !pIspTNM) {
        LOGE("V2500 get TNM failed - Camera=%p, Sensor=%p, Tcp=%p, pModTNM=%p, pIspTNM=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModTNM, pIspTNM);
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
            if ((ISP_TNM_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_TNM_CMD_VER_V0:
                default:
                    pModTNM->bBypass = pIspTNM->bBypass;
                    pModTNM->adInY[0] = pIspTNM->aInY[0];
                    pModTNM->adInY[1] = pIspTNM->aInY[1];
                    pModTNM->adOutY[0] = pIspTNM->aOutY[0];
                    pModTNM->adOutY[1] = pIspTNM->aOutY[1];
                    pModTNM->adOutC[0] = pIspTNM->aOutC[0];
                    pModTNM->adOutC[1] = pIspTNM->aOutC[1];
                    pModTNM->dWeightLocal = pIspTNM->fWeightLocal;
                    pModTNM->dWeightLine = pIspTNM->fWeightLine;
                    pModTNM->dFlatFactor = pIspTNM->fFlatFactor;
                    pModTNM->dFlatMin = pIspTNM->fFlatMin;
                    pModTNM->dColourConfidence = pIspTNM->fColourConfidence;
                    pModTNM->dColourSaturation = pIspTNM->fColourSaturation;
                    memcpy(
                        (void *)pModTNM->adCurve,
                        (void *)pIspTNM->aCurve,
                        sizeof(pModTNM->adCurve)
                        );
                    pModTNM->bStaticCurve = pIspTNM->bStaticCurve;
                    if (ISP_TNM_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_TNM);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get TNM.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get TNM.bBypass: %d\n", pModTNM->bBypass);
                        LOGE("V2500 get TNM.adInY[]: [%lf, %lf]\n", pModTNM->adInY[0], pModTNM->adInY[1]);
                        LOGE("V2500 get TNM.adOutY[]: [%lf, %lf]\n", pModTNM->adOutY[0], pModTNM->adOutY[1]);
                        LOGE("V2500 get TNM.adOutC[]: [%lf, %lf]\n", pModTNM->adOutC[0], pModTNM->adOutC[1]);
                        LOGE("V2500 get TNM.dWeightLocal: %lf\n", pModTNM->dWeightLocal);
                        LOGE("V2500 get TNM.dWeightLine: %lf\n", pModTNM->dWeightLine);
                        LOGE("V2500 get TNM.dFlatFactor: %lf\n", pModTNM->dFlatFactor);
                        LOGE("V2500 get TNM.dFlatMin: %lf\n", pModTNM->dFlatMin);
                        LOGE("V2500 get TNM.dColourConfidence: %lf\n", pModTNM->dColourConfidence);
                        LOGE("V2500 get TNM.dColourSaturation: %lf\n", pModTNM->dColourSaturation);
                        LOGE("V2500 get TNM.adCurve[]:\n");
                        for (int iIdx = 0; iIdx < ISP_TNM_CURVE_NPOINTS; iIdx++) {
                            if (((ISP_TNM_CURVE_NPOINTS - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%lf\n", pModTNM->adCurve[iIdx]);
                            } else {
                                LOGE("%lf ", pModTNM->adCurve[iIdx]);
                            }
                        }
                        LOGE("V2500 get TNM.bStaticCurve: %d\n", pModTNM->bStaticCurve);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetTNMParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_TNM* pModTNM = (ISP_MDLE_TNM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleTNM *pIspTNM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModTNM || !pIspTNM) {
        LOGE("V2500 set TNM failed - Camera=%p, Sensor=%p, Tcp=%p, pModTNM=%p, pIspTNM=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModTNM, pIspTNM);
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
            if ((ISP_TNM_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_TNM_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set TNM.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set TNM.bBypass: %d\n", pModTNM->bBypass);
                        LOGE("V2500 set TNM.adInY[]: [%lf, %lf]\n", pModTNM->adInY[0], pModTNM->adInY[1]);
                        LOGE("V2500 set TNM.adOutY[]: [%lf, %lf]\n", pModTNM->adOutY[0], pModTNM->adOutY[1]);
                        LOGE("V2500 set TNM.adOutC[]: [%lf, %lf]\n", pModTNM->adOutC[0], pModTNM->adOutC[1]);
                        LOGE("V2500 set TNM.dWeightLocal: %lf\n", pModTNM->dWeightLocal);
                        LOGE("V2500 set TNM.dWeightLine: %lf\n", pModTNM->dWeightLine);
                        LOGE("V2500 set TNM.dFlatFactor: %lf\n", pModTNM->dFlatFactor);
                        LOGE("V2500 set TNM.dFlatMin: %lf\n", pModTNM->dFlatMin);
                        LOGE("V2500 set TNM.dColourConfidence: %lf\n", pModTNM->dColourConfidence);
                        LOGE("V2500 set TNM.dColourSaturation: %lf\n", pModTNM->dColourSaturation);
                        LOGE("V2500 set TNM.adCurve[]:\n");
                        for (int iIdx = 0; iIdx < ISP_TNM_CURVE_NPOINTS; iIdx++) {
                            if (((ISP_TNM_CURVE_NPOINTS - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%lf\n", pModTNM->adCurve[iIdx]);
                            } else {
                                LOGE("%lf ", pModTNM->adCurve[iIdx]);
                            }
                        }
                        LOGE("V2500 set TNM.bStaticCurve: %d\n", pModTNM->bStaticCurve);
                    }

                    pIspTNM->bBypass = pModTNM->bBypass;
                    pIspTNM->aInY[0] = pModTNM->adInY[0];
                    pIspTNM->aInY[1] = pModTNM->adInY[1];
                    pIspTNM->aOutY[0] = pModTNM->adOutY[0];
                    pIspTNM->aOutY[1] = pModTNM->adOutY[1];
                    pIspTNM->aOutC[0] = pModTNM->adOutC[0];
                    pIspTNM->aOutC[1] = pModTNM->adOutC[1];
                    pIspTNM->fWeightLocal = pModTNM->dWeightLocal;
                    pIspTNM->fWeightLine = pModTNM->dWeightLine;
                    pIspTNM->fFlatFactor = pModTNM->dFlatFactor;
                    pIspTNM->fFlatMin = pModTNM->dFlatMin;
                    pIspTNM->fColourConfidence = pModTNM->dColourConfidence;
                    pIspTNM->fColourSaturation = pModTNM->dColourSaturation;
                    memcpy(
                        (void *)pIspTNM->aCurve,
                        (void *)pModTNM->adCurve,
                        sizeof(pModTNM->adCurve)
                        );
                    pIspTNM->bStaticCurve = pModTNM->bStaticCurve;
                    pIspTNM->requestUpdate();
                    pIspTNM->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetSHAParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_SHA* pModSHA = (ISP_MDLE_SHA*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleSHA *pIspSHA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModSHA || !pIspSHA) {
        LOGE("V2500 get SHA failed - Camera=%p, Sensor=%p, Tcp=%p, pModSHA=%p, pIspSHA=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModSHA, pIspSHA);
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
            if ((ISP_SHA_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_SHA_CMD_VER_V0:
                default:
                    pModSHA->dRadius = pIspSHA->fRadius;
                    pModSHA->dfStrength = pIspSHA->fStrength;
                    pModSHA->dThreshold = pIspSHA->fThreshold;
                    pModSHA->dDetail = pIspSHA->fDetail;
                    pModSHA->dEdgeScale = pIspSHA->fEdgeScale;
                    pModSHA->dEdgeOffset = pIspSHA->fEdgeOffset;
                    pModSHA->bBypassDenoise = pIspSHA->bBypassDenoise;
                    pModSHA->dDenoiseTau = pIspSHA->fDenoiseTau;
                    pModSHA->dDenoiseSigma = pIspSHA->fDenoiseSigma;
                    if (ISP_SHA_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_SHA);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get SHA.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get SHA.dRadius: %lf\n", pModSHA->dRadius);
                        LOGE("V2500 get SHA.dfStrength: %lf\n", pModSHA->dfStrength);
                        LOGE("V2500 get SHA.dThreshold: %lf\n", pModSHA->dThreshold);
                        LOGE("V2500 get SHA.dDetail: %lf\n", pModSHA->dDetail);
                        LOGE("V2500 get SHA.dEdgeScale: %lf\n", pModSHA->dEdgeScale);
                        LOGE("V2500 get SHA.dEdgeOffset: %lf\n", pModSHA->dEdgeOffset);
                        LOGE("V2500 get SHA.bBypassDenoise: %d\n", pModSHA->bBypassDenoise);
                        LOGE("V2500 get SHA.dDenoiseTau: %lf\n", pModSHA->dDenoiseTau);
                        LOGE("V2500 get SHA.dDenoiseSigma: %lf\n", pModSHA->dDenoiseSigma);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetSHAParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_SHA* pModSHA = (ISP_MDLE_SHA*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleSHA *pIspSHA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModSHA || !pIspSHA) {
        LOGE("V2500 set SHA failed - Camera=%p, Sensor=%p, Tcp=%p, pModSHA=%p, pIspSHA=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModSHA, pIspSHA);
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
            if ((ISP_SHA_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_SHA_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set SHA.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set SHA.dRadius: %lf\n", pModSHA->dRadius);
                        LOGE("V2500 set SHA.dfStrength: %lf\n", pModSHA->dfStrength);
                        LOGE("V2500 set SHA.dThreshold: %lf\n", pModSHA->dThreshold);
                        LOGE("V2500 set SHA.dDetail: %lf\n", pModSHA->dDetail);
                        LOGE("V2500 set SHA.dEdgeScale: %lf\n", pModSHA->dEdgeScale);
                        LOGE("V2500 set SHA.dEdgeOffset: %lf\n", pModSHA->dEdgeOffset);
                        LOGE("V2500 set SHA.bBypassDenoise: %d\n", pModSHA->bBypassDenoise);
                        LOGE("V2500 set SHA.dDenoiseTau: %lf\n", pModSHA->dDenoiseTau);
                        LOGE("V2500 set SHA.dDenoiseSigma: %lf\n", pModSHA->dDenoiseSigma);
                    }

                    pIspSHA->fRadius = pModSHA->dRadius;
                    pIspSHA->fStrength = pModSHA->dfStrength;
                    pIspSHA->fThreshold = pModSHA->dThreshold;
                    pIspSHA->fDetail = pModSHA->dDetail;
                    pIspSHA->fEdgeScale = pModSHA->dEdgeScale;
                    pIspSHA->fEdgeOffset = pModSHA->dEdgeOffset;
                    pIspSHA->bBypassDenoise = pModSHA->bBypassDenoise;
                    pIspSHA->fDenoiseTau = pModSHA->dDenoiseTau;
                    pIspSHA->fDenoiseSigma = pModSHA->dDenoiseSigma;
                    pIspSHA->requestUpdate();
                    pIspSHA->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetWBCAndCCMParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_WBC_CCM* pModWbcCcm = (ISP_MDLE_WBC_CCM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleCCM *pIspCCM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *pIspWBC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();
    ISPC::ControlAWB_PID *pIspAWB = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAWB_PID>();
    const ISPC::TemperatureCorrection & TempCorr = pIspAWB->getTemperatureCorrections();
    int iIdx;
    int iRow;
    int iCol;
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModWbcCcm || !pIspCCM || !pIspWBC || !pIspAWB) {
        LOGE("V2500 get WBC_CCM failed - Camera=%p, Sensor=%p, Tcp=%p, pModWbcCcm=%p, pIspCCM=%p, pIspWBC=%p, pIspAWB=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModWbcCcm, pIspCCM, pIspWBC, pIspAWB);
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
            if ((ISP_WBC_CCM_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_WBC_CCM_CMD_VER_V0:
                default:
                    pModWbcCcm->WBC.adWBGain[0] = pIspWBC->aWBGain[0];
                    pModWbcCcm->WBC.adWBGain[1] = pIspWBC->aWBGain[1];
                    pModWbcCcm->WBC.adWBGain[2] = pIspWBC->aWBGain[2];
                    pModWbcCcm->WBC.adWBGain[3] = pIspWBC->aWBGain[3];
                    pModWbcCcm->WBC.adWBClip[0] = pIspWBC->aWBClip[0];
                    pModWbcCcm->WBC.adWBClip[1] = pIspWBC->aWBClip[1];
                    pModWbcCcm->WBC.adWBClip[2] = pIspWBC->aWBClip[2];
                    pModWbcCcm->WBC.adWBClip[3] = pIspWBC->aWBClip[3];
                    pModWbcCcm->CCM.adMatrix[0] = pIspCCM->aMatrix[0];
                    pModWbcCcm->CCM.adMatrix[1] = pIspCCM->aMatrix[1];
                    pModWbcCcm->CCM.adMatrix[2] = pIspCCM->aMatrix[2];
                    pModWbcCcm->CCM.adMatrix[3] = pIspCCM->aMatrix[3];
                    pModWbcCcm->CCM.adMatrix[4] = pIspCCM->aMatrix[4];
                    pModWbcCcm->CCM.adMatrix[5] = pIspCCM->aMatrix[5];
                    pModWbcCcm->CCM.adMatrix[6] = pIspCCM->aMatrix[6];
                    pModWbcCcm->CCM.adMatrix[7] = pIspCCM->aMatrix[7];
                    pModWbcCcm->CCM.adMatrix[8] = pIspCCM->aMatrix[8];
                    pModWbcCcm->CCM.adOffset[0] = pIspCCM->aOffset[0];
                    pModWbcCcm->CCM.adOffset[1] = pIspCCM->aOffset[1];
                    pModWbcCcm->CCM.adOffset[2] = pIspCCM->aOffset[2];
                    pModWbcCcm->uiTemperatureCorrectionCount = TempCorr.size();
                    for (iIdx = 0; (unsigned int)iIdx < pModWbcCcm->uiTemperatureCorrectionCount; iIdx++) {
                        ISPC::ColorCorrection cc = TempCorr.getCorrection(iIdx);
                        pModWbcCcm->TC[iIdx].dTemperature = cc.temperature;
                        for (iRow = 0; iRow < 3; iRow++) {
                            for (iCol = 0; iCol < 3; iCol++) {
                                pModWbcCcm->TC[iIdx].dCoefficients[iRow][iCol] = cc.coefficients[iRow][iCol];
                            }
                        }
                        for (iRow = 0; iRow < 3; iRow++) {
                            pModWbcCcm->TC[iIdx].dOffsets[iRow] = cc.offsets[0][iRow];
                        }
                        for (iRow = 0; iRow < 4; iRow++) {
                            pModWbcCcm->TC[iIdx].dGains[iRow] = cc.gains[0][iRow];
                        }
                    }
                    if (ISP_WBC_CCM_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_WBC_CCM);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get WBC_CCM.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get WBC_CCM.WBC.adWBGain[0]: %lf\n", pModWbcCcm->WBC.adWBGain[0]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBGain[1]: %lf\n", pModWbcCcm->WBC.adWBGain[1]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBGain[2]: %lf\n", pModWbcCcm->WBC.adWBGain[2]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBGain[3]: %lf\n", pModWbcCcm->WBC.adWBGain[3]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBClip[0]: %lf\n", pModWbcCcm->WBC.adWBClip[0]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBClip[1]: %lf\n", pModWbcCcm->WBC.adWBClip[1]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBClip[2]: %lf\n", pModWbcCcm->WBC.adWBClip[2]);
                        LOGE("V2500 get WBC_CCM.WBC.adWBClip[3]: %lf\n", pModWbcCcm->WBC.adWBClip[3]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[0]: %lf\n", pModWbcCcm->CCM.adMatrix[0]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[1]: %lf\n", pModWbcCcm->CCM.adMatrix[1]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[2]: %lf\n", pModWbcCcm->CCM.adMatrix[2]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[3]: %lf\n", pModWbcCcm->CCM.adMatrix[3]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[4]: %lf\n", pModWbcCcm->CCM.adMatrix[4]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[5]: %lf\n", pModWbcCcm->CCM.adMatrix[5]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[6]: %lf\n", pModWbcCcm->CCM.adMatrix[6]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[7]: %lf\n", pModWbcCcm->CCM.adMatrix[7]);
                        LOGE("V2500 get WBC_CCM.CCM.adMatrix[8]: %lf\n", pModWbcCcm->CCM.adMatrix[8]);
                        LOGE("V2500 get WBC_CCM.CCM.adOffset[0]: %lf\n", pModWbcCcm->CCM.adOffset[0]);
                        LOGE("V2500 get WBC_CCM.CCM.adOffset[1]: %lf\n", pModWbcCcm->CCM.adOffset[1]);
                        LOGE("V2500 get WBC_CCM.CCM.adOffset[2]: %lf\n", pModWbcCcm->CCM.adOffset[2]);
                        LOGE("V2500 get WBC_CCM.uiTemperatureCorrectionCount: %u\n", pModWbcCcm->uiTemperatureCorrectionCount);
                        for (iIdx = 0; (unsigned int)iIdx < pModWbcCcm->uiTemperatureCorrectionCount; iIdx++) {
                            ISPC::ColorCorrection cc = TempCorr.getCorrection(iIdx);
                            pModWbcCcm->TC[iIdx].dTemperature = cc.temperature;
                            LOGE("----- %d -----\n", iIdx);
                            LOGE("V2500 get WBC_CCM.TC.dTemperature: %lf\n", pModWbcCcm->TC[iIdx].dTemperature);
                            LOGE("V2500 get WBC_CCM.TC.dCoefficients[]:\n");
                            for (iRow = 0; iRow < 3; iRow++) {
                                LOGE("[ %7.4lf %7.4lf %7.4lf ]\n",
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][0],
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][1],
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][2]
                                     );
                            }
                            LOGE("V2500 get WBC_CCM.TC.dOffsets[]: [ %7.4lf %7.4lf %7.4lf ]\n",
                                 pModWbcCcm->TC[iIdx].dOffsets[0],
                                 pModWbcCcm->TC[iIdx].dOffsets[1],
                                 pModWbcCcm->TC[iIdx].dOffsets[2]
                                 );
                            LOGE("V2500 get WBC_CCM.TC.dGains[]: [ %7.4lf %7.4lf %7.4lf %7.4lf ]\n",
                                 pModWbcCcm->TC[iIdx].dGains[0],
                                 pModWbcCcm->TC[iIdx].dGains[1],
                                 pModWbcCcm->TC[iIdx].dGains[2],
                                 pModWbcCcm->TC[iIdx].dGains[3]
                                 );
                        }
                        LOGE("-------------\n");
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetWBCAndCCMParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_WBC_CCM* pModWbcCcm = (ISP_MDLE_WBC_CCM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ModuleCCM *pIspCCM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *pIspWBC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();
    ISPC::ControlAWB_PID *pIspAWB = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAWB_PID>();
    ISPC::TemperatureCorrection & TempCorr = pIspAWB->getTemperatureCorrections();
    int iIdx;
    int iRow;
    int iCol;
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pModWbcCcm || !pIspCCM || !pIspWBC || !pIspAWB) {
        LOGE("V2500 set WBC_CCM failed - Camera=%p, Sensor=%p, Tcp=%p, pModWbcCcm=%p, pIspCCM=%p, pIspWBC=%p, pIspAWB=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pModWbcCcm, pIspCCM, pIspWBC, pIspAWB);
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
            if ((ISP_WBC_CCM_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_WBC_CCM_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set WBC_CCM.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set WBC_CCM.WBC.adWBGain[0]: %lf\n", pModWbcCcm->WBC.adWBGain[0]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBGain[1]: %lf\n", pModWbcCcm->WBC.adWBGain[1]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBGain[2]: %lf\n", pModWbcCcm->WBC.adWBGain[2]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBGain[3]: %lf\n", pModWbcCcm->WBC.adWBGain[3]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBClip[0]: %lf\n", pModWbcCcm->WBC.adWBClip[0]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBClip[1]: %lf\n", pModWbcCcm->WBC.adWBClip[1]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBClip[2]: %lf\n", pModWbcCcm->WBC.adWBClip[2]);
                        LOGE("V2500 set WBC_CCM.WBC.adWBClip[3]: %lf\n", pModWbcCcm->WBC.adWBClip[3]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[0]: %lf\n", pModWbcCcm->CCM.adMatrix[0]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[1]: %lf\n", pModWbcCcm->CCM.adMatrix[1]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[2]: %lf\n", pModWbcCcm->CCM.adMatrix[2]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[3]: %lf\n", pModWbcCcm->CCM.adMatrix[3]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[4]: %lf\n", pModWbcCcm->CCM.adMatrix[4]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[5]: %lf\n", pModWbcCcm->CCM.adMatrix[5]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[6]: %lf\n", pModWbcCcm->CCM.adMatrix[6]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[7]: %lf\n", pModWbcCcm->CCM.adMatrix[7]);
                        LOGE("V2500 set WBC_CCM.CCM.adMatrix[8]: %lf\n", pModWbcCcm->CCM.adMatrix[8]);
                        LOGE("V2500 set WBC_CCM.CCM.adOffset[0]: %lf\n", pModWbcCcm->CCM.adOffset[0]);
                        LOGE("V2500 set WBC_CCM.CCM.adOffset[1]: %lf\n", pModWbcCcm->CCM.adOffset[1]);
                        LOGE("V2500 set WBC_CCM.CCM.adOffset[2]: %lf\n", pModWbcCcm->CCM.adOffset[2]);
                        for (iIdx = 0; (unsigned int)iIdx < pModWbcCcm->uiTemperatureCorrectionCount; iIdx++) {
                            ISPC::ColorCorrection cc = TempCorr.getCorrection(iIdx);
                            pModWbcCcm->TC[iIdx].dTemperature = cc.temperature;
                            LOGE("----- %d -----\n", iIdx);
                            LOGE("V2500 get WBC_CCM.TC.dTemperature: %lf\n", pModWbcCcm->TC[iIdx].dTemperature);
                            LOGE("V2500 get WBC_CCM.TC.dCoefficients[]:\n");
                            for (iRow = 0; iRow < 3; iRow++) {
                                LOGE("[ %7.4lf %7.4lf %7.4lf ]\n",
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][0],
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][1],
                                     pModWbcCcm->TC[iIdx].dCoefficients[iRow][2]
                                     );
                            }
                            LOGE("V2500 get WBC_CCM.TC.dOffsets[]: [ %7.4lf %7.4lf %7.4lf ]\n",
                                 pModWbcCcm->TC[iIdx].dOffsets[0],
                                 pModWbcCcm->TC[iIdx].dOffsets[1],
                                 pModWbcCcm->TC[iIdx].dOffsets[2]
                                 );
                            LOGE("V2500 get WBC_CCM.TC.dGains[]: [ %7.4lf %7.4lf %7.4lf %7.4lf ]\n",
                                 pModWbcCcm->TC[iIdx].dGains[0],
                                 pModWbcCcm->TC[iIdx].dGains[1],
                                 pModWbcCcm->TC[iIdx].dGains[2],
                                 pModWbcCcm->TC[iIdx].dGains[3]
                                 );
                        }
                    }

                    pIspWBC->aWBGain[0] = pModWbcCcm->WBC.adWBGain[0];
                    pIspWBC->aWBGain[1] = pModWbcCcm->WBC.adWBGain[1];
                    pIspWBC->aWBGain[2] = pModWbcCcm->WBC.adWBGain[2];
                    pIspWBC->aWBGain[3] = pModWbcCcm->WBC.adWBGain[3];
                    pIspWBC->aWBClip[0] = pModWbcCcm->WBC.adWBClip[0];
                    pIspWBC->aWBClip[1] = pModWbcCcm->WBC.adWBClip[1];
                    pIspWBC->aWBClip[2] = pModWbcCcm->WBC.adWBClip[2];
                    pIspWBC->aWBClip[3] = pModWbcCcm->WBC.adWBClip[3];
                    pIspCCM->aMatrix[0] = pModWbcCcm->CCM.adMatrix[0];
                    pIspCCM->aMatrix[1] = pModWbcCcm->CCM.adMatrix[1];
                    pIspCCM->aMatrix[2] = pModWbcCcm->CCM.adMatrix[2];
                    pIspCCM->aMatrix[3] = pModWbcCcm->CCM.adMatrix[3];
                    pIspCCM->aMatrix[4] = pModWbcCcm->CCM.adMatrix[4];
                    pIspCCM->aMatrix[5] = pModWbcCcm->CCM.adMatrix[5];
                    pIspCCM->aMatrix[6] = pModWbcCcm->CCM.adMatrix[6];
                    pIspCCM->aMatrix[7] = pModWbcCcm->CCM.adMatrix[7];
                    pIspCCM->aMatrix[8] = pModWbcCcm->CCM.adMatrix[8];
                    pIspCCM->aOffset[0] = pModWbcCcm->CCM.adOffset[0];
                    pIspCCM->aOffset[1] = pModWbcCcm->CCM.adOffset[1];
                    pIspCCM->aOffset[2] = pModWbcCcm->CCM.adOffset[2];
                    for (iIdx = 0; (unsigned int)iIdx < pModWbcCcm->uiTemperatureCorrectionCount; iIdx++) {
                        ISPC::ColorCorrection cc;
                        cc.temperature = pModWbcCcm->TC[iIdx].dTemperature;
                        for (iRow = 0; iRow < 3; iRow++) {
                            for (iCol = 0; iCol < 3; iCol++) {
                                cc.coefficients[iRow][iCol] = pModWbcCcm->TC[iIdx].dCoefficients[iRow][iCol];
                            }
                        }
                        for (iRow = 0; iRow < 3; iRow++) {
                            cc.offsets[0][iRow] = pModWbcCcm->TC[iIdx].dOffsets[iRow];
                        }
                        for (iRow = 0; iRow < 4; iRow++) {
                            cc.gains[0][iRow] = pModWbcCcm->TC[iIdx].dGains[iRow];
                        }
                        TempCorr.setCorrection(iIdx, cc);
                    }
                    pIspCCM->requestUpdate();
                    pIspWBC->requestUpdate();
                    pIspCCM->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    pIspWBC->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetAECParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_AE* pCtrlAE = (ISP_CTRL_AE*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlAE *pIspAE = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAE>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlAE || !pIspAE) {
        LOGE("V2500 get AEC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlAE=%p, pIspAE=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlAE, pIspAE);
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
            if ((ISP_AEC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_AEC_CMD_VER_V0:
                default:
                    pCtrlAE->bEnable = pIspAE->isEnabled();
                    pCtrlAE->dCurrentBrightness = pIspAE->getCurrentBrightness();
                    pCtrlAE->dOriginalTargetBrightness = pIspAE->getOriTargetBrightness();
                    pCtrlAE->dTargetBrightness = pIspAE->getTargetBrightness();
                    pCtrlAE->dUpdateSpeed = pIspAE->getUpdateSpeed();
                    pCtrlAE->bFlickerRejection = pIspAE->getFlickerRejection();
                    pCtrlAE->bAutoFlickerRejection = pIspAE->getAutoFlickerRejection();
                    //pCtrlAE->dFlickerFreqConfig = pIspAE->getFlickerFreqConfig();   // It's only for V2505.
                    pCtrlAE->dFlickerFreq = pIspAE->getFlickerRejectionFrequency();
                    pCtrlAE->dTargetBracket = pIspAE->getTargetBracket();
                    //pCtrlAE->dMaxAeGain = pIspAE->getMaxAeGain();                   // It's only for V2505.
                    pCtrlAE->dMaxSensorGain = m_Cam[Idx].pCamera->sensor->getMaxGain();
                    //pCtrlAE->dTargetAeGain = pIspAE->getTargetAeGain();             // It's only for V2505.
                    //pCtrlAE->uiMaxAeExposure = pIspAE->getMaxAeExposure();          // It's only for V2505.
                    pCtrlAE->uiMaxSensorExposure = m_Cam[Idx].pCamera->sensor->getMaxExposure();
                    if (!pIspAE->isEnabled()) {
                        pCtrlAE->dGain = m_Cam[Idx].pCamera->sensor->getGain();
                    } else {
                        pCtrlAE->dGain = pIspAE->getNewGain();
                    }
                    if (!pIspAE->isEnabled()) {
                        pCtrlAE->uiExposure = m_Cam[Idx].pCamera->sensor->getExposure();
                    } else {
                        pCtrlAE->uiExposure = pIspAE->getNewExposure();
                    }
                    //pCtrlAE->bUseFixedAeGain = pIspAE->isFixedAeGain();             // It's only for V2505.
                    //pCtrlAE->dFixedAeGain = pIspAE->getFixedAeGain();               // It's only for V2505.
                    //pCtrlAE->bUseFixedAeExposure = pIspAE->isFixedAeExposure();     // It's only for V2505.
                    //pCtrlAE->uiFixedAeExposure = pIspAE->getFixedAeExposure();      // It's only for V2505.
                    pCtrlAE->bDoAE = pIspAE->IsAEenabled();
                    pIspAE->getRegionBrightness(&pCtrlAE->adRegionBrightness[0][0]);
                    pCtrlAE->dOverUnderExpDiff = pIspAE->getOverUnderExpDiff();
                    pCtrlAE->dOverexposeRatio = pIspAE->getRatioOverExp();
                    pCtrlAE->dMiddleOverexposeRatio = pIspAE->getRatioMidOverExp();
                    pCtrlAE->dUnderexposeRatio = pIspAE->getRatioUnderExp();
                    pCtrlAE->bAeTargetMoveEnable = pIspAE->IsAeTargetMoveEnable();
                    pCtrlAE->iAeTargetMoveMethod = pIspAE->getAeTargetMoveMethod();
                    pCtrlAE->dAeTargetMax = pIspAE->getAeTargetMax();
                    pCtrlAE->dAeTargetMin = pIspAE->getAeTargetMin();
                    pCtrlAE->dAeTargetGainNormalModeMaxLmt = pIspAE->getAeTargetGainNormalModeMaxLmt();
                    pCtrlAE->dAeTargetMoveStep = pIspAE->getAeTargetMoveStep();
                    pCtrlAE->dAeTargetMinLmt = pIspAE->getAeTargetMinLmt();
                    pCtrlAE->dAeTargetUpOverThreshold = pIspAE->getAeTargetUpOverThreshold();
                    pCtrlAE->dAeTargetUpUnderThreshold = pIspAE->getAeTargetUpUnderThreshold();
                    pCtrlAE->dAeTargetDnOverThreshold = pIspAE->getAeTargetDnOverThreshold();
                    pCtrlAE->dAeTargetDnUnderThreshold = pIspAE->getAeTargetDnUnderThreshold();
                    pCtrlAE->iAeExposureMethod = pIspAE->getAeExposureMethod();
                    pCtrlAE->dAeTargetLowluxGainEnter = pIspAE->getAeTargetLowluxGainEnter();
                    pCtrlAE->dAeTargetLowluxGainExit = pIspAE->getAeTargetLowluxGainExit();
                    pCtrlAE->iAeTargetLowluxExposureEnter = pIspAE->getAeTargetLowluxExposureEnter();
                    pCtrlAE->dAeTargetLowluxFPS = pIspAE->getAeTargetLowluxFPS();
                    pCtrlAE->dAeTargetNormalFPS = pIspAE->getAeTargetNormalFPS();
                    pCtrlAE->bAeTargetMaxFpsLockEnable = pIspAE->getAeTargetMaxFpsLockEnable();
                    pCtrlAE->iAeBrightnessMeteringMethod = pIspAE->getAeBrightnessMeteringMethod();
                    pCtrlAE->dAeRegionDuce = pIspAE->getAeRegionDuce();
                    if (ISP_AEC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_CTRL_AE);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get AEC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get AEC.bEnable: %d\n", pCtrlAE->bEnable);
                        LOGE("V2500 get AEC.dCurrentBrightness: %lf\n", pCtrlAE->dCurrentBrightness);
                        LOGE("V2500 get AEC.dOriginalTargetBrightness: %lf\n", pCtrlAE->dOriginalTargetBrightness);
                        LOGE("V2500 get AEC.dTargetBrightness: %lf\n", pCtrlAE->dTargetBrightness);
                        LOGE("V2500 get AEC.dUpdateSpeed: %lf\n", pCtrlAE->dUpdateSpeed);
                        LOGE("V2500 get AEC.bFlickerRejection: %d\n", pCtrlAE->bFlickerRejection);
                        LOGE("V2500 get AEC.bAutoFlickerRejection: %d\n", pCtrlAE->bAutoFlickerRejection);
                        //LOGE("V2500 get AEC.dFlickerFreqConfig: %lf\n", pCtrlAE->dFlickerFreqConfig);   // It's only for V2505.
                        LOGE("V2500 get AEC.dFlickerFreq: %lf\n", pCtrlAE->dFlickerFreq);
                        LOGE("V2500 get AEC.dTargetBracket: %lf\n", pCtrlAE->dTargetBracket);
                        //LOGE("V2500 get AEC.dMaxAeGain: %lf\n", pCtrlAE->dMaxAeGain);                   // It's only for V2505.
                        LOGE("V2500 get AEC.dMaxSensorGain: %lf\n", pCtrlAE->dMaxSensorGain);
                        //LOGE("V2500 get AEC.dTargetAeGain: %lf\n", pCtrlAE->dTargetAeGain);             // It's only for V2505.
                        //LOGE("V2500 get AEC.uiMaxAeExposure: %u\n", pCtrlAE->uiMaxAeExposure);          // It's only for V2505.
                        LOGE("V2500 get AEC.uiMaxSensorExposure: %u\n", pCtrlAE->uiMaxSensorExposure);
                        LOGE("V2500 get AEC.dGain: %lf\n", pCtrlAE->dGain);
                        LOGE("V2500 get AEC.uiExposure: %u\n", pCtrlAE->uiExposure);
                        //LOGE("V2500 get AEC.bUseFixedAeGain: %d\n", pCtrlAE->bUseFixedAeGain);          // It's only for V2505.
                        //LOGE("V2500 get AEC.dFixedAeGain: %lf\n", pCtrlAE->dFixedAeGain);               // It's only for V2505.
                        //LOGE("V2500 get AEC.bUseFixedAeExposure: %d\n", pCtrlAE->bUseFixedAeExposure);  // It's only for V2505.
                        //LOGE("V2500 get AEC.uiFixedAeExposure: %u\n", pCtrlAE->uiFixedAeExposure);      // It's only for V2505.
                        LOGE("V2500 get AEC.bDoAE: %d\n", pCtrlAE->bDoAE);
                        LOGE("V2500 get AEC.adRegionBrightness[]:\n");
                        for (int i = 0; i < HIS_REGION_VTILES; i++) {
                            LOGE("%5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf\n",
                                 pCtrlAE->adRegionBrightness[i][0],
                                 pCtrlAE->adRegionBrightness[i][1],
                                 pCtrlAE->adRegionBrightness[i][2],
                                 pCtrlAE->adRegionBrightness[i][3],
                                 pCtrlAE->adRegionBrightness[i][4],
                                 pCtrlAE->adRegionBrightness[i][5],
                                 pCtrlAE->adRegionBrightness[i][6]
                                 );
                        }
                        LOGE("V2500 get AEC.dOverUnderExpDiff: %lf\n", pCtrlAE->dOverUnderExpDiff);
                        LOGE("V2500 get AEC.dOverexposeRatio: %lf\n", pCtrlAE->dOverexposeRatio);
                        LOGE("V2500 get AEC.dMiddleOverexposeRatio: %lf\n", pCtrlAE->dMiddleOverexposeRatio);
                        LOGE("V2500 get AEC.dUnderexposeRatio: %lf\n", pCtrlAE->dUnderexposeRatio);
                        LOGE("V2500 get AEC.bAeTargetMoveEnable: %d\n", pCtrlAE->bAeTargetMoveEnable);
                        LOGE("V2500 get AEC.iAeTargetMoveMethod: %d\n", pCtrlAE->iAeTargetMoveMethod);
                        LOGE("V2500 get AEC.dAeTargetMax: %lf\n", pCtrlAE->dAeTargetMax);
                        LOGE("V2500 get AEC.dAeTargetMin: %lf\n", pCtrlAE->dAeTargetMin);
                        LOGE("V2500 get AEC.dAeTargetGainNormalModeMaxLmt: %lf\n", pCtrlAE->dAeTargetGainNormalModeMaxLmt);
                        LOGE("V2500 get AEC.dAeTargetMoveStep: %lf\n", pCtrlAE->dAeTargetMoveStep);
                        LOGE("V2500 get AEC.dAeTargetMinLmt: %lf\n", pCtrlAE->dAeTargetMinLmt);
                        LOGE("V2500 get AEC.dAeTargetUpOverThreshold: %lf\n", pCtrlAE->dAeTargetUpOverThreshold);
                        LOGE("V2500 get AEC.dAeTargetUpUnderThreshold: %lf\n", pCtrlAE->dAeTargetUpUnderThreshold);
                        LOGE("V2500 get AEC.dAeTargetDnOverThreshold: %lf\n", pCtrlAE->dAeTargetDnOverThreshold);
                        LOGE("V2500 get AEC.dAeTargetDnUnderThreshold: %lf\n", pCtrlAE->dAeTargetDnUnderThreshold);
                        LOGE("V2500 get AEC.iAeExposureMethod: %d\n", pCtrlAE->iAeExposureMethod);
                        LOGE("V2500 get AEC.dAeTargetLowluxGainEnter: %lf\n", pCtrlAE->dAeTargetLowluxGainEnter);
                        LOGE("V2500 get AEC.dAeTargetLowluxGainExit: %lf\n", pCtrlAE->dAeTargetLowluxGainExit);
                        LOGE("V2500 get AEC.iAeTargetLowluxExposureEnter: %d\n", pCtrlAE->iAeTargetLowluxExposureEnter);
                        LOGE("V2500 get AEC.dAeTargetLowluxFPS: %lf\n", pCtrlAE->dAeTargetLowluxFPS);
                        LOGE("V2500 get AEC.dAeTargetNormalFPS: %lf\n", pCtrlAE->dAeTargetNormalFPS);
                        LOGE("V2500 get AEC.bAeTargetMaxFpsLockEnable: %d\n", pCtrlAE->bAeTargetMaxFpsLockEnable);
                        LOGE("V2500 get AEC.iAeBrightnessMeteringMethod: %d\n", pCtrlAE->iAeBrightnessMeteringMethod);
                        LOGE("V2500 get AEC.dAeRegionDuce: %lf\n", pCtrlAE->dAeRegionDuce);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetAECParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_AE* pCtrlAE = (ISP_CTRL_AE*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlAE *pIspAE = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAE>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlAE || !pIspAE) {
        LOGE("V2500 set AEC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlAE=%p, pIspAE=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlAE, pIspAE);
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
            if ((ISP_AEC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_AEC_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set AEC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set AEC.bEnable: %d\n", pCtrlAE->bEnable);
                        //LOGE("V2500 set AEC.dCurrentBrightness: %lf\n", pCtrlAE->dCurrentBrightness);   // It's read only.
                        LOGE("V2500 set AEC.dOriginalTargetBrightness: %lf\n", pCtrlAE->dOriginalTargetBrightness);
                        LOGE("V2500 set AEC.dTargetBrightness: %lf\n", pCtrlAE->dTargetBrightness);
                        LOGE("V2500 set AEC.dUpdateSpeed: %lf\n", pCtrlAE->dUpdateSpeed);
                        LOGE("V2500 set AEC.bFlickerRejection: %d\n", pCtrlAE->bFlickerRejection);
                        LOGE("V2500 set AEC.bAutoFlickerRejection: %d\n", pCtrlAE->bAutoFlickerRejection);
                        //LOGE("V2500 set AEC.dFlickerFreqConfig: %lf\n", pCtrlAE->dFlickerFreqConfig);   // It's only for V2505.
                        LOGE("V2500 set AEC.dFlickerFreq: %lf\n", pCtrlAE->dFlickerFreq);
                        LOGE("V2500 set AEC.dTargetBracket: %lf\n", pCtrlAE->dTargetBracket);
                        //LOGE("V2500 set AEC.dMaxAeGain: %lf\n", pCtrlAE->dMaxAeGain);                   // It's only for V2505.
                        LOGE("V2500 set AEC.dMaxSensorGain: %lf\n", pCtrlAE->dMaxSensorGain);
                        //LOGE("V2500 set AEC.dTargetAeGain: %lf\n", pCtrlAE->dTargetAeGain);             // It's only for V2505.
                        //LOGE("V2500 set AEC.uiMaxAeExposure: %u\n", pCtrlAE->uiMaxAeExposure);          // It's only for V2505.
                        LOGE("V2500 set AEC.uiMaxSensorExposure: %u\n", pCtrlAE->uiMaxSensorExposure);
                        LOGE("V2500 set AEC.dGain: %lf\n", pCtrlAE->dGain);
                        LOGE("V2500 set AEC.uiExposure: %u\n", pCtrlAE->uiExposure);
                        //LOGE("V2500 set AEC.bUseFixedAeGain: %d\n", pCtrlAE->bUseFixedAeGain);          // It's only for V2505.
                        //LOGE("V2500 set AEC.dFixedAeGain: %lf\n", pCtrlAE->dFixedAeGain);               // It's only for V2505.
                        //LOGE("V2500 set AEC.bUseFixedAeExposure: %d\n", pCtrlAE->bUseFixedAeExposure);  // It's only for V2505.
                        //LOGE("V2500 set AEC.uiFixedAeExposure: %u\n", pCtrlAE->uiFixedAeExposure);      // It's only for V2505.
                        LOGE("V2500 set AEC.bDoAE: %d\n", pCtrlAE->bDoAE);
                        //LOGE("V2500 set AEC.adRegionBrightness[]:\n");                                  // It's read only.
                        //for (int i = 0; i < HIS_REGION_VTILES; i++) {
                        //    LOGE("%5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf\n",
                        //         pCtrlAE->adRegionBrightness[i][0],
                        //         pCtrlAE->adRegionBrightness[i][1],
                        //         pCtrlAE->adRegionBrightness[i][2],
                        //         pCtrlAE->adRegionBrightness[i][3],
                        //         pCtrlAE->adRegionBrightness[i][4],
                        //         pCtrlAE->adRegionBrightness[i][5],
                        //         pCtrlAE->adRegionBrightness[i][6]
                        //         );
                        //}
                        //LOGE("V2500 set AEC.dOverUnderExpDiff: %lf\n", pCtrlAE->dOverUnderExpDiff);     // It's read only.
                        //LOGE("V2500 set AEC.dOverexposeRatio: %lf\n", pCtrlAE->dOverexposeRatio);       // It's read only.
                        //LOGE("V2500 set AEC.dMiddleOverexposeRatio: %lf\n", pCtrlAE->dMiddleOverexposeRatio);   // It's read only.
                        //LOGE("V2500 set AEC.dUnderexposeRatio: %lf\n", pCtrlAE->dUnderexposeRatio);     // It's read only.
                        LOGE("V2500 set AEC.bAeTargetMoveEnable: %d\n", pCtrlAE->bAeTargetMoveEnable);
                        LOGE("V2500 set AEC.iAeTargetMoveMethod: %d\n", pCtrlAE->iAeTargetMoveMethod);
                        LOGE("V2500 set AEC.dAeTargetMax: %lf\n", pCtrlAE->dAeTargetMax);
                        LOGE("V2500 set AEC.dAeTargetMin: %lf\n", pCtrlAE->dAeTargetMin);
                        LOGE("V2500 set AEC.dAeTargetGainNormalModeMaxLmt: %lf\n", pCtrlAE->dAeTargetGainNormalModeMaxLmt);
                        LOGE("V2500 set AEC.dAeTargetMoveStep: %lf\n", pCtrlAE->dAeTargetMoveStep);
                        LOGE("V2500 set AEC.dAeTargetMinLmt: %lf\n", pCtrlAE->dAeTargetMinLmt);
                        LOGE("V2500 set AEC.dAeTargetUpOverThreshold: %lf\n", pCtrlAE->dAeTargetUpOverThreshold);
                        LOGE("V2500 set AEC.dAeTargetUpUnderThreshold: %lf\n", pCtrlAE->dAeTargetUpUnderThreshold);
                        LOGE("V2500 set AEC.dAeTargetDnOverThreshold: %lf\n", pCtrlAE->dAeTargetDnOverThreshold);
                        LOGE("V2500 set AEC.dAeTargetDnUnderThreshold: %lf\n", pCtrlAE->dAeTargetDnUnderThreshold);
                        LOGE("V2500 set AEC.iAeExposureMethod: %d\n", pCtrlAE->iAeExposureMethod);
                        LOGE("V2500 set AEC.dAeTargetLowluxGainEnter: %lf\n", pCtrlAE->dAeTargetLowluxGainEnter);
                        LOGE("V2500 set AEC.dAeTargetLowluxGainExit: %lf\n", pCtrlAE->dAeTargetLowluxGainExit);
                        LOGE("V2500 set AEC.iAeTargetLowluxExposureEnter: %d\n", pCtrlAE->iAeTargetLowluxExposureEnter);
                        LOGE("V2500 set AEC.dAeTargetLowluxFPS: %lf\n", pCtrlAE->dAeTargetLowluxFPS);
                        LOGE("V2500 set AEC.dAeTargetNormalFPS: %lf\n", pCtrlAE->dAeTargetNormalFPS);
                        LOGE("V2500 set AEC.bAeTargetMaxFpsLockEnable: %d\n", pCtrlAE->bAeTargetMaxFpsLockEnable);
                        LOGE("V2500 set AEC.iAeBrightnessMeteringMethod: %d\n", pCtrlAE->iAeBrightnessMeteringMethod);
                        LOGE("V2500 set AEC.dAeRegionDuce: %lf\n", pCtrlAE->dAeRegionDuce);
                    }

                    pIspAE->enableControl(pCtrlAE->bEnable);
                    //pIspAE->setCurrentBrightness(pCtrlAE->dCurrentBrightness);    // It's read only.
                    pIspAE->setOriTargetBrightness(pCtrlAE->dOriginalTargetBrightness);
                    pIspAE->setTargetBrightness(pCtrlAE->dTargetBrightness);
                    pIspAE->setUpdateSpeed(pCtrlAE->dUpdateSpeed);
                    pIspAE->enableFlickerRejection(pCtrlAE->bFlickerRejection);
                    pIspAE->enableAutoFlickerRejection(pCtrlAE->bAutoFlickerRejection);
                    //pIspAE->enableFlickerRejection(pCtrlAE->bFlickerRejection, pCtrlAE->dFlickerFreqConfig);    // It's only for V2505.
                    pIspAE->setAntiflickerFreq(pCtrlAE->dFlickerFreq);
                    pIspAE->setTargetBracket(pCtrlAE->dTargetBracket);
                    //pIspAE->setMaxAeGain(pCtrlAE->dMaxAeGain);                      // It's only for V2505.
                    m_Cam[Idx].pCamera->sensor->setMaxGain(pCtrlAE->dMaxSensorGain);
                    //pIspAE->setTargetAeGain(pCtrlAE->dTargetAeGain);                // It's only for V2505.
                    //pIspAE->setMaxAeExposure((ISPC::ControlAE::microseconds_t)pCtrlAE->uiMaxAeExposure);     // It's only for V2505.
                    //m_Cam[Idx].pCamera->sensor->setMaxExposure(pCtrlAE->uiMaxSensorExposure);
                    if (!pIspAE->isEnabled()) {
                        m_Cam[Idx].pCamera->sensor->setGain(pCtrlAE->dGain);
                    } else {
                        //pIspAE->setNewGain(pCtrlAE->dGain);                         // It's read only.
                    }
                    if (!pIspAE->isEnabled()) {
                        m_Cam[Idx].pCamera->sensor->setExposure(pCtrlAE->uiExposure);
                    } else {
                        //pIspAE->setNewExposure(pCtrlAE->uiExposure);                // It's read only.
                    }
                    //pIspAE->setFixedAeGain(pCtrlAE->bUseFixedAeGain);               // It's only for V2505.
                    //pIspAE->setFixedAeGain(pCtrlAE->dFixedAeGain);                  // It's only for V2505.
                    //pIspAE->enableFixedAeExposure(pCtrlAE->bUseFixedAeExposure);    // It's only for V2505.
                    //pIspAE->setFixedAeExposure((ISPC::ControlAE::microseconds_t)pCtrlAE->uiFixedAeExposure);    // It's only for V2505.
                    pIspAE->enableAE(pCtrlAE->bDoAE);
                    //pIspAE->setRegionBrightness(&pCtrlAE->adRegionBrightness[0][0]);// It's read only.
                    //pIspAE->setOverUnderExpDiff(pCtrlAE->dOverUnderExpDiff);        // It's read only.
                    //pIspAE->setRatioOverExp(pCtrlAE->dOverexposeRatio);             // It's read only.
                    //pIspAE->setRatioMidOverExp(pCtrlAE->dMiddleOverexposeRatio);    // It's read only.
                    //pIspAE->setRatioUnderExp(pCtrlAE->dUnderexposeRatio);           // It's read only.
                    pIspAE->AeTargetMoveEnable(pCtrlAE->bAeTargetMoveEnable);
                    pIspAE->setAeTargetMoveMethod(pCtrlAE->iAeTargetMoveMethod);
                    pIspAE->setAeTargetMax(pCtrlAE->dAeTargetMax);
                    pIspAE->setAeTargetMin(pCtrlAE->dAeTargetMin);
                    pIspAE->setAeTargetGainNormalModeMaxLmt(pCtrlAE->dAeTargetGainNormalModeMaxLmt);
                    pIspAE->setAeTargetMoveStep(pCtrlAE->dAeTargetMoveStep);
                    pIspAE->setAeTargetMinLmt(pCtrlAE->dAeTargetMinLmt);
                    pIspAE->setAeTargetUpOverThreshold(pCtrlAE->dAeTargetUpOverThreshold);
                    pIspAE->setAeTargetUpUnderThreshold(pCtrlAE->dAeTargetUpUnderThreshold);
                    pIspAE->setAeTargetDnOverThreshold(pCtrlAE->dAeTargetDnOverThreshold);
                    pIspAE->setAeTargetDnUnderThreshold(pCtrlAE->dAeTargetDnUnderThreshold);
                    pIspAE->setAeExposureMethod(pCtrlAE->iAeExposureMethod);
                    pIspAE->setAeTargetLowluxGainEnter(pCtrlAE->dAeTargetLowluxGainEnter);
                    pIspAE->setAeTargetLowluxGainExit(pCtrlAE->dAeTargetLowluxGainExit);
                    pIspAE->setAeTargetLowluxExposureEnter(pCtrlAE->iAeTargetLowluxExposureEnter);
                    pIspAE->setAeTargetLowluxFPS(pCtrlAE->dAeTargetLowluxFPS);
                    pIspAE->setAeTargetNormalFPS(pCtrlAE->dAeTargetNormalFPS);
                    pIspAE->setAeTargetMaxFpsLockEnable(pCtrlAE->bAeTargetMaxFpsLockEnable);
                    pIspAE->setAeBrightnessMeteringMethod(pCtrlAE->iAeBrightnessMeteringMethod);
                    pIspAE->setAeRegionDuce(pCtrlAE->dAeRegionDuce);
                    pIspAE->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetAWBParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_AWB_PID* pCtrlAWB = (ISP_CTRL_AWB_PID*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlAWB_PID *pIspAWB = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAWB_PID>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlAWB || !pIspAWB) {
        LOGE("V2500 get AWB failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlAWB=%p, pIspAWB=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlAWB, pIspAWB);
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
            if ((ISP_AWB_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_AWB_CMD_VER_V0:
                default:
                    pCtrlAWB->bEnable = pIspAWB->isEnabled();
                    pCtrlAWB->bDoAwb = pIspAWB->IsAWBenabled();
                    pCtrlAWB->uiCorrectionMode = (ISP_CORRECTION_TYPES)pIspAWB->getCorrectionMode();
                    pCtrlAWB->dRGain = pIspAWB->getRGain();
                    pCtrlAWB->dBGain = pIspAWB->getBGain();
                    //pCtrlAWB->dTemperature = pIspAWB->getCorrectionTemperature();
                    pCtrlAWB->dTemperature = pIspAWB->getMeasuredTemperature();
                    pCtrlAWB->bSwAwbEnable = pIspAWB->bSwAwbEnable;
                    pCtrlAWB->bUseOriginalCCM = pIspAWB->bUseOriginalCCM;
                    pCtrlAWB->dImgRGain = pIspAWB->dbImg_rgain;
                    pCtrlAWB->dImgBGain = pIspAWB->dbImg_bgain;
                    pCtrlAWB->dSwRGain = pIspAWB->dbSw_rgain;
                    pCtrlAWB->dSwBGain = pIspAWB->dbSw_bgain;
                    pCtrlAWB->dSwAwbUseOriCcmNrmlGainLmt = pIspAWB->dbSwAwbUseOriCcmNrmlGainLmt;
                    pCtrlAWB->dSwAwbUseOriCcmNrmlBrightnessLmt = pIspAWB->dbSwAwbUseOriCcmNrmlBrightnessLmt;
                    pCtrlAWB->dSwAwbUseOriCcmNrmlUnderExposeLmt = pIspAWB->dbSwAwbUseOriCcmNrmlUnderExposeLmt;
                    pCtrlAWB->dSwAwbUseStdCcmLowLuxGainLmt = pIspAWB->dbSwAwbUseStdCcmLowLuxGainLmt;
                    pCtrlAWB->dSwAwbUseStdCcmDarkGainLmt = pIspAWB->dbSwAwbUseStdCcmDarkGainLmt;
                    pCtrlAWB->dSwAwbUseStdCcmLowLuxBrightnessLmt = pIspAWB->dbSwAwbUseStdCcmLowLuxBrightnessLmt;
                    pCtrlAWB->dSwAwbUseStdCcmLowLuxUnderExposeLmt = pIspAWB->dbSwAwbUseStdCcmLowLuxUnderExposeLmt;
                    pCtrlAWB->dSwAwbUseStdCcmDarkBrightnessLmt = pIspAWB->dbSwAwbUseStdCcmDarkBrightnessLmt;
                    pCtrlAWB->dSwAwbUseStdCcmDarkUnderExposeLmt = pIspAWB->dbSwAwbUseStdCcmDarkUnderExposeLmt;
                    //pCtrlAWB->bHwAwbEnable = pIspAWB->bHwAwbEnable;             // It's only for V2505.
                    //pCtrlAWB->uiHwAwbMethod = pIspAWB->ui32HwAwbMethod;         // It's only for V2505.
                    //pCtrlAWB->uiHwAwbFirstPixel = pIspAWB->ui32HwAwbFirstPixel; // It's only for V2505.
                    //memcpy(                                                     // It's read only and only for V2505.
                    //    (void *)pCtrlAWB->stHwAwbStatistic,
                    //    (void *)pIspAWB->stHwAwbStat,
                    //    sizeof(pCtrlAWB->stHwAwbStatistic)
                    //    );
                    pCtrlAWB->dHwSwAwbUpdateSpeed = pIspAWB->dbHwSwAwbUpdateSpeed;
                    pCtrlAWB->dHwSwAwbDropRatio = pIspAWB->dbHwSwAwbDropRatio;
                    memcpy(
                        (void *)pCtrlAWB->auiHwSwAwbWeightingTable,
                        (void *)pIspAWB->uiHwSwAwbWeightingTable,
                        sizeof(pCtrlAWB->auiHwSwAwbWeightingTable)
                        );
                    pCtrlAWB->dRGainMin = pIspAWB->fRGainMin;
                    pCtrlAWB->dRGainMax = pIspAWB->fRGainMax;
                    pCtrlAWB->dBGainMin = pIspAWB->fBGainMin;
                    pCtrlAWB->dBGainMax = pIspAWB->fBGainMax;
                    pCtrlAWB->uiFramesToSkip = pIspAWB->getNumberToSkip();
                    pCtrlAWB->uiFramesSkipped = pIspAWB->getNumberOfSkipped();
                    pCtrlAWB->bResetStates = pIspAWB->getResetStates();
                    pCtrlAWB->dMargin = pIspAWB->getMargin();
                    pCtrlAWB->dPidKP = pIspAWB->getPidKP();
                    pCtrlAWB->dPidKD = pIspAWB->getPidKD();
                    pCtrlAWB->dPidKI = pIspAWB->getPidKI();
                    if (ISP_AWB_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_CTRL_AWB_PID);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get AWB.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get AWB.bEnable: %d\n", pCtrlAWB->bEnable);
                        LOGE("V2500 get AWB.bDoAwb: %d\n", pCtrlAWB->bDoAwb);
                        LOGE("V2500 get AWB.uiCorrectionMode: %d\n", pCtrlAWB->uiCorrectionMode);
                        LOGE("V2500 get AWB.dRGain: %lf\n", pCtrlAWB->dRGain);
                        LOGE("V2500 get AWB.dBGain: %lf\n", pCtrlAWB->dBGain);
                        LOGE("V2500 get AWB.dTemperature: %lf\n", pCtrlAWB->dTemperature);
                        LOGE("V2500 get AWB.bSwAwbEnable: %d\n", pCtrlAWB->bSwAwbEnable);
                        LOGE("V2500 get AWB.bUseOriginalCCM: %d\n", pCtrlAWB->bUseOriginalCCM);
                        LOGE("V2500 get AWB.dImgRGain: %lf\n", pCtrlAWB->dImgRGain);
                        LOGE("V2500 get AWB.dImgBGain: %lf\n", pCtrlAWB->dImgBGain);
                        LOGE("V2500 get AWB.dSwRGain: %lf\n", pCtrlAWB->dSwRGain);
                        LOGE("V2500 get AWB.dSwBGain: %lf\n", pCtrlAWB->dSwBGain);
                        LOGE("V2500 get AWB.dSwAwbUseOriCcmNrmlGainLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlGainLmt);
                        LOGE("V2500 get AWB.dSwAwbUseOriCcmNrmlBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlBrightnessLmt);
                        LOGE("V2500 get AWB.dSwAwbUseOriCcmNrmlUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlUnderExposeLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmLowLuxGainLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxGainLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmDarkGainLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkGainLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmLowLuxBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxBrightnessLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmLowLuxUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxUnderExposeLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmDarkBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkBrightnessLmt);
                        LOGE("V2500 get AWB.dSwAwbUseStdCcmDarkUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkUnderExposeLmt);
                        //LOGE("V2500 get AWB.bHwAwbEnable: %d\n", pCtrlAWB->bHwAwbEnable);           // It's only for V2505.
                        //LOGE("V2500 get AWB.uiHwAwbMethod: %d\n", pCtrlAWB->uiHwAwbMethod);         // It's only for V2505.
                        //LOGE("V2500 get AWB.uiHwAwbFirstPixel: %d\n", pCtrlAWB->uiHwAwbFirstPixel); // It's only for V2505.
                        //LOGE("V2500 get AWB.stHwAwbStatistic[]:\n");                                // It's read only and only for V2505.
                        //for (int i = 0; i < 16; i++) {
                        //    for (int j = 0; j < 16; j += 4) {
                        //        LOGE("%5.3f %5.3f %4d, %5.3f %5.3f %4d, %5.3f %5.3f %4d, %5.3f %5.3f %4d\n",
                        //             pCtrlAWB->stHwAwbStatistic[i][j].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].uiQualCnt
                        //            );
                        //    }
                        //}
                        LOGE("V2500 get AWB.dHwSwAwbUpdateSpeed: %lf\n", pCtrlAWB->dHwSwAwbUpdateSpeed);
                        LOGE("V2500 get AWB.dHwSwAwbDropRatio: %lf\n", pCtrlAWB->dHwSwAwbDropRatio);
                        LOGE("V2500 get AWB.auiHwSwAwbWeightingTable[]:\n");
                        for (int iBIdx = 0; iBIdx < 32; iBIdx++) {
                            //LOGE("HW_SW_AWB_WEIGHTING_TBL_%02d:", iBIdx);
                            for (int iRIdx = 0; iRIdx < 32; iRIdx++) {
                                if ((32 - 1) == iRIdx) {
                                    LOGE("%2d\n", pCtrlAWB->auiHwSwAwbWeightingTable[iBIdx][iRIdx]);
                                } else {
                                    LOGE("%2d ", pCtrlAWB->auiHwSwAwbWeightingTable[iBIdx][iRIdx]);
                                }
                            }
                        }
                        LOGE("V2500 get AWB.dRGainMin: %lf\n", pCtrlAWB->dRGainMin);
                        LOGE("V2500 get AWB.dRGainMax: %lf\n", pCtrlAWB->dRGainMax);
                        LOGE("V2500 get AWB.dBGainMin: %lf\n", pCtrlAWB->dBGainMin);
                        LOGE("V2500 get AWB.dBGainMax: %lf\n", pCtrlAWB->dBGainMax);
                        LOGE("V2500 get AWB.uiFramesToSkip: %d\n", pCtrlAWB->uiFramesToSkip);
                        LOGE("V2500 get AWB.uiFramesSkipped: %d\n", pCtrlAWB->uiFramesSkipped);
                        LOGE("V2500 get AWB.bResetStates: %d\n", pCtrlAWB->bResetStates);
                        LOGE("V2500 get AWB.dMargin: %lf\n", pCtrlAWB->dMargin);
                        LOGE("V2500 get AWB.dPidKP: %lf\n", pCtrlAWB->dPidKP);
                        LOGE("V2500 get AWB.dPidKD: %lf\n", pCtrlAWB->dPidKD);
                        LOGE("V2500 get AWB.dPidKI: %lf\n", pCtrlAWB->dPidKI);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetAWBParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_AWB_PID* pCtrlAWB = (ISP_CTRL_AWB_PID*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlAWB_PID *pIspAWB = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAWB_PID>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlAWB || !pIspAWB) {
        LOGE("V2500 set AWB failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlAWB=%p, pIspAWB=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlAWB, pIspAWB);
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
            if ((ISP_AWB_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_AWB_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set AWB.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set AWB.bEnable: %d\n", pCtrlAWB->bEnable);
                        LOGE("V2500 set AWB.bDoAwb: %d\n", pCtrlAWB->bDoAwb);
                        LOGE("V2500 set AWB.uiCorrectionMode: %d\n", pCtrlAWB->uiCorrectionMode);
                        LOGE("V2500 set AWB.dRGain: %lf\n", pCtrlAWB->dRGain);
                        LOGE("V2500 set AWB.dBGain: %lf\n", pCtrlAWB->dBGain);
                        LOGE("V2500 set AWB.dTemperature: %lf\n", pCtrlAWB->dTemperature);
                        LOGE("V2500 set AWB.bSwAwbEnable: %d\n", pCtrlAWB->bSwAwbEnable);
                        LOGE("V2500 set AWB.bUseOriginalCCM: %d\n", pCtrlAWB->bUseOriginalCCM);
                        LOGE("V2500 set AWB.dImgRGain: %lf\n", pCtrlAWB->dImgRGain);
                        LOGE("V2500 set AWB.dImgBGain: %lf\n", pCtrlAWB->dImgBGain);
                        LOGE("V2500 set AWB.dSwRGain: %lf\n", pCtrlAWB->dSwRGain);
                        LOGE("V2500 set AWB.dSwBGain: %lf\n", pCtrlAWB->dSwBGain);
                        LOGE("V2500 set AWB.dSwAwbUseOriCcmNrmlGainLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlGainLmt);
                        LOGE("V2500 set AWB.dSwAwbUseOriCcmNrmlBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlBrightnessLmt);
                        LOGE("V2500 set AWB.dSwAwbUseOriCcmNrmlUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseOriCcmNrmlUnderExposeLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmLowLuxGainLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxGainLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmDarkGainLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkGainLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmLowLuxBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxBrightnessLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmLowLuxUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmLowLuxUnderExposeLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmDarkBrightnessLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkBrightnessLmt);
                        LOGE("V2500 set AWB.dSwAwbUseStdCcmDarkUnderExposeLmt: %lf\n", pCtrlAWB->dSwAwbUseStdCcmDarkUnderExposeLmt);
                        //LOGE("V2500 set AWB.bHwAwbEnable: %d\n", pCtrlAWB->bHwAwbEnable);           // It's only for V2505.
                        //LOGE("V2500 set AWB.uiHwAwbMethod: %d\n", pCtrlAWB->uiHwAwbMethod);         // It's only for V2505.
                        //LOGE("V2500 set AWB.uiHwAwbFirstPixel: %d\n", pCtrlAWB->uiHwAwbFirstPixel); // It's only for V2505.
                        //LOGE("V2500 set AWB.stHwAwbStatistic[]:\n");                                // It's read only and only for V2505.
                        //for (int i = 0; i < 16; i++) {
                        //    for (int j = 0; j < 16; j += 4) {
                        //        LOGE("%5.3f %5.3f %4d, %5.3f %5.3f %4d, %5.3f %5.3f %4d, %5.3f %5.3f %4d\n",
                        //             pCtrlAWB->stHwAwbStatistic[i][j].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+1].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+2].uiQualCnt,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].fGR,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].fGB,
                        //             pCtrlAWB->stHwAwbStatistic[i][j+3].uiQualCnt
                        //            );
                        //    }
                        //}
                        LOGE("V2500 set AWB.dHwSwAwbUpdateSpeed: %lf\n", pCtrlAWB->dHwSwAwbUpdateSpeed);
                        LOGE("V2500 set AWB.dHwSwAwbDropRatio: %lf\n", pCtrlAWB->dHwSwAwbDropRatio);
                        LOGE("V2500 set AWB.auiHwSwAwbWeightingTable[]:\n");
                        for (int iBIdx = 0; iBIdx < 32; iBIdx++) {
                            //LOGE("HW_SW_AWB_WEIGHTING_TBL_%02d:", iBIdx);
                            for (int iRIdx = 0; iRIdx < 32; iRIdx++) {
                                if ((32 - 1) == iRIdx) {
                                    LOGE("%2d\n", pCtrlAWB->auiHwSwAwbWeightingTable[iBIdx][iRIdx]);
                                } else {
                                    LOGE("%2d ", pCtrlAWB->auiHwSwAwbWeightingTable[iBIdx][iRIdx]);
                                }
                            }
                        }
                        LOGE("V2500 set AWB.dRGainMin: %lf\n", pCtrlAWB->dRGainMin);
                        LOGE("V2500 set AWB.dRGainMax: %lf\n", pCtrlAWB->dRGainMax);
                        LOGE("V2500 set AWB.dBGainMin: %lf\n", pCtrlAWB->dBGainMin);
                        LOGE("V2500 set AWB.dBGainMax: %lf\n", pCtrlAWB->dBGainMax);
                        LOGE("V2500 set AWB.uiFramesToSkip: %d\n", pCtrlAWB->uiFramesToSkip);
                        LOGE("V2500 set AWB.uiFramesSkipped: %d\n", pCtrlAWB->uiFramesSkipped);
                        LOGE("V2500 set AWB.bResetStates: %d\n", pCtrlAWB->bResetStates);
                        LOGE("V2500 set AWB.dMargin: %lf\n", pCtrlAWB->dMargin);
                        LOGE("V2500 set AWB.dPidKP: %lf\n", pCtrlAWB->dPidKP);
                        LOGE("V2500 set AWB.dPidKD: %lf\n", pCtrlAWB->dPidKD);
                        LOGE("V2500 set AWB.dPidKI: %lf\n", pCtrlAWB->dPidKI);
                    }

                    pIspAWB->enableControl(pCtrlAWB->bEnable);
                    pIspAWB->enableAWB(pCtrlAWB->bDoAwb);
                    pIspAWB->setCorrectionMode((ISPC::ControlAWB::Correction_Types)pCtrlAWB->uiCorrectionMode);
                    //pCtrlAWB->dRGain = pIspAWB->getRGain();
                    //pCtrlAWB->dBGain = pIspAWB->getBGain();
                    ////pCtrlAWB->dTemperature = pIspAWB->getCorrectionTemperature();
                    //pCtrlAWB->dTemperature = pIspAWB->getMeasuredTemperature();
                    pIspAWB->bSwAwbEnable = pCtrlAWB->bSwAwbEnable;
                    pIspAWB->bUseOriginalCCM = pCtrlAWB->bUseOriginalCCM;
                    //pCtrlAWB->dImgRGain = pIspAWB->dbImg_rgain;
                    //pCtrlAWB->dImgBGain = pIspAWB->dbImg_bgain;
                    //pCtrlAWB->dSwRGain = pIspAWB->dbSw_rgain;
                    //pCtrlAWB->dSwBGain = pIspAWB->dbSw_bgain;
                    pIspAWB->dbSwAwbUseOriCcmNrmlGainLmt = pCtrlAWB->dSwAwbUseOriCcmNrmlGainLmt;
                    pIspAWB->dbSwAwbUseOriCcmNrmlBrightnessLmt = pCtrlAWB->dSwAwbUseOriCcmNrmlBrightnessLmt;
                    pIspAWB->dbSwAwbUseOriCcmNrmlUnderExposeLmt = pCtrlAWB->dSwAwbUseOriCcmNrmlUnderExposeLmt;
                    pIspAWB->dbSwAwbUseStdCcmLowLuxGainLmt = pCtrlAWB->dSwAwbUseStdCcmLowLuxGainLmt;
                    pIspAWB->dbSwAwbUseStdCcmDarkGainLmt = pCtrlAWB->dSwAwbUseStdCcmDarkGainLmt;
                    pIspAWB->dbSwAwbUseStdCcmLowLuxBrightnessLmt = pCtrlAWB->dSwAwbUseStdCcmLowLuxBrightnessLmt;
                    pIspAWB->dbSwAwbUseStdCcmLowLuxUnderExposeLmt = pCtrlAWB->dSwAwbUseStdCcmLowLuxUnderExposeLmt;
                    pIspAWB->dbSwAwbUseStdCcmDarkBrightnessLmt = pCtrlAWB->dSwAwbUseStdCcmDarkBrightnessLmt;
                    pIspAWB->dbSwAwbUseStdCcmDarkUnderExposeLmt = pCtrlAWB->dSwAwbUseStdCcmDarkUnderExposeLmt;
                    //pIspAWB->bHwAwbEnable = pCtrlAWB->bHwAwbEnable;                 // It's only for V2505.
                    //pIspAWB->ui32HwAwbMethod = pCtrlAWB->uiHwAwbMethod;             // It's only for V2505.
                    //pIspAWB->ui32HwAwbFirstPixel = pCtrlAWB->uiHwAwbFirstPixel;     // It's only for V2505.
                    //memcpy(                                                         // It's read only and only for V2505.
                    //    (void *)pCtrlAWB->stHwAwbStatistic,
                    //    (void *)pIspAWB->stHwAwbStat,
                    //    sizeof(pCtrlAWB->stHwAwbStatistic)
                    //    );
                    pIspAWB->dbHwSwAwbUpdateSpeed = pCtrlAWB->dHwSwAwbUpdateSpeed;
                    pIspAWB->dbHwSwAwbDropRatio = pCtrlAWB->dHwSwAwbDropRatio;
                    memcpy(
                        (void *)pIspAWB->uiHwSwAwbWeightingTable,
                        (void *)pCtrlAWB->auiHwSwAwbWeightingTable,
                        sizeof(pCtrlAWB->auiHwSwAwbWeightingTable)
                        );
                    pIspAWB->fRGainMin = pCtrlAWB->dRGainMin;
                    pIspAWB->fRGainMax = pCtrlAWB->dRGainMax;
                    pIspAWB->fBGainMin = pCtrlAWB->dBGainMin;
                    pIspAWB->fBGainMax = pCtrlAWB->dBGainMax;
                    //pCtrlAWB->uiFramesToSkip = pIspAWB->getNumberToSkip();
                    //pCtrlAWB->uiFramesSkipped = pIspAWB->getNumberOfSkipped();
                    pIspAWB->setResetStates(pCtrlAWB->bResetStates);
                    pIspAWB->setMargin(pCtrlAWB->dMargin);
                    pIspAWB->setPidKP(pCtrlAWB->dPidKP);
                    pIspAWB->setPidKD(pCtrlAWB->dPidKD);
                    pIspAWB->setPidKI(pCtrlAWB->dPidKI);
                    pIspAWB->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetCMCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_CMC* pCtrlCMC = (ISP_CTRL_CMC*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlCMC *pIspCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    unsigned int uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_MAX];
    int iIdx;
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlCMC || !pIspCMC) {
        LOGE("V2500 get CMC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlCMC=%p, pIspCMC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlCMC, pIspCMC);
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
            if ((ISP_CMC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_CMC_CMD_VER_V0:
                default:
                    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE] = 3;
                    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT] = 3;
                    //pCtrlCMC->bEnable = pIspCMC->isEnabled();
                    pCtrlCMC->bCmcEnable = pIspCMC->bCmcEnable;
                    pCtrlCMC->bDnTargetIdxChageEnable = pIspCMC->bDnTargetIdxChageEnable;
                    pCtrlCMC->bSensorGainMedianFilterEnable = pIspCMC->bEnableCalcAvgGain;
                    pCtrlCMC->bSensorGainLevelCtrl = pIspCMC->blSensorGainLevelCtrl;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dSensorGainLevel[0] = 1.0f;
                        pCtrlCMC->dSensorGainLevelInterpolation[0] = 1.0f;
                        pCtrlCMC->dCcmAttenuation[0] = 1.0f;
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dSensorGainLevel[1] = pIspCMC->fSensorGain_lv1;
                        pCtrlCMC->dSensorGainLevelInterpolation[1] = pIspCMC->fSensorGainLv1Interpolation;
                        pCtrlCMC->dCcmAttenuation[1] = pIspCMC->dbCcmAttenuation_lv1;
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dSensorGainLevel[2] = pIspCMC->fSensorGain_lv2;
                        pCtrlCMC->dSensorGainLevelInterpolation[2] = pIspCMC->fSensorGainLv2Interpolation;
                        pCtrlCMC->dCcmAttenuation[2] = pIspCMC->dbCcmAttenuation_lv2;
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dSensorGainLevel[3] = pIspCMC->fSensorGain_lv3;
                    //    pCtrlCMC->dSensorGainLevelInterpolation[3] = pIspCMC->fSensorGainLv3Interpolation;
                    //    pCtrlCMC->dCcmAttenuation[3] = pIspCMC->dbCcmAttenuation_lv3;
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dSensorGainLevel[4] = pIspCMC->fSensorGain_lv4;
                    //    pCtrlCMC->dSensorGainLevelInterpolation[4] = pIspCMC->fSensorGainLv4Interpolation;
                    //    pCtrlCMC->dCcmAttenuation[4] = pIspCMC->dbCcmAttenuation_lv4;
                    //}
                    pCtrlCMC->bInterpolationGammaStyleEnable = pIspCMC->bEnableInterpolationGamma;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dInterpolationGamma[0] = pIspCMC->fCmcInterpolationGamma[0];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pCtrlCMC->afGammaLUT[0][iIdx] = pIspCMC->afGammaLUT[0][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dInterpolationGamma[1] = pIspCMC->fCmcInterpolationGamma[1];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pCtrlCMC->afGammaLUT[1][iIdx] = pIspCMC->afGammaLUT[1][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->dInterpolationGamma[2] = pIspCMC->fCmcInterpolationGamma[2];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pCtrlCMC->afGammaLUT[2][iIdx] = pIspCMC->afGammaLUT[2][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dInterpolationGamma[3] = pIspCMC->fCmcInterpolationGamma[3];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //    for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                    //        pCtrlCMC->afGammaLUT[3][iIdx] = pIspCMC->afGammaLUT[3][iIdx];
                    //    }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dInterpolationGamma[4] = pIspCMC->fCmcInterpolationGamma[4];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //    for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                    //        pCtrlCMC->afGammaLUT[4][iIdx] = pIspCMC->afGammaLUT[4][iIdx];
                    //    }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //}
                    pCtrlCMC->dR2YSaturationExpect = pIspCMC->fR2YSaturationExpect;
                    pCtrlCMC->dShaStrengthExpect = pIspCMC->fShaStrengthExpect;
                    //pCtrlCMC->bShaStrengthChange = pIspCMC->bShaStrengthChange;
                    //if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][0] = pIspCMC->fShaStrengthOffset_acm;
                    //}
                    //if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][1] = pIspCMC->fShaStrengthOffset_acm_lv1;
                    //}
                    //if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][2] = pIspCMC->fShaStrengthOffset_acm_lv2;
                    //}
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][3] = pIspCMC->fShaStrengthOffset_acm_lv3;
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][4] = pIspCMC->fShaStrengthOffset_acm_lv4;
                    //}
                    //if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][0] = pIspCMC->fShaStrengthOffset_fcm;
                    //}
                    //if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][1] = pIspCMC->fShaStrengthOffset_fcm_lv1;
                    //}
                    //if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][2] = pIspCMC->fShaStrengthOffset_fcm_lv2;
                    //}
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][3] = pIspCMC->fShaStrengthOffset_fcm_lv3;
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][4] = pIspCMC->fShaStrengthOffset_fcm_lv4;
                    //}
                    pCtrlCMC->bCmcNightModeDetectEnable = pIspCMC->bCmcNightModeDetectEnable;
                    pCtrlCMC->dCmcNightModeDetectBrightnessEnter = pIspCMC->fCmcNightModeDetectBrightnessEnter;
                    pCtrlCMC->dCmcNightModeDetectBrightnessExit = pIspCMC->fCmcNightModeDetectBrightnessExit;
                    pCtrlCMC->dCmcNightModeDetectGainEnter = pIspCMC->fCmcNightModeDetectGainEnter;
                    pCtrlCMC->dCmcNightModeDetectGainExit = pIspCMC->fCmcNightModeDetectGainExit;
                    pCtrlCMC->dCmcNightModeDetectWeighting = pIspCMC->fCmcNightModeDetectWeighting;

                    pCtrlCMC->bCaptureModeIqParamChangeEnable = pIspCMC->bEnableCaptureIQ;
                    pCtrlCMC->dCaptureModeCCMRatio = pIspCMC->fCmcCaptureModeCCMRatio;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pCtrlCMC->adCaptureModeR2YRangeMult[iIdx] = pIspCMC->fCmcCaptureR2YRangeMul[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pCtrlCMC->aiCaptureModeTnmInY[iIdx] = pIspCMC->iCmcCaptureInY[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pCtrlCMC->aiCaptureModeTnmOutY[iIdx] = pIspCMC->iCmcCaptureOutY[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pCtrlCMC->aiCaptureModeTnmOutC[iIdx] = pIspCMC->iCmcCaptureOutC[iIdx];
                    }

                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiNoOfGainLevelUsed = uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE];
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTargetBrightness = pIspCMC->targetBrightness_acm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dAeTargetMin = pIspCMC->fAeTargetMin_acm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSensorMaxGain = pIspCMC->fSensorMaxGain_acm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBrightness = pIspCMC->fBrightness_acm;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].adRangeMult[iIdx] = pIspCMC->aRangeMult_acm[iIdx];
                    }
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bEnableGamma = pIspCMC->bEnableGamma_acm;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[0] = pIspCMC->ui32DnTargetIdx_acm;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[0][iIdx] = pIspCMC->ui32BlcSensorBlack_acm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[0] = pIspCMC->fDnsStrength_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[0] = pIspCMC->fRadius_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[0] = pIspCMC->fStrength_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[0] = pIspCMC->fThreshold_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[0] = pIspCMC->fDetail_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[0] = pIspCMC->fEdgeScale_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[0] = pIspCMC->fEdgeOffset_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[0] = pIspCMC->bBypassDenoise_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[0] = pIspCMC->fDenoiseTau_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[0] = pIspCMC->fDenoiseSigma_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[0] = pIspCMC->fContrast_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[0] = pIspCMC->fSaturation_acm;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[0][iIdx] = pIspCMC->iGridStartCoords_acm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[0][iIdx] = pIspCMC->iGridTileDimensions_acm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[0] = pIspCMC->fDpfWeight_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[0] = pIspCMC->iDpfThreshold_acm;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[0][iIdx] = pIspCMC->iInY_acm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[0][iIdx] = pIspCMC->iOutY_acm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[0][iIdx] = pIspCMC->iOutC_acm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[0] = pIspCMC->fFlatFactor_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[0] = pIspCMC->fWeightLine_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[0] = pIspCMC->fColourConfidence_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[0] = pIspCMC->fColourSaturation_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[0] = pIspCMC->fEqualBrightSupressRatio_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[0] = pIspCMC->fEqualDarkSupressRatio_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[0] = pIspCMC->fOvershotThreshold_acm;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[0][iIdx] = pIspCMC->fWdrCeiling_acm[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[0][iIdx] = pIspCMC->fWdrFloor_acm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[0] = pIspCMC->ui32GammaCrvMode_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[0] = pIspCMC->fTnmGamma_acm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[0] = pIspCMC->fBezierCtrlPnt_acm;
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[1] = pIspCMC->ui32DnTargetIdx_acm_lv1;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[1][iIdx] = pIspCMC->ui32BlcSensorBlack_acm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[1] = pIspCMC->fDnsStrength_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[1] = pIspCMC->fRadius_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[1] = pIspCMC->fStrength_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[1] = pIspCMC->fThreshold_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[1] = pIspCMC->fDetail_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[1] = pIspCMC->fEdgeScale_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[1] = pIspCMC->fEdgeOffset_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[1] = pIspCMC->bBypassDenoise_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[1] = pIspCMC->fDenoiseTau_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[1] = pIspCMC->fDenoiseSigma_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[1] = pIspCMC->fContrast_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[1] = pIspCMC->fSaturation_acm_lv1;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[1][iIdx] = pIspCMC->iGridStartCoords_acm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[1][iIdx] = pIspCMC->iGridTileDimensions_acm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[1] = pIspCMC->fDpfWeight_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[1] = pIspCMC->iDpfThreshold_acm_lv1;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[1][iIdx] = pIspCMC->iInY_acm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[1][iIdx] = pIspCMC->iOutY_acm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[1][iIdx] = pIspCMC->iOutC_acm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[1] = pIspCMC->fFlatFactor_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[1] = pIspCMC->fWeightLine_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[1] = pIspCMC->fColourConfidence_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[1] = pIspCMC->fColourSaturation_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[1] = pIspCMC->fEqualBrightSupressRatio_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[1] = pIspCMC->fEqualDarkSupressRatio_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[1] = pIspCMC->fOvershotThreshold_acm_lv1;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[1][iIdx] = pIspCMC->fWdrCeiling_acm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[1][iIdx] = pIspCMC->fWdrFloor_acm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[1] = pIspCMC->ui32GammaCrvMode_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[1] = pIspCMC->fTnmGamma_acm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[1] = pIspCMC->fBezierCtrlPnt_acm_lv1;
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[2] = pIspCMC->ui32DnTargetIdx_acm_lv2;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[2][iIdx] = pIspCMC->ui32BlcSensorBlack_acm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[2] = pIspCMC->fDnsStrength_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[2] = pIspCMC->fRadius_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[2] = pIspCMC->fStrength_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[2] = pIspCMC->fThreshold_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[2] = pIspCMC->fDetail_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[2] = pIspCMC->fEdgeScale_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[2] = pIspCMC->fEdgeOffset_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[2] = pIspCMC->bBypassDenoise_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[2] = pIspCMC->fDenoiseTau_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[2] = pIspCMC->fDenoiseSigma_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[2] = pIspCMC->fContrast_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[2] = pIspCMC->fSaturation_acm_lv2;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[2][iIdx] = pIspCMC->iGridStartCoords_acm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[2][iIdx] = pIspCMC->iGridTileDimensions_acm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[2] = pIspCMC->fDpfWeight_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[2] = pIspCMC->iDpfThreshold_acm_lv2;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[2][iIdx] = pIspCMC->iInY_acm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[2][iIdx] = pIspCMC->iOutY_acm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[2][iIdx] = pIspCMC->iOutC_acm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[2] = pIspCMC->fFlatFactor_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[2] = pIspCMC->fWeightLine_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[2] = pIspCMC->fColourConfidence_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[2] = pIspCMC->fColourSaturation_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[2] = pIspCMC->fEqualBrightSupressRatio_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[2] = pIspCMC->fEqualDarkSupressRatio_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[2] = pIspCMC->fOvershotThreshold_acm_lv2;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[2][iIdx] = pIspCMC->fWdrCeiling_acm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[2][iIdx] = pIspCMC->fWdrFloor_acm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[2] = pIspCMC->ui32GammaCrvMode_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[2] = pIspCMC->fTnmGamma_acm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[2] = pIspCMC->fBezierCtrlPnt_acm_lv2;
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[3] = pIspCMC->ui32DnTargetIdx_acm_lv3;
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[3][iIdx] = pIspCMC->ui32BlcSensorBlack_acm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[3] = pIspCMC->fDnsStrength_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[3] = pIspCMC->fRadius_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[3] = pIspCMC->fStrength_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[3] = pIspCMC->fThreshold_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[3] = pIspCMC->fDetail_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[3] = pIspCMC->fEdgeScale_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[3] = pIspCMC->fEdgeOffset_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[3] = pIspCMC->bBypassDenoise_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[3] = pIspCMC->fDenoiseTau_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[3] = pIspCMC->fDenoiseSigma_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[3] = pIspCMC->fContrast_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[3] = pIspCMC->fSaturation_acm_lv3;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[3][iIdx] = pIspCMC->iGridStartCoords_acm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[3][iIdx] = pIspCMC->iGridTileDimensions_acm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[3] = pIspCMC->fDpfWeight_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[3] = pIspCMC->iDpfThreshold_acm_lv3;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[3][iIdx] = pIspCMC->iInY_acm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[3][iIdx] = pIspCMC->iOutY_acm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[3][iIdx] = pIspCMC->iOutC_acm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[3] = pIspCMC->fFlatFactor_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[3] = pIspCMC->fWeightLine_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[3] = pIspCMC->fColourConfidence_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[3] = pIspCMC->fColourSaturation_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[3] = pIspCMC->fEqualBrightSupressRatio_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[3] = pIspCMC->fEqualDarkSupressRatio_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[3] = pIspCMC->fOvershotThreshold_acm_lv3;
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[3][iIdx] = pIspCMC->fWdrCeiling_acm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[3][iIdx] = pIspCMC->fWdrFloor_acm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[3] = pIspCMC->ui32GammaCrvMode_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[3] = pIspCMC->fTnmGamma_acm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[3] = pIspCMC->fBezierCtrlPnt_acm_lv3;
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[4] = pIspCMC->ui32DnTargetIdx_acm_lv4;
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[4][iIdx] = pIspCMC->ui32BlcSensorBlack_acm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[4] = pIspCMC->fDnsStrength_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[4] = pIspCMC->fRadius_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[4] = pIspCMC->fStrength_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[4] = pIspCMC->fThreshold_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[4] = pIspCMC->fDetail_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[4] = pIspCMC->fEdgeScale_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[4] = pIspCMC->fEdgeOffset_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[4] = pIspCMC->bBypassDenoise_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[4] = pIspCMC->fDenoiseTau_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[4] = pIspCMC->fDenoiseSigma_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[4] = pIspCMC->fContrast_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[4] = pIspCMC->fSaturation_acm_lv4;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[4][iIdx] = pIspCMC->iGridStartCoords_acm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[4][iIdx] = pIspCMC->iGridTileDimensions_acm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[4] = pIspCMC->fDpfWeight_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[4] = pIspCMC->iDpfThreshold_acm_lv4;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[4][iIdx] = pIspCMC->iInY_acm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[4][iIdx] = pIspCMC->iOutY_acm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[4][iIdx] = pIspCMC->iOutC_acm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[4] = pIspCMC->fFlatFactor_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[4] = pIspCMC->fWeightLine_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[4] = pIspCMC->fColourConfidence_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[4] = pIspCMC->fColourSaturation_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[4] = pIspCMC->fEqualBrightSupressRatio_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[4] = pIspCMC->fEqualDarkSupressRatio_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[4] = pIspCMC->fOvershotThreshold_acm_lv4;
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[4][iIdx] = pIspCMC->fWdrCeiling_acm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[4][iIdx] = pIspCMC->fWdrFloor_acm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[4] = pIspCMC->ui32GammaCrvMode_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[4] = pIspCMC->fTnmGamma_acm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[4] = pIspCMC->fBezierCtrlPnt_acm_lv4;
                    //}
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiNoOfGainLevelUsed = uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT];
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTargetBrightness = pIspCMC->targetBrightness_fcm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dAeTargetMin = pIspCMC->fAeTargetMin_fcm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSensorMaxGain = pIspCMC->fSensorMaxGain_fcm;
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBrightness = pIspCMC->fBrightness_fcm;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].adRangeMult[iIdx] = pIspCMC->aRangeMult_fcm[iIdx];
                    }
                    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bEnableGamma = pIspCMC->bEnableGamma_fcm;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[0] = pIspCMC->ui32DnTargetIdx_fcm;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[0][iIdx] = pIspCMC->ui32BlcSensorBlack_fcm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[0] = pIspCMC->fDnsStrength_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[0] = pIspCMC->fRadius_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[0] = pIspCMC->fStrength_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[0] = pIspCMC->fThreshold_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[0] = pIspCMC->fDetail_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[0] = pIspCMC->fEdgeScale_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[0] = pIspCMC->fEdgeOffset_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[0] = pIspCMC->bBypassDenoise_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[0] = pIspCMC->fDenoiseTau_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[0] = pIspCMC->fDenoiseSigma_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[0] = pIspCMC->fContrast_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[0] = pIspCMC->fSaturation_fcm;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[0][iIdx] = pIspCMC->iGridStartCoords_fcm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[0][iIdx] = pIspCMC->iGridTileDimensions_fcm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[0] = pIspCMC->fDpfWeight_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[0] = pIspCMC->iDpfThreshold_fcm;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[0][iIdx] = pIspCMC->iInY_fcm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[0][iIdx] = pIspCMC->iOutY_fcm[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[0][iIdx] = pIspCMC->iOutC_fcm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[0] = pIspCMC->fFlatFactor_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[0] = pIspCMC->fWeightLine_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[0] = pIspCMC->fColourConfidence_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[0] = pIspCMC->fColourSaturation_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[0] = pIspCMC->fEqualBrightSupressRatio_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[0] = pIspCMC->fEqualDarkSupressRatio_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[0] = pIspCMC->fOvershotThreshold_fcm;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[0][iIdx] = pIspCMC->fWdrCeiling_fcm[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[0][iIdx] = pIspCMC->fWdrFloor_fcm[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[0] = pIspCMC->ui32GammaCrvMode_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[0] = pIspCMC->fTnmGamma_fcm;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[0] = pIspCMC->fBezierCtrlPnt_fcm;
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[1] = pIspCMC->ui32DnTargetIdx_fcm_lv1;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[1][iIdx] = pIspCMC->ui32BlcSensorBlack_fcm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[1] = pIspCMC->fDnsStrength_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[1] = pIspCMC->fRadius_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[1] = pIspCMC->fStrength_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[1] = pIspCMC->fThreshold_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[1] = pIspCMC->fDetail_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[1] = pIspCMC->fEdgeScale_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[1] = pIspCMC->fEdgeOffset_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[1] = pIspCMC->bBypassDenoise_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[1] = pIspCMC->fDenoiseTau_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[1] = pIspCMC->fDenoiseSigma_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[1] = pIspCMC->fContrast_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[1] = pIspCMC->fSaturation_fcm_lv1;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[1][iIdx] = pIspCMC->iGridStartCoords_fcm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[1][iIdx] = pIspCMC->iGridTileDimensions_fcm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[1] = pIspCMC->fDpfWeight_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[1] = pIspCMC->iDpfThreshold_fcm_lv1;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[1][iIdx] = pIspCMC->iInY_fcm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[1][iIdx] = pIspCMC->iOutY_fcm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[1][iIdx] = pIspCMC->iOutC_fcm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[1] = pIspCMC->fFlatFactor_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[1] = pIspCMC->fWeightLine_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[1] = pIspCMC->fColourConfidence_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[1] = pIspCMC->fColourSaturation_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[1] = pIspCMC->fEqualBrightSupressRatio_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[1] = pIspCMC->fEqualDarkSupressRatio_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[1] = pIspCMC->fOvershotThreshold_fcm_lv1;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[1][iIdx] = pIspCMC->fWdrCeiling_fcm_lv1[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[1][iIdx] = pIspCMC->fWdrFloor_fcm_lv1[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[1] = pIspCMC->ui32GammaCrvMode_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[1] = pIspCMC->fTnmGamma_fcm_lv1;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[1] = pIspCMC->fBezierCtrlPnt_fcm_lv1;
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[2] = pIspCMC->ui32DnTargetIdx_fcm_lv2;
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[2][iIdx] = pIspCMC->ui32BlcSensorBlack_fcm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[2] = pIspCMC->fDnsStrength_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[2] = pIspCMC->fRadius_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[2] = pIspCMC->fStrength_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[2] = pIspCMC->fThreshold_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[2] = pIspCMC->fDetail_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[2] = pIspCMC->fEdgeScale_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[2] = pIspCMC->fEdgeOffset_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[2] = pIspCMC->bBypassDenoise_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[2] = pIspCMC->fDenoiseTau_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[2] = pIspCMC->fDenoiseSigma_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[2] = pIspCMC->fContrast_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[2] = pIspCMC->fSaturation_fcm_lv2;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[2][iIdx] = pIspCMC->iGridStartCoords_fcm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[2][iIdx] = pIspCMC->iGridTileDimensions_fcm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[2] = pIspCMC->fDpfWeight_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[2] = pIspCMC->iDpfThreshold_fcm_lv2;
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[2][iIdx] = pIspCMC->iInY_fcm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[2][iIdx] = pIspCMC->iOutY_fcm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[2][iIdx] = pIspCMC->iOutC_fcm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[2] = pIspCMC->fFlatFactor_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[2] = pIspCMC->fWeightLine_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[2] = pIspCMC->fColourConfidence_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[2] = pIspCMC->fColourSaturation_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[2] = pIspCMC->fEqualBrightSupressRatio_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[2] = pIspCMC->fEqualDarkSupressRatio_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[2] = pIspCMC->fOvershotThreshold_fcm_lv2;
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[2][iIdx] = pIspCMC->fWdrCeiling_fcm_lv2[iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[2][iIdx] = pIspCMC->fWdrFloor_fcm_lv2[iIdx];
                        }
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[2] = pIspCMC->ui32GammaCrvMode_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[2] = pIspCMC->fTnmGamma_fcm_lv2;
                        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[2] = pIspCMC->fBezierCtrlPnt_fcm_lv2;
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[3] = pIspCMC->ui32DnTargetIdx_fcm_lv3;
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[3][iIdx] = pIspCMC->ui32BlcSensorBlack_fcm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[3] = pIspCMC->fDnsStrength_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[3] = pIspCMC->fRadius_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[3] = pIspCMC->fStrength_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[3] = pIspCMC->fThreshold_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[3] = pIspCMC->fDetail_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[3] = pIspCMC->fEdgeScale_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[3] = pIspCMC->fEdgeOffset_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[3] = pIspCMC->bBypassDenoise_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[3] = pIspCMC->fDenoiseTau_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[3] = pIspCMC->fDenoiseSigma_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[3] = pIspCMC->fContrast_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[3] = pIspCMC->fSaturation_fcm_lv3;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[3][iIdx] = pIspCMC->iGridStartCoords_fcm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[3][iIdx] = pIspCMC->iGridTileDimensions_fcm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[3] = pIspCMC->fDpfWeight_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[3] = pIspCMC->iDpfThreshold_fcm_lv3;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[3][iIdx] = pIspCMC->iInY_fcm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[3][iIdx] = pIspCMC->iOutY_fcm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[3][iIdx] = pIspCMC->iOutC_fcm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[3] = pIspCMC->fFlatFactor_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[3] = pIspCMC->fWeightLine_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[3] = pIspCMC->fColourConfidence_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[3] = pIspCMC->fColourSaturation_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[3] = pIspCMC->fEqualBrightSupressRatio_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[3] = pIspCMC->fEqualDarkSupressRatio_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[3] = pIspCMC->fOvershotThreshold_fcm_lv3;
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[3][iIdx] = pIspCMC->fWdrCeiling_fcm_lv3[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[3][iIdx] = pIspCMC->fWdrFloor_fcm_lv3[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[3] = pIspCMC->ui32GammaCrvMode_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[3] = pIspCMC->fTnmGamma_fcm_lv3;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[3] = pIspCMC->fBezierCtrlPnt_fcm_lv3;
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[4] = pIspCMC->ui32DnTargetIdx_fcm_lv4;
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[4][iIdx] = pIspCMC->ui32BlcSensorBlack_fcm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[4] = pIspCMC->fDnsStrength_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[4] = pIspCMC->fRadius_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[4] = pIspCMC->fStrength_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[4] = pIspCMC->fThreshold_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[4] = pIspCMC->fDetail_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[4] = pIspCMC->fEdgeScale_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[4] = pIspCMC->fEdgeOffset_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[4] = pIspCMC->bBypassDenoise_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[4] = pIspCMC->fDenoiseTau_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[4] = pIspCMC->fDenoiseSigma_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[4] = pIspCMC->fContrast_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[4] = pIspCMC->fSaturation_fcm_lv4;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[4][iIdx] = pIspCMC->iGridStartCoords_fcm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[4][iIdx] = pIspCMC->iGridTileDimensions_fcm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[4] = pIspCMC->fDpfWeight_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[4] = pIspCMC->iDpfThreshold_fcm_lv4;
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[4][iIdx] = pIspCMC->iOutY_fcm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[4][iIdx] = pIspCMC->iOutY_fcm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[4][iIdx] = pIspCMC->iOutC_fcm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[4] = pIspCMC->fFlatFactor_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[4] = pIspCMC->fWeightLine_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[4] = pIspCMC->fColourConfidence_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[4] = pIspCMC->fColourSaturation_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[4] = pIspCMC->fEqualBrightSupressRatio_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[4] = pIspCMC->fEqualDarkSupressRatio_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[4] = pIspCMC->fOvershotThreshold_fcm_lv4;
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[1][iIdx] = pIspCMC->fWdrCeiling_fcm_lv4[iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[4][iIdx] = pIspCMC->fWdrFloor_fcm_lv4[iIdx];
                    //    }
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[4] = pIspCMC->ui32GammaCrvMode_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[4] = pIspCMC->fTnmGamma_fcm_lv4;
                    //    pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[4] = pIspCMC->fBezierCtrlPnt_fcm_lv4;
                    //}
                    if (ISP_CMC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_CTRL_CMC);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        int iCmcMode;
                        int iLvl;

                        LOGE("V2500 get CMC.datasize: %u\n", pTcp->datasize);
                        //LOGE("V2500 get CMC.bEnable: %d\n", pCtrlCMC->bEnable);
                        LOGE("V2500 get CMC.bCmcEnable: %d\n", pCtrlCMC->bCmcEnable);
                        LOGE("V2500 get CMC.bDnTargetIdxChageEnable: %d\n", pCtrlCMC->bDnTargetIdxChageEnable);
                        LOGE("V2500 get CMC.bSensorGainMedianFilterEnable: %d\n", pCtrlCMC->bSensorGainMedianFilterEnable);
                        LOGE("V2500 get CMC.bSensorGainLevelCtrl: %d\n", pCtrlCMC->bSensorGainLevelCtrl);
                        for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                            if ((unsigned int)iLvl < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                                LOGE("V2500 get CMC.dSensorGainLevel[%d]: %lf\n", iLvl, pCtrlCMC->dSensorGainLevel[iLvl]);
                                LOGE("V2500 get CMC.dSensorGainLevelInterpolation[%d]: %lf\n", iLvl, pCtrlCMC->dSensorGainLevelInterpolation[iLvl]);
                                LOGE("V2500 get CMC.dCcmAttenuation[%d]: %lf\n", iLvl, pCtrlCMC->dCcmAttenuation[iLvl]);
                            } else {
                                break;
                            }
                        }
                        LOGE("V2500 get CMC.bInterpolationGammaStyleEnable: %d\n", pCtrlCMC->bInterpolationGammaStyleEnable);
                        for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                            if ((unsigned int)iLvl < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                                LOGE("V2500 get CMC.dInterpolationGamma[%d]: %lf\n", iLvl, pCtrlCMC->dInterpolationGamma[iLvl]);
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                                LOGE("V2500 get CMC.afGammaLUT[%d]:\n", iLvl, pCtrlCMC->afGammaLUT[iLvl]);
                                for (int iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                                    if ((16 - 1) == iIdx) {
                                        LOGE("%5.3lf\n", pCtrlCMC->afGammaLUT[iLvl][iIdx]);
                                    } else {
                                        LOGE("%5.3lf ", pCtrlCMC->afGammaLUT[iLvl][iIdx]);
                                    }
                                }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                            } else {
                                break;
                            }
                        }
                        LOGE("V2500 get CMC.dR2YSaturationExpect: %lf\n", pCtrlCMC->dR2YSaturationExpect);
                        LOGE("V2500 get CMC.dShaStrengthExpect: %lf\n", pCtrlCMC->dShaStrengthExpect);
                        //LOGE("V2500 get CMC.bShaStrengthChange: %d\n", pCtrlCMC->bShaStrengthChange);
                        //for (iCmcMode = 0; iCmcMode < ISP_CMC_COLOR_MODE_MAX; iCmcMode++) {
                        //    for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                        //        if ((unsigned int)iLvl < uiNoOfGainLevelUsed[iCmcMode]) {
                        //            LOGE("V2500 get CMC.dShaStrengthOffset[%d][%d]: %lf\n", iCmcMode, iLvl, pCtrlCMC->dShaStrengthOffset[iCmcMode][iLvl]);
                        //        } else {
                        //            break;
                        //        }
                        //    }
                        //}
                        LOGE("V2500 get CMC.bCmcNightModeDetectEnable: %d\n", pCtrlCMC->bCmcNightModeDetectEnable);
                        LOGE("V2500 get CMC.dCmcNightModeDetectBrightnessEnter: %lf\n", pCtrlCMC->dCmcNightModeDetectBrightnessEnter);
                        LOGE("V2500 get CMC.dCmcNightModeDetectBrightnessExit: %lf\n", pCtrlCMC->dCmcNightModeDetectBrightnessExit);
                        LOGE("V2500 get CMC.dCmcNightModeDetectGainEnter: %lf\n", pCtrlCMC->dCmcNightModeDetectGainEnter);
                        LOGE("V2500 get CMC.dCmcNightModeDetectGainExit: %lf\n", pCtrlCMC->dCmcNightModeDetectGainExit);
                        LOGE("V2500 get CMC.dCmcNightModeDetectWeighting: %lf\n", pCtrlCMC->dCmcNightModeDetectWeighting);
                        LOGE("V2500 get CMC.bCaptureModeIqParamChangeEnable: %d\n", pCtrlCMC->bCaptureModeIqParamChangeEnable);
                        LOGE("V2500 get CMC.dCaptureModeCCMRatio: %lf\n", pCtrlCMC->dCaptureModeCCMRatio);
                        LOGE("V2500 get CMC.adCaptureModeR2YRangeMult: [ %5.3lf %5.3lf %5.3lf ]\n",
                             pCtrlCMC->adCaptureModeR2YRangeMult[0],
                             pCtrlCMC->adCaptureModeR2YRangeMult[1],
                             pCtrlCMC->adCaptureModeR2YRangeMult[2]
                             );
                        LOGE("V2500 get CMC.aiCaptureModeTnmInY: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmInY[0], pCtrlCMC->aiCaptureModeTnmInY[1]);
                        LOGE("V2500 get CMC.aiCaptureModeTnmOutY: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmOutY[0], pCtrlCMC->aiCaptureModeTnmOutY[1]);
                        LOGE("V2500 get CMC.aiCaptureModeTnmOutC: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmOutC[0], pCtrlCMC->aiCaptureModeTnmOutC[1]);
                        for (iCmcMode = 0; iCmcMode < ISP_CMC_COLOR_MODE_MAX; iCmcMode++) {
                            LOGE("V2500 get CMC.stCMC[%s].uiNoOfGainLevelUsed: %u\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].uiNoOfGainLevelUsed);
                            LOGE("V2500 get CMC.stCMC[%s].dTargetBrightness: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dTargetBrightness);
                            LOGE("V2500 get CMC.stCMC[%s].dAeTargetMin: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dAeTargetMin);
                            LOGE("V2500 get CMC.stCMC[%s].dSensorMaxGain: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dSensorMaxGain);
                            LOGE("V2500 get CMC.stCMC[%s].dBrightness: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dBrightness);
                            LOGE("V2500 get CMC.stCMC[%s].adRangeMult: [ %5.3lf %5.3lf %5.3lf ]\n",
                                 ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[0],
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[1],
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[2]
                                 );
                            LOGE("V2500 get CMC.stCMC[%s].bEnableGamma: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].bEnableGamma);
                            for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                                if ((unsigned int)iLvl < uiNoOfGainLevelUsed[iCmcMode]) {
                                    LOGE("V2500 get CMC.stCMC[%s_%d].uiDnTargetIdx[%d]: %u\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].uiDnTargetIdx[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iBlcSensorBlack[%d]: [ %d %d %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][3]
                                         );
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dDnsStrength[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDnsStrength[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dRadius[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dRadius[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dStrength[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dStrength[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dThreshold[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dThreshold[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dDetail[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDetail[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dEdgeScale[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEdgeScale[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dEdgeOffset[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEdgeOffset[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].bBypassDenoise[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].bBypassDenoise[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dDenoiseTau[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDenoiseTau[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dDenoiseSigma[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDenoiseSigma[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dContrast[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dContrast[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dSaturation[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dSaturation[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iGridStartCoords[%d]: [ %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iGridStartCoords[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iGridStartCoords[iLvl][1]
                                         );
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iGridTileDimensions[%d]: [ %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iGridTileDimensions[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iGridTileDimensions[iLvl][1]
                                         );
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dDpfWeight[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDpfWeight[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iDpfThreshold[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iDpfThreshold[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iInY[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iInY[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iInY[iLvl][1]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iOutY[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iOutY[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iOutY[iLvl][1]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iOutC[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iOutC[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iOutC[iLvl][1]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dFlatFactor[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dFlatFactor[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dWeightLine[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dWeightLine[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dColourConfidence[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dColourConfidence[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dColourSaturation[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dColourSaturation[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dEqualBrightSuppressRatio[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEqualBrightSuppressRatio[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dEqualDarkSuppressRatio[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEqualDarkSuppressRatio[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dOvershotThreshold[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dOvershotThreshold[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dWdrCeiling[%d]: [ %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][3],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][4],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][5],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][6],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][7]
                                         );
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dWdrFloor[%d]: [ %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][3],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][4],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][5],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][6],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][7]
                                         );
                                    LOGE("V2500 get CMC.stCMC[%s_%d].iGammaCrvMode[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iGammaCrvMode[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dTnmGamma[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dTnmGamma[iLvl]);
                                    LOGE("V2500 get CMC.stCMC[%s_%d].dBezierCtrlPnt[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dBezierCtrlPnt[iLvl]);
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetCMCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_CMC* pCtrlCMC = (ISP_CTRL_CMC*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlCMC *pIspCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    unsigned int uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_MAX];
    int iIdx;
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlCMC || !pIspCMC) {
        LOGE("V2500 set CMC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlCMC=%p, pIspCMC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlCMC, pIspCMC);
        return iRet;
    }
    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiNoOfGainLevelUsed;
    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiNoOfGainLevelUsed;
    switch (pTcp->cmd.ui3Type) {
        case CMD_TYPE_SINGLE:
            break;

        case CMD_TYPE_GROUP:
            break;

        case CMD_TYPE_GROUP_ASAP:
            break;

        case CMD_TYPE_ALL:
            if ((ISP_CMC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }

            // Stop CMC function to avoid multi-thread access variables and caused variables fill wrong value.
            //pIspCMC->enableControl(pCtrlCMC->bEnable);
            pIspCMC->bCmcEnable = false;
            usleep(20000);

            switch (pTcp->ver) {
                case ISP_CMC_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        int iCmcMode;
                        int iLvl;

                        LOGE("V2500 set CMC.datasize: %u\n", pTcp->datasize);
                        //LOGE("V2500 set CMC.bEnable: %d\n", pCtrlCMC->bEnable);
                        LOGE("V2500 set CMC.bCmcEnable: %d\n", pCtrlCMC->bCmcEnable);
                        LOGE("V2500 set CMC.bDnTargetIdxChageEnable: %d\n", pCtrlCMC->bDnTargetIdxChageEnable);
                        LOGE("V2500 set CMC.bSensorGainMedianFilterEnable: %d\n", pCtrlCMC->bSensorGainMedianFilterEnable);
                        LOGE("V2500 set CMC.bSensorGainLevelCtrl: %d\n", pCtrlCMC->bSensorGainLevelCtrl);
                        for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                            if ((unsigned int)iLvl < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                                LOGE("V2500 set CMC.dSensorGainLevel[%d]: %lf\n", iLvl, pCtrlCMC->dSensorGainLevel[iLvl]);
                                LOGE("V2500 set CMC.dSensorGainLevelInterpolation[%d]: %lf\n", iLvl, pCtrlCMC->dSensorGainLevelInterpolation[iLvl]);
                                LOGE("V2500 set CMC.dCcmAttenuation[%d]: %lf\n", iLvl, pCtrlCMC->dCcmAttenuation[iLvl]);
                            } else {
                                break;
                            }
                        }
                        LOGE("V2500 set CMC.bInterpolationGammaStyleEnable: %d\n", pCtrlCMC->bInterpolationGammaStyleEnable);
                        for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                            if ((unsigned int)iLvl < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                                //LOGE("V2500 set CMC.dInterpolationGamma[%d]: %lf\n", iLvl, pCtrlCMC->dInterpolationGamma[iLvl]);
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                                LOGE("V2500 set CMC.afGammaLUT[%d]:\n", iLvl, pCtrlCMC->afGammaLUT[iLvl]);
                                for (int iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                                    if ((16 - 1) == iIdx) {
                                        LOGE("%5.3lf\n", pCtrlCMC->afGammaLUT[iLvl][iIdx]);
                                    } else {
                                        LOGE("%5.3lf ", pCtrlCMC->afGammaLUT[iLvl][iIdx]);
                                    }
                                }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                            } else {
                                break;
                            }
                        }
                        LOGE("V2500 set CMC.dR2YSaturationExpect: %lf\n", pCtrlCMC->dR2YSaturationExpect);
                        LOGE("V2500 set CMC.dShaStrengthExpect: %lf\n", pCtrlCMC->dShaStrengthExpect);
                        //LOGE("V2500 set CMC.bShaStrengthChange: %d\n", pCtrlCMC->bShaStrengthChange);
                        //for (iCmcMode = 0; iCmcMode < ISP_CMC_COLOR_MODE_MAX; iCmcMode++) {
                        //    for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                        //        if ((unsigned int)iLvl < uiNoOfGainLevelUsed[iCmcMode]) {
                        //            LOGE("V2500 set CMC.dShaStrengthOffset[%d][%d]: %lf\n", iCmcMode, iLvl, pCtrlCMC->dShaStrengthOffset[iCmcMode][iLvl]);
                        //        } else {
                        //            break;
                        //        }
                        //    }
                        //}
                        LOGE("V2500 set CMC.bCmcNightModeDetectEnable: %d\n", pCtrlCMC->bCmcNightModeDetectEnable);
                        LOGE("V2500 set CMC.dCmcNightModeDetectBrightnessEnter: %lf\n", pCtrlCMC->dCmcNightModeDetectBrightnessEnter);
                        LOGE("V2500 set CMC.dCmcNightModeDetectBrightnessExit: %lf\n", pCtrlCMC->dCmcNightModeDetectBrightnessExit);
                        LOGE("V2500 set CMC.dCmcNightModeDetectGainEnter: %lf\n", pCtrlCMC->dCmcNightModeDetectGainEnter);
                        LOGE("V2500 set CMC.dCmcNightModeDetectGainExit: %lf\n", pCtrlCMC->dCmcNightModeDetectGainExit);
                        LOGE("V2500 set CMC.dCmcNightModeDetectWeighting: %lf\n", pCtrlCMC->dCmcNightModeDetectWeighting);
                        LOGE("V2500 set CMC.bCaptureModeIqParamChangeEnable: %d\n", pCtrlCMC->bCaptureModeIqParamChangeEnable);
                        LOGE("V2500 set CMC.dCaptureModeCCMRatio: %lf\n", pCtrlCMC->dCaptureModeCCMRatio);
                        LOGE("V2500 set CMC.adCaptureModeR2YRangeMult: [ %5.3lf %5.3lf %5.3lf ]\n",
                             pCtrlCMC->adCaptureModeR2YRangeMult[0],
                             pCtrlCMC->adCaptureModeR2YRangeMult[1],
                             pCtrlCMC->adCaptureModeR2YRangeMult[2]
                             );
                        LOGE("V2500 set CMC.aiCaptureModeTnmInY: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmInY[0], pCtrlCMC->aiCaptureModeTnmInY[1]);
                        LOGE("V2500 set CMC.aiCaptureModeTnmOutY: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmOutY[0], pCtrlCMC->aiCaptureModeTnmOutY[1]);
                        LOGE("V2500 set CMC.aiCaptureModeTnmOutC: [ %d %d ]\n", pCtrlCMC->aiCaptureModeTnmOutC[0], pCtrlCMC->aiCaptureModeTnmOutC[1]);
                        for (iCmcMode = 0; iCmcMode < ISP_CMC_COLOR_MODE_MAX; iCmcMode++) {
                            LOGE("V2500 set CMC.stCMC[%s].uiNoOfGainLevelUsed: %u\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].uiNoOfGainLevelUsed);
                            LOGE("V2500 set CMC.stCMC[%s].dTargetBrightness: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dTargetBrightness);
                            LOGE("V2500 set CMC.stCMC[%s].dAeTargetMin: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dAeTargetMin);
                            LOGE("V2500 set CMC.stCMC[%s].dSensorMaxGain: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dSensorMaxGain);
                            LOGE("V2500 set CMC.stCMC[%s].dBrightness: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].dBrightness);
                            LOGE("V2500 set CMC.stCMC[%s].adRangeMult: [ %5.3lf %5.3lf %5.3lf ]\n",
                                 ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[0],
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[1],
                                 pCtrlCMC->stCMC[iCmcMode].adRangeMult[2]
                                 );
                            LOGE("V2500 set CMC.stCMC[%s].bEnableGamma: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), pCtrlCMC->stCMC[iCmcMode].bEnableGamma);
                            for (iLvl = 0; iLvl < ISP_CMC_GAIN_LEVEL_MAX; iLvl++) {
                                if ((unsigned int)iLvl < uiNoOfGainLevelUsed[iCmcMode]) {
                                    LOGE("V2500 set CMC.stCMC[%s_%d].uiDnTargetIdx[%d]: %u\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].uiDnTargetIdx[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iBlcSensorBlack[%d]: [ %d %d %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].iBlcSensorBlack[iLvl][3]
                                         );
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dDnsStrength[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDnsStrength[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dRadius[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dRadius[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dStrength[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dStrength[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dThreshold[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dThreshold[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dDetail[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDetail[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dEdgeScale[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEdgeScale[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dEdgeOffset[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEdgeOffset[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].bBypassDenoise[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].bBypassDenoise[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dDenoiseTau[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDenoiseTau[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dDenoiseSigma[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDenoiseSigma[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dContrast[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dContrast[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dSaturation[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dSaturation[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iGridStartCoords[%d]: [ %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iGridStartCoords[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iGridStartCoords[iLvl][1]
                                         );
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iGridTileDimensions[%d]: [ %d %d ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].iGridTileDimensions[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].iGridTileDimensions[iLvl][1]
                                         );
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dDpfWeight[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dDpfWeight[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iDpfThreshold[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iDpfThreshold[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iInY[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iInY[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iInY[iLvl][1]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iOutY[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iOutY[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iOutY[iLvl][1]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iOutC[%d]: [ %d %d ]\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iOutC[iLvl][0], pCtrlCMC->stCMC[iCmcMode].iOutC[iLvl][1]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dFlatFactor[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dFlatFactor[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dWeightLine[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dWeightLine[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dColourConfidence[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dColourConfidence[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dColourSaturation[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dColourSaturation[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dEqualBrightSuppressRatio[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEqualBrightSuppressRatio[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dEqualDarkSuppressRatio[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dEqualDarkSuppressRatio[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dOvershotThreshold[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dOvershotThreshold[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dWdrCeiling[%d]: [ %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][3],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][4],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][5],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][6],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrCeiling[iLvl][7]
                                         );
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dWdrFloor[%d]: [ %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf %5.3lf ]\n",
                                         ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")),
                                         iLvl,
                                         iLvl,
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][0],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][1],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][2],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][3],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][4],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][5],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][6],
                                         pCtrlCMC->stCMC[iCmcMode].dWdrFloor[iLvl][7]
                                         );
                                    LOGE("V2500 set CMC.stCMC[%s_%d].iGammaCrvMode[%d]: %d\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].iGammaCrvMode[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dTnmGamma[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dTnmGamma[iLvl]);
                                    LOGE("V2500 set CMC.stCMC[%s_%d].dBezierCtrlPnt[%d]: %lf\n", ((ISP_CMC_COLOR_MODE_ADVANCE == iCmcMode) ? ("ACM") : ("FCM")), iLvl, iLvl, pCtrlCMC->stCMC[iCmcMode].dBezierCtrlPnt[iLvl]);
                                } else {
                                    break;
                                }
                            }
                        }
                    }

                    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiNoOfGainLevelUsed;
                    uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiNoOfGainLevelUsed;
#if 0
                    //pIspCMC->enableControl(pCtrlCMC->bEnable);
                    pIspCMC->bCmcEnable = pCtrlCMC->bCmcEnable;
#endif
                    pIspCMC->bDnTargetIdxChageEnable = pCtrlCMC->bDnTargetIdxChageEnable;
                    pIspCMC->bEnableCalcAvgGain = pCtrlCMC->bSensorGainMedianFilterEnable;
                    pIspCMC->blSensorGainLevelCtrl = pCtrlCMC->bSensorGainLevelCtrl;
                    //if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fSensorGain_Lv0 = pCtrlCMC->dSensorGainLevel[0];
                    //    pIspCMC->fSensorGainLv0Interpolation = pCtrlCMC->dSensorGainLevelInterpolation[0];
                    //    pIspCMC->dbCcmAttenuation_lv0 = pCtrlCMC->dCcmAttenuation[0];
                    //}
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pIspCMC->fSensorGain_lv1 = pCtrlCMC->dSensorGainLevel[1];
                        pIspCMC->fSensorGainLv1Interpolation = pCtrlCMC->dSensorGainLevelInterpolation[1];
                        pIspCMC->dbCcmAttenuation_lv1 = pCtrlCMC->dCcmAttenuation[1];
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pIspCMC->fSensorGain_lv2 = pCtrlCMC->dSensorGainLevel[2];
                        pIspCMC->fSensorGainLv2Interpolation = pCtrlCMC->dSensorGainLevelInterpolation[2];
                        pIspCMC->dbCcmAttenuation_lv2 = pCtrlCMC->dCcmAttenuation[2];
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fSensorGain_lv3 = pCtrlCMC->dSensorGainLevel[3];
                    //    pIspCMC->fSensorGainLv3Interpolation = pCtrlCMC->dSensorGainLevelInterpolation[3];
                    //    pIspCMC->dbCcmAttenuation_lv3 = pCtrlCMC->dCcmAttenuation[3];
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fSensorGain_lv4 = pCtrlCMC->dSensorGainLevel[4];
                    //    pIspCMC->fSensorGainLv4Interpolation = pCtrlCMC->dSensorGainLevelInterpolation[4];
                    //    pIspCMC->dbCcmAttenuation_lv4 = pCtrlCMC->dCcmAttenuation[4];
                    //}
                    pIspCMC->bEnableInterpolationGamma = pCtrlCMC->bInterpolationGammaStyleEnable;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        //pIspCMC->fCmcInterpolationGamma[0] = pCtrlCMC->dInterpolationGamma[0];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pIspCMC->afGammaLUT[0][iIdx] = pCtrlCMC->afGammaLUT[0][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        //pIspCMC->fCmcInterpolationGamma[1] = pCtrlCMC->dInterpolationGamma[1];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pIspCMC->afGammaLUT[1][iIdx] = pCtrlCMC->afGammaLUT[1][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        //pIspCMC->fCmcInterpolationGamma[2] = pCtrlCMC->dInterpolationGamma[2];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                        for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                            pIspCMC->afGammaLUT[2][iIdx] = pCtrlCMC->afGammaLUT[2][iIdx];
                        }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fCmcInterpolationGamma[3] = pCtrlCMC->dInterpolationGamma[3];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //    for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                    //        pIspCMC->afGammaLUT[3][iIdx] = pCtrlCMC->afGammaLUT[3][iIdx];
                    //    }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fCmcInterpolationGamma[4] = pCtrlCMC->dInterpolationGamma[4];
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //    for (iIdx = 0; iIdx < ISP_CMC_INTERPOLATION_GMA_PNT_CNT; iIdx++) {
                    //        pIspCMC->afGammaLUT[4][iIdx] = pCtrlCMC->afGammaLUT[4][iIdx];
                    //    }
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
                    //}
                    pIspCMC->fR2YSaturationExpect = pCtrlCMC->dR2YSaturationExpect;
                    pIspCMC->fShaStrengthExpect = pCtrlCMC->dShaStrengthExpect;
                    //pCtrlCMC->bShaStrengthChange = pIspCMC->bShaStrengthChange;
                    //if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fShaStrengthOffset_acm = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][0];
                    //}
                    //if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fShaStrengthOffset_acm_lv1 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][1];
                    //}
                    //if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fShaStrengthOffset_acm_lv2 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][2];
                    //}
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fShaStrengthOffset_acm_lv3 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][3];
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->fShaStrengthOffset_acm_lv4 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_ADVANCE][4];
                    //}
                    //if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->fShaStrengthOffset_fcm = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][0];
                    //}
                    //if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->fShaStrengthOffset_fcm_lv1 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][1];
                    //}
                    //if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->fShaStrengthOffset_fcm_lv2 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][2];
                    //}
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->fShaStrengthOffset_fcm_lv3 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][3];
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->fShaStrengthOffset_fcm_lv4 = pCtrlCMC->dShaStrengthOffset[ISP_CMC_COLOR_MODE_FLAT][4];
                    //}
                    pIspCMC->bCmcNightModeDetectEnable = pCtrlCMC->bCmcNightModeDetectEnable;
                    pIspCMC->fCmcNightModeDetectBrightnessEnter = pCtrlCMC->dCmcNightModeDetectBrightnessEnter;
                    pIspCMC->fCmcNightModeDetectBrightnessExit = pCtrlCMC->dCmcNightModeDetectBrightnessExit;
                    pIspCMC->fCmcNightModeDetectGainEnter = pCtrlCMC->dCmcNightModeDetectGainEnter;
                    pIspCMC->fCmcNightModeDetectGainExit = pCtrlCMC->dCmcNightModeDetectGainExit;
                    pIspCMC->fCmcNightModeDetectWeighting = pCtrlCMC->dCmcNightModeDetectWeighting;

                    pIspCMC->bEnableCaptureIQ = pCtrlCMC->bCaptureModeIqParamChangeEnable;
                    pIspCMC->fCmcCaptureModeCCMRatio = pCtrlCMC->dCaptureModeCCMRatio;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pIspCMC->fCmcCaptureR2YRangeMul[iIdx] = pCtrlCMC->adCaptureModeR2YRangeMult[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pIspCMC->iCmcCaptureInY[iIdx] = pCtrlCMC->aiCaptureModeTnmInY[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pIspCMC->iCmcCaptureOutY[iIdx] = pCtrlCMC->aiCaptureModeTnmOutY[iIdx];
                    }
                    for (iIdx = 0; iIdx < 2; iIdx++) {
                        pIspCMC->iCmcCaptureOutC[iIdx] = pCtrlCMC->aiCaptureModeTnmOutC[iIdx];
                    }

                    //uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiNoOfGainLevelUsed;
                    pIspCMC->targetBrightness_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTargetBrightness;
                    pIspCMC->fAeTargetMin_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dAeTargetMin;
                    pIspCMC->fSensorMaxGain_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSensorMaxGain;
                    pIspCMC->fBrightness_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBrightness;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pIspCMC->aRangeMult_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].adRangeMult[iIdx];
                    }
                    pIspCMC->bEnableGamma_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bEnableGamma;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pIspCMC->ui32DnTargetIdx_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[0];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[0][iIdx];
                        }
                        pIspCMC->fDnsStrength_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[0];
                        pIspCMC->fRadius_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[0];
                        pIspCMC->fStrength_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[0];
                        pIspCMC->fThreshold_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[0];
                        pIspCMC->fDetail_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[0];
                        pIspCMC->fEdgeScale_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[0];
                        pIspCMC->fEdgeOffset_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[0];
                        pIspCMC->bBypassDenoise_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[0];
                        pIspCMC->fDenoiseTau_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[0];
                        pIspCMC->fDenoiseSigma_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[0];
                        pIspCMC->fContrast_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[0];
                        pIspCMC->fSaturation_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[0];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[0][iIdx];
                        }
                        pIspCMC->fDpfWeight_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[0];
                        pIspCMC->iDpfThreshold_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[0];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[0][iIdx];
                        }
                        pIspCMC->fFlatFactor_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[0];
                        pIspCMC->fWeightLine_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[0];
                        pIspCMC->fColourConfidence_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[0];
                        pIspCMC->fColourSaturation_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[0];
                        pIspCMC->fEqualBrightSupressRatio_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[0];
                        pIspCMC->fEqualDarkSupressRatio_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[0];
                        pIspCMC->fOvershotThreshold_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[0];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_acm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[0][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[0];
                        pIspCMC->fTnmGamma_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[0];
                        pIspCMC->fBezierCtrlPnt_acm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[0];
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pIspCMC->ui32DnTargetIdx_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[1];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[1][iIdx];
                        }
                        pIspCMC->fDnsStrength_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[1];
                        pIspCMC->fRadius_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[1];
                        pIspCMC->fStrength_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[1];
                        pIspCMC->fThreshold_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[1];
                        pIspCMC->fDetail_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[1];
                        pIspCMC->fEdgeScale_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[1];
                        pIspCMC->fEdgeOffset_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[1];
                        pIspCMC->bBypassDenoise_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[1];
                        pIspCMC->fDenoiseTau_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[1];
                        pIspCMC->fDenoiseSigma_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[1];
                        pIspCMC->fContrast_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[1];
                        pIspCMC->fSaturation_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[1];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[1][iIdx];
                        }
                        pIspCMC->fDpfWeight_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[1];
                        pIspCMC->iDpfThreshold_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[1];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[1][iIdx];
                        }
                        pIspCMC->fFlatFactor_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[1];
                        pIspCMC->fWeightLine_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[1];
                        pIspCMC->fColourConfidence_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[1];
                        pIspCMC->fColourSaturation_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[1];
                        pIspCMC->fEqualBrightSupressRatio_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[1];
                        pIspCMC->fEqualDarkSupressRatio_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[1];
                        pIspCMC->fOvershotThreshold_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[1];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_acm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[1][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[1];
                        pIspCMC->fTnmGamma_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[1];
                        pIspCMC->fBezierCtrlPnt_acm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[1];
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                        pIspCMC->ui32DnTargetIdx_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[2];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[2][iIdx];
                        }
                        pIspCMC->fDnsStrength_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[2];
                        pIspCMC->fRadius_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[2];
                        pIspCMC->fStrength_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[2];
                        pIspCMC->fThreshold_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[2];
                        pIspCMC->fDetail_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[2];
                        pIspCMC->fEdgeScale_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[2];
                        pIspCMC->fEdgeOffset_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[2];
                        pIspCMC->bBypassDenoise_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[2];
                        pIspCMC->fDenoiseTau_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[2];
                        pIspCMC->fDenoiseSigma_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[2];
                        pIspCMC->fContrast_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[2];
                        pIspCMC->fSaturation_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[2];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[2][iIdx];
                        }
                        pIspCMC->fDpfWeight_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[2];
                        pIspCMC->iDpfThreshold_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[2];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[2][iIdx];
                        }
                        pIspCMC->fFlatFactor_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[2];
                        pIspCMC->fWeightLine_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[2];
                        pIspCMC->fColourConfidence_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[2];
                        pIspCMC->fColourSaturation_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[2];
                        pIspCMC->fEqualBrightSupressRatio_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[2];
                        pIspCMC->fEqualDarkSupressRatio_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[2];
                        pIspCMC->fOvershotThreshold_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[2];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_acm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[2][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[2];
                        pIspCMC->fTnmGamma_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[2];
                        pIspCMC->fBezierCtrlPnt_acm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[2];
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->ui32DnTargetIdx_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[3];
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pIspCMC->ui32BlcSensorBlack_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[3][iIdx];
                    //    }
                    //    pIspCMC->fDnsStrength_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[3];
                    //    pIspCMC->fRadius_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[3];
                    //    pIspCMC->fStrength_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[3];
                    //    pIspCMC->fThreshold_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[3];
                    //    pIspCMC->fDetail_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[3];
                    //    pIspCMC->fEdgeScale_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[3];
                    //    pIspCMC->fEdgeOffset_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[3];
                    //    pIspCMC->bBypassDenoise_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[3];
                    //    pIspCMC->fDenoiseTau_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[3];
                    //    pIspCMC->fDenoiseSigma_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[3];
                    //    pIspCMC->fContrast_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[3];
                    //    pIspCMC->fSaturation_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[3];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridStartCoords_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridTileDimensions_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[3][iIdx];
                    //    }
                    //    pIspCMC->fDpfWeight_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[3];
                    //    pIspCMC->iDpfThreshold_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[3];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iInY_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutY_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutC_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[3][iIdx];
                    //    }
                    //    pIspCMC->fFlatFactor_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[3];
                    //    pIspCMC->fWeightLine_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[3];
                    //    pIspCMC->fColourConfidence_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[3];
                    //    pIspCMC->fColourSaturation_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[3];
                    //    pIspCMC->fEqualBrightSupressRatio_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[3];
                    //    pIspCMC->fEqualDarkSupressRatio_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[3];
                    //    pIspCMC->fOvershotThreshold_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[3];
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrCeiling_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrFloor_acm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[3][iIdx];
                    //    }
                    //    pIspCMC->ui32GammaCrvMode_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[3];
                    //    pIspCMC->fTnmGamma_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[3];
                    //    pIspCMC->fBezierCtrlPnt_acm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[3];
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_ADVANCE]) {
                    //    pIspCMC->ui32DnTargetIdx_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].uiDnTargetIdx[4];
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pIspCMC->ui32BlcSensorBlack_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iBlcSensorBlack[4][iIdx];
                    //    }
                    //    pIspCMC->fDnsStrength_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDnsStrength[4];
                    //    pIspCMC->fRadius_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dRadius[4];
                    //    pIspCMC->fStrength_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dStrength[4];
                    //    pIspCMC->fThreshold_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dThreshold[4];
                    //    pIspCMC->fDetail_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDetail[4];
                    //    pIspCMC->fEdgeScale_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeScale[4];
                    //    pIspCMC->fEdgeOffset_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEdgeOffset[4];
                    //    pIspCMC->bBypassDenoise_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].bBypassDenoise[4];
                    //    pIspCMC->fDenoiseTau_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseTau[4];
                    //    pIspCMC->fDenoiseSigma_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDenoiseSigma[4];
                    //    pIspCMC->fContrast_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dContrast[4];
                    //    pIspCMC->fSaturation_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dSaturation[4];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridStartCoords_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridStartCoords[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridTileDimensions_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGridTileDimensions[4][iIdx];
                    //    }
                    //    pIspCMC->fDpfWeight_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dDpfWeight[4];
                    //    pIspCMC->iDpfThreshold_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iDpfThreshold[4];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iInY_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iInY[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutY_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutY[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutC_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iOutC[4][iIdx];
                    //    }
                    //    pIspCMC->fFlatFactor_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dFlatFactor[4];
                    //    pIspCMC->fWeightLine_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWeightLine[4];
                    //    pIspCMC->fColourConfidence_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourConfidence[4];
                    //    pIspCMC->fColourSaturation_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dColourSaturation[4];
                    //    pIspCMC->fEqualBrightSupressRatio_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualBrightSuppressRatio[4];
                    //    pIspCMC->fEqualDarkSupressRatio_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dEqualDarkSuppressRatio[4];
                    //    pIspCMC->fOvershotThreshold_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dOvershotThreshold[4];
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrCeiling_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrCeiling[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrFloor_acm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dWdrFloor[4][iIdx];
                    //    }
                    //    pIspCMC->ui32GammaCrvMode_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].iGammaCrvMode[4];
                    //    pIspCMC->fTnmGamma_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dTnmGamma[4];
                    //    pIspCMC->fBezierCtrlPnt_acm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_ADVANCE].dBezierCtrlPnt[4];
                    //}
                    //uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiNoOfGainLevelUsed;
                    pIspCMC->targetBrightness_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTargetBrightness;
                    pIspCMC->fAeTargetMin_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dAeTargetMin;
                    pIspCMC->fSensorMaxGain_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSensorMaxGain;
                    pIspCMC->fBrightness_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBrightness;
                    for (iIdx = 0; iIdx < 3; iIdx++) {
                        pIspCMC->aRangeMult_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].adRangeMult[iIdx];
                    }
                    pIspCMC->bEnableGamma_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bEnableGamma;
                    if (0 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pIspCMC->ui32DnTargetIdx_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[0];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[0][iIdx];
                        }
                        pIspCMC->fDnsStrength_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[0];
                        pIspCMC->fRadius_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[0];
                        pIspCMC->fStrength_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[0];
                        pIspCMC->fThreshold_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[0];
                        pIspCMC->fDetail_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[0];
                        pIspCMC->fEdgeScale_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[0];
                        pIspCMC->fEdgeOffset_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[0];
                        pIspCMC->bBypassDenoise_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[0];
                        pIspCMC->fDenoiseTau_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[0];
                        pIspCMC->fDenoiseSigma_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[0];
                        pIspCMC->fContrast_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[0];
                        pIspCMC->fSaturation_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[0];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[0][iIdx];
                        }
                        pIspCMC->fDpfWeight_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[0];
                        pIspCMC->iDpfThreshold_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[0];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[0][iIdx];
                        }
                        pIspCMC->fFlatFactor_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[0];
                        pIspCMC->fWeightLine_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[0];
                        pIspCMC->fColourConfidence_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[0];
                        pIspCMC->fColourSaturation_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[0];
                        pIspCMC->fEqualBrightSupressRatio_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[0];
                        pIspCMC->fEqualDarkSupressRatio_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[0];
                        pIspCMC->fOvershotThreshold_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[0];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[0][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_fcm[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[0][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[0];
                        pIspCMC->fTnmGamma_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[0];
                        pIspCMC->fBezierCtrlPnt_fcm = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[0];
                    }
                    if (1 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pIspCMC->ui32DnTargetIdx_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[1];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[1][iIdx];
                        }
                        pIspCMC->fDnsStrength_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[1];
                        pIspCMC->fRadius_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[1];
                        pIspCMC->fStrength_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[1];
                        pIspCMC->fThreshold_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[1];
                        pIspCMC->fDetail_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[1];
                        pIspCMC->fEdgeScale_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[1];
                        pIspCMC->fEdgeOffset_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[1];
                        pIspCMC->bBypassDenoise_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[1];
                        pIspCMC->fDenoiseTau_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[1];
                        pIspCMC->fDenoiseSigma_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[1];
                        pIspCMC->fContrast_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[1];
                        pIspCMC->fSaturation_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[1];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[1][iIdx];
                        }
                        pIspCMC->fDpfWeight_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[1];
                        pIspCMC->iDpfThreshold_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[1];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[1][iIdx];
                        }
                        pIspCMC->fFlatFactor_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[1];
                        pIspCMC->fWeightLine_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[1];
                        pIspCMC->fColourConfidence_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[1];
                        pIspCMC->fColourSaturation_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[1];
                        pIspCMC->fEqualBrightSupressRatio_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[1];
                        pIspCMC->fEqualDarkSupressRatio_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[1];
                        pIspCMC->fOvershotThreshold_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[1];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[1][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_fcm_lv1[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[1][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[1];
                        pIspCMC->fTnmGamma_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[1];
                        pIspCMC->fBezierCtrlPnt_fcm_lv1 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[1];
                    }
                    if (2 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                        pIspCMC->ui32DnTargetIdx_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[2];
                        for (iIdx = 0; iIdx < 4; iIdx++) {
                            pIspCMC->ui32BlcSensorBlack_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[2][iIdx];
                        }
                        pIspCMC->fDnsStrength_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[2];
                        pIspCMC->fRadius_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[2];
                        pIspCMC->fStrength_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[2];
                        pIspCMC->fThreshold_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[2];
                        pIspCMC->fDetail_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[2];
                        pIspCMC->fEdgeScale_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[2];
                        pIspCMC->fEdgeOffset_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[2];
                        pIspCMC->bBypassDenoise_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[2];
                        pIspCMC->fDenoiseTau_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[2];
                        pIspCMC->fDenoiseSigma_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[2];
                        pIspCMC->fContrast_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[2];
                        pIspCMC->fSaturation_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[2];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridStartCoords_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iGridTileDimensions_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[2][iIdx];
                        }
                        pIspCMC->fDpfWeight_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[2];
                        pIspCMC->iDpfThreshold_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[2];
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iInY_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutY_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < 2; iIdx++) {
                            pIspCMC->iOutC_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[2][iIdx];
                        }
                        pIspCMC->fFlatFactor_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[2];
                        pIspCMC->fWeightLine_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[2];
                        pIspCMC->fColourConfidence_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[2];
                        pIspCMC->fColourSaturation_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[2];
                        pIspCMC->fEqualBrightSupressRatio_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[2];
                        pIspCMC->fEqualDarkSupressRatio_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[2];
                        pIspCMC->fOvershotThreshold_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[2];
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrCeiling_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[2][iIdx];
                        }
                        for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            pIspCMC->fWdrFloor_fcm_lv2[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[2][iIdx];
                        }
                        pIspCMC->ui32GammaCrvMode_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[2];
                        pIspCMC->fTnmGamma_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[2];
                        pIspCMC->fBezierCtrlPnt_fcm_lv2 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[2];
                    }
                    //if (3 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->ui32DnTargetIdx_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[3];
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pIspCMC->ui32BlcSensorBlack_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[3][iIdx];
                    //    }
                    //    pIspCMC->fDnsStrength_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[3];
                    //    pIspCMC->fRadius_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[3];
                    //    pIspCMC->fStrength_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[3];
                    //    pIspCMC->fThreshold_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[3];
                    //    pIspCMC->fDetail_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[3];
                    //    pIspCMC->fEdgeScale_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[3];
                    //    pIspCMC->fEdgeOffset_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[3];
                    //    pIspCMC->bBypassDenoise_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[3];
                    //    pIspCMC->fDenoiseTau_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[3];
                    //    pIspCMC->fDenoiseSigma_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[3];
                    //    pIspCMC->fContrast_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[3];
                    //    pIspCMC->fSaturation_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[3];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridStartCoords_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridTileDimensions_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[3][iIdx];
                    //    }
                    //    pIspCMC->fDpfWeight_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[3];
                    //    pIspCMC->iDpfThreshold_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[3];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iInY_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutY_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutC_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[3][iIdx];
                    //    }
                    //    pIspCMC->fFlatFactor_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[3];
                    //    pIspCMC->fWeightLine_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[3];
                    //    pIspCMC->fColourConfidence_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[3];
                    //    pIspCMC->fColourSaturation_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[3];
                    //    pIspCMC->fEqualBrightSupressRatio_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[3];
                    //    pIspCMC->fEqualDarkSupressRatio_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[3];
                    //    pIspCMC->fOvershotThreshold_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[3];
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrCeiling_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[3][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrFloor_fcm_lv3[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[3][iIdx];
                    //    }
                    //    pIspCMC->ui32GammaCrvMode_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[3];
                    //    pIspCMC->fTnmGamma_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[3];
                    //    pIspCMC->fBezierCtrlPnt_fcm_lv3 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[3];
                    //}
                    //if (4 < uiNoOfGainLevelUsed[ISP_CMC_COLOR_MODE_FLAT]) {
                    //    pIspCMC->ui32DnTargetIdx_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].uiDnTargetIdx[4];
                    //    for (iIdx = 0; iIdx < 4; iIdx++) {
                    //        pIspCMC->ui32BlcSensorBlack_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iBlcSensorBlack[4][iIdx];
                    //    }
                    //    pIspCMC->fDnsStrength_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDnsStrength[4];
                    //    pIspCMC->fRadius_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dRadius[4];
                    //    pIspCMC->fStrength_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dStrength[4];
                    //    pIspCMC->fThreshold_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dThreshold[4];
                    //    pIspCMC->fDetail_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDetail[4];
                    //    pIspCMC->fEdgeScale_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeScale[4];
                    //    pIspCMC->fEdgeOffset_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEdgeOffset[4];
                    //    pIspCMC->bBypassDenoise_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].bBypassDenoise[4];
                    //    pIspCMC->fDenoiseTau_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseTau[4];
                    //    pIspCMC->fDenoiseSigma_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDenoiseSigma[4];
                    //    pIspCMC->fContrast_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dContrast[4];
                    //    pIspCMC->fSaturation_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dSaturation[4];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridStartCoords_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridStartCoords[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iGridTileDimensions_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGridTileDimensions[4][iIdx];
                    //    }
                    //    pIspCMC->fDpfWeight_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dDpfWeight[4];
                    //    pIspCMC->iDpfThreshold_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iDpfThreshold[4];
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutY_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iInY[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutY_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutY[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < 2; iIdx++) {
                    //        pIspCMC->iOutC_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iOutC[4][iIdx];
                    //    }
                    //    pIspCMC->fFlatFactor_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dFlatFactor[4];
                    //    pIspCMC->fWeightLine_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWeightLine[4];
                    //    pIspCMC->fColourConfidence_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourConfidence[4];
                    //    pIspCMC->fColourSaturation_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dColourSaturation[4];
                    //    pIspCMC->fEqualBrightSupressRatio_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualBrightSuppressRatio[4];
                    //    pIspCMC->fEqualDarkSupressRatio_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dEqualDarkSuppressRatio[4];
                    //    pIspCMC->fOvershotThreshold_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dOvershotThreshold[4];
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrCeiling_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrCeiling[4][iIdx];
                    //    }
                    //    for (iIdx = 0; iIdx < TNMC_WDR_SEGMENT_CNT; iIdx++) {
                    //        pIspCMC->fWdrFloor_fcm_lv4[iIdx] = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dWdrFloor[4][iIdx];
                    //    }
                    //    pIspCMC->ui32GammaCrvMode_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].iGammaCrvMode[4];
                    //    pIspCMC->fTnmGamma_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dTnmGamma[4];
                    //    pIspCMC->fBezierCtrlPnt_fcm_lv4 = pCtrlCMC->stCMC[ISP_CMC_COLOR_MODE_FLAT].dBezierCtrlPnt[4];
                    //}
#if 1
                    //pIspCMC->enableControl(pCtrlCMC->bEnable);
                    pIspCMC->bCmcEnable = pCtrlCMC->bCmcEnable;
#endif
                    pIspCMC->bCmcHasChange = true;
                    pIspCMC->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetTNMCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_TNM* pCtrlTNM = (ISP_CTRL_TNM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlTNM *pIspTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlTNM || !pIspTNMC) {
        LOGE("V2500 get TNMC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlTNM=%p, pIspTNMC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlTNM, pIspTNMC);
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
            if ((ISP_TNMC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_TNMC_CMD_VER_V0:
                default:
                    pCtrlTNM->bEnable = pIspTNMC->isEnabled();
                    pCtrlTNM->dAdaptiveStrength = pIspTNMC->getAdaptiveStrength();
                    pCtrlTNM->dHistMin = pIspTNMC->getHistMin();
                    pCtrlTNM->dHistMax = pIspTNMC->getHistMax();
                    pCtrlTNM->dSmoothing = pIspTNMC->getSmoothing();
                    pCtrlTNM->dTempering = pIspTNMC->getTempering();
                    pCtrlTNM->dUpdateSpeed = pIspTNMC->getUpdateSpeed();
                    pIspTNMC->getHistogram(pCtrlTNM->dHistogram);
                    pIspTNMC->getMappingCurve(pCtrlTNM->dMappingCurve);
                    pCtrlTNM->bLocalTNM = pIspTNMC->getLocalTNM();
                    pCtrlTNM->bAdaptiveTNM = pIspTNMC->getAdaptiveTNM();
                    pCtrlTNM->bAdaptiveTNM_FCM = pIspTNMC->getAdaptiveTNM_FCM();
                    pCtrlTNM->dLocalStrength = pIspTNMC->getLocalStrength();
                    pCtrlTNM->bConfigureHis = pIspTNMC->getAllowHISConfig();
                    pCtrlTNM->iSmoothHistogramMethod = pIspTNMC->getSmoothHistogramMethod();
                    pCtrlTNM->bEnableEqualization = pIspTNMC->isEqualizationEnabled();
                    pCtrlTNM->bEnableGamma = pIspTNMC->isGammaEnabled();
                    pCtrlTNM->dEqualMaxBrightSupress = pIspTNMC->getEqualMaxBrightSupress();
                    pCtrlTNM->dEqualMaxDarkSupress = pIspTNMC->getEqualMaxDarkSupress();
                    pCtrlTNM->dEqualBrightSupressRatio = pIspTNMC->getEqualBrightSupressRatio();
                    pCtrlTNM->dEqualDarkSupressRatio = pIspTNMC->getEqualDarkSupressRatio();
                    pCtrlTNM->dEqualFactor = pIspTNMC->getEqualFactor();
                    pCtrlTNM->dOvershotThreshold = pIspTNMC->getOvershotThreshold();
                    pIspTNMC->getWdrCeiling(pCtrlTNM->dWdrCeiling);
                    pIspTNMC->getWdrFloor(pCtrlTNM->dWdrFloor);
                    pCtrlTNM->dMapCurveUpdateDamp = pIspTNMC->getMapCurveUpdateDamp();
                    pCtrlTNM->dMapCurvePowerValue = pIspTNMC->getMapCurvePowerValue();
                    pCtrlTNM->iGammaCrvMode = pIspTNMC->getGammaCurveMode();
                    pCtrlTNM->dGammaACM = pIspTNMC->getGammaACM();
                    pCtrlTNM->dGammaFCM = pIspTNMC->getGammaFCM();
                    pCtrlTNM->dBezierCtrlPnt = pIspTNMC->getBezierCtrlPnt();
                    pCtrlTNM->bTnmCrvSimBrightnessContrastEnable = pIspTNMC->isTnmCrvSimBrightnessContrastEnabled();
                    pCtrlTNM->dTnmCrvBrightness = pIspTNMC->getTnmCrvBrightness();
                    pCtrlTNM->dTnmCrvContrast = pIspTNMC->getTnmCrvContrast();
                    if (ISP_TNMC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_CTRL_TNM);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        int iIdx;

                        LOGE("V2500 get TNMC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get TNMC.bEnable: %d\n", pCtrlTNM->bEnable);
                        LOGE("V2500 get TNMC.dAdaptiveStrength: %lf\n", pCtrlTNM->dAdaptiveStrength);
                        LOGE("V2500 get TNMC.dHistMin: %lf\n", pCtrlTNM->dHistMin);
                        LOGE("V2500 get TNMC.dHistMax: %lf\n", pCtrlTNM->dHistMax);
                        LOGE("V2500 get TNMC.dSmoothing: %lf\n", pCtrlTNM->dSmoothing);
                        LOGE("V2500 get TNMC.dTempering: %lf\n", pCtrlTNM->dTempering);
                        LOGE("V2500 get TNMC.dUpdateSpeed: %lf\n", pCtrlTNM->dUpdateSpeed);
                        LOGE("V2500 get TNMC.dHistogram[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_N_HIST; iIdx++) {
                            if (((ISP_TNMC_N_HIST - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dHistogram[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dHistogram[iIdx]);
                            }
                        }
                        LOGE("V2500 get TNMC.dMappingCurve[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_N_CURVE; iIdx++) {
                            if (((ISP_TNMC_N_CURVE - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dMappingCurve[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dMappingCurve[iIdx]);
                            }
                        }
                        LOGE("V2500 get TNMC.bLocalTNM: %d\n", pCtrlTNM->bLocalTNM);
                        LOGE("V2500 get TNMC.bAdaptiveTNM: %d\n", pCtrlTNM->bAdaptiveTNM);
                        LOGE("V2500 get TNMC.bAdaptiveTNM_FCM: %d\n", pCtrlTNM->bAdaptiveTNM_FCM);
                        LOGE("V2500 get TNMC.dLocalStrength: %lf\n", pCtrlTNM->dLocalStrength);
                        LOGE("V2500 get TNMC.bConfigureHis: %d\n", pCtrlTNM->bConfigureHis);
                        LOGE("V2500 get TNMC.iSmoothHistogramMethod: %d\n", pCtrlTNM->iSmoothHistogramMethod);
                        LOGE("V2500 get TNMC.bEnableEqualization: %d\n", pCtrlTNM->bEnableEqualization);
                        LOGE("V2500 get TNMC.bEnableGamma: %d\n", pCtrlTNM->bEnableGamma);
                        LOGE("V2500 get TNMC.dEqualMaxBrightSupress: %lf\n", pCtrlTNM->dEqualMaxBrightSupress);
                        LOGE("V2500 get TNMC.dEqualMaxDarkSupress: %lf\n", pCtrlTNM->dEqualMaxDarkSupress);
                        LOGE("V2500 get TNMC.dEqualBrightSupressRatio: %lf\n", pCtrlTNM->dEqualBrightSupressRatio);
                        LOGE("V2500 get TNMC.dEqualDarkSupressRatio: %lf\n", pCtrlTNM->dEqualDarkSupressRatio);
                        LOGE("V2500 get TNMC.dEqualFactor: %lf\n", pCtrlTNM->dEqualFactor);
                        LOGE("V2500 get TNMC.dOvershotThreshold: %lf\n", pCtrlTNM->dOvershotThreshold);
                        LOGE("V2500 get TNMC.dWdrCeiling[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            if (((ISP_TNMC_WDR_SEGMENT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dWdrCeiling[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dWdrCeiling[iIdx]);
                            }
                        }
                        LOGE("V2500 get TNMC.dWdrFloor[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            if (((ISP_TNMC_WDR_SEGMENT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dWdrFloor[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dWdrFloor[iIdx]);
                            }
                        }
                        LOGE("V2500 get TNMC.dMapCurveUpdateDamp: %lf\n", pCtrlTNM->dMapCurveUpdateDamp);
                        LOGE("V2500 get TNMC.dMapCurvePowerValue: %lf\n", pCtrlTNM->dMapCurvePowerValue);
                        LOGE("V2500 get TNMC.iGammaCrvMode: %d\n", pCtrlTNM->iGammaCrvMode);
                        LOGE("V2500 get TNMC.dGammaACM: %lf\n", pCtrlTNM->dGammaACM);
                        LOGE("V2500 get TNMC.dGammaFCM: %lf\n", pCtrlTNM->dGammaFCM);
                        LOGE("V2500 get TNMC.dBezierCtrlPnt: %lf\n", pCtrlTNM->dBezierCtrlPnt);
                        LOGE("V2500 get TNMC.bTnmCrvSimBrightnessContrastEnable: %d\n", pCtrlTNM->bTnmCrvSimBrightnessContrastEnable);
                        LOGE("V2500 get TNMC.dTnmCrvBrightness: %lf\n", pCtrlTNM->dTnmCrvBrightness);
                        LOGE("V2500 get TNMC.dTnmCrvContrast: %lf\n", pCtrlTNM->dTnmCrvContrast);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetTNMCParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_CTRL_TNM* pCtrlTNM = (ISP_CTRL_TNM*)((unsigned char*)pData + sizeof(TCPCommand));
    ISPC::ControlTNM *pIspTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    int iRet = -1;

    if (!m_Cam[Idx].pCamera || !m_Cam[Idx].pCamera->sensor || !pTcp || !pCtrlTNM || !pIspTNMC) {
        LOGE("V2500 set TNMC failed - Camera=%p, Sensor=%p, Tcp=%p, pCtrlTNM=%p, pIspTNMC=%p\n",
             m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor, pTcp, pCtrlTNM, pIspTNMC);
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
            if ((ISP_TNMC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_TNMC_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        int iIdx;

                        LOGE("V2500 set TNMC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set TNMC.bEnable: %d\n", pCtrlTNM->bEnable);
                        //LOGE("V2500 set TNMC.dAdaptiveStrength: %lf\n", pCtrlTNM->dAdaptiveStrength);
                        LOGE("V2500 set TNMC.dHistMin: %lf\n", pCtrlTNM->dHistMin);
                        LOGE("V2500 set TNMC.dHistMax: %lf\n", pCtrlTNM->dHistMax);
                        LOGE("V2500 set TNMC.dSmoothing: %lf\n", pCtrlTNM->dSmoothing);
                        LOGE("V2500 set TNMC.dTempering: %lf\n", pCtrlTNM->dTempering);
                        LOGE("V2500 set TNMC.dUpdateSpeed: %lf\n", pCtrlTNM->dUpdateSpeed);
                        //LOGE("V2500 set TNMC.dHistogram[]:\n");
                        //for (iIdx = 0; iIdx < ISP_TNMC_N_HIST; iIdx++) {
                        //    if (((ISP_TNMC_N_HIST - 1) == iIdx) ||
                        //        (15 == (iIdx % 16))) {
                        //        LOGE("%5.3lf\n", pCtrlTNM->dHistogram[iIdx]);
                        //    } else {
                        //        LOGE("%5.3lf ", pCtrlTNM->dHistogram[iIdx]);
                        //    }
                        //}
                        //LOGE("V2500 set TNMC.dMappingCurve[]:\n");
                        //for (iIdx = 0; iIdx < ISP_TNMC_N_CURVE; iIdx++) {
                        //    if (((ISP_TNMC_N_CURVE - 1) == iIdx) ||
                        //        (15 == (iIdx % 16))) {
                        //        LOGE("%5.3lf\n", pCtrlTNM->dMappingCurve[iIdx]);
                        //    } else {
                        //        LOGE("%5.3lf ", pCtrlTNM->dMappingCurve[iIdx]);
                        //    }
                        //}
                        LOGE("V2500 set TNMC.bLocalTNM: %d\n", pCtrlTNM->bLocalTNM);
                        LOGE("V2500 set TNMC.bAdaptiveTNM: %d\n", pCtrlTNM->bAdaptiveTNM);
                        LOGE("V2500 set TNMC.bAdaptiveTNM_FCM: %d\n", pCtrlTNM->bAdaptiveTNM_FCM);
                        LOGE("V2500 set TNMC.dLocalStrength: %lf\n", pCtrlTNM->dLocalStrength);
                        LOGE("V2500 set TNMC.bConfigureHis: %d\n", pCtrlTNM->bConfigureHis);
                        LOGE("V2500 set TNMC.iSmoothHistogramMethod: %d\n", pCtrlTNM->iSmoothHistogramMethod);
                        LOGE("V2500 set TNMC.bEnableEqualization: %d\n", pCtrlTNM->bEnableEqualization);
                        LOGE("V2500 set TNMC.bEnableGamma: %d\n", pCtrlTNM->bEnableGamma);
                        LOGE("V2500 set TNMC.dEqualMaxBrightSupress: %lf\n", pCtrlTNM->dEqualMaxBrightSupress);
                        LOGE("V2500 set TNMC.dEqualMaxDarkSupress: %lf\n", pCtrlTNM->dEqualMaxDarkSupress);
                        LOGE("V2500 set TNMC.dEqualBrightSupressRatio: %lf\n", pCtrlTNM->dEqualBrightSupressRatio);
                        LOGE("V2500 set TNMC.dEqualDarkSupressRatio: %lf\n", pCtrlTNM->dEqualDarkSupressRatio);
                        LOGE("V2500 set TNMC.dEqualFactor: %lf\n", pCtrlTNM->dEqualFactor);
                        LOGE("V2500 set TNMC.dOvershotThreshold: %lf\n", pCtrlTNM->dOvershotThreshold);
                        LOGE("V2500 set TNMC.dWdrCeiling[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            if (((ISP_TNMC_WDR_SEGMENT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dWdrCeiling[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dWdrCeiling[iIdx]);
                            }
                        }
                        LOGE("V2500 set TNMC.dWdrFloor[]:\n");
                        for (iIdx = 0; iIdx < ISP_TNMC_WDR_SEGMENT_CNT; iIdx++) {
                            if (((ISP_TNMC_WDR_SEGMENT_CNT - 1) == iIdx) ||
                                (15 == (iIdx % 16))) {
                                LOGE("%5.3lf\n", pCtrlTNM->dWdrFloor[iIdx]);
                            } else {
                                LOGE("%5.3lf ", pCtrlTNM->dWdrFloor[iIdx]);
                            }
                        }
                        LOGE("V2500 set TNMC.dMapCurveUpdateDamp: %lf\n", pCtrlTNM->dMapCurveUpdateDamp);
                        LOGE("V2500 set TNMC.dMapCurvePowerValue: %lf\n", pCtrlTNM->dMapCurvePowerValue);
                        LOGE("V2500 set TNMC.iGammaCrvMode: %d\n", pCtrlTNM->iGammaCrvMode);
                        LOGE("V2500 set TNMC.dGammaACM: %lf\n", pCtrlTNM->dGammaACM);
                        LOGE("V2500 set TNMC.dGammaFCM: %lf\n", pCtrlTNM->dGammaFCM);
                        LOGE("V2500 set TNMC.dBezierCtrlPnt: %lf\n", pCtrlTNM->dBezierCtrlPnt);
                        LOGE("V2500 set TNMC.bTnmCrvSimBrightnessContrastEnable: %d\n", pCtrlTNM->bTnmCrvSimBrightnessContrastEnable);
                        LOGE("V2500 set TNMC.dTnmCrvBrightness: %lf\n", pCtrlTNM->dTnmCrvBrightness);
                        LOGE("V2500 set TNMC.dTnmCrvContrast: %lf\n", pCtrlTNM->dTnmCrvContrast);
                    }

                    pIspTNMC->enableControl(pCtrlTNM->bEnable);
                    //pIspTNMC->adaptiveStrength = pCtrlTNM->getAdaptiveStrength();
                    pIspTNMC->setHistMin(pCtrlTNM->dHistMin);
                    pIspTNMC->setHistMax(pCtrlTNM->dHistMax);
                    pIspTNMC->setSmoothing(pCtrlTNM->dSmoothing);
                    pIspTNMC->setTempering(pCtrlTNM->dTempering);
                    pIspTNMC->setUpdateSpeed(pCtrlTNM->dUpdateSpeed);
                    //pIspTNMC->getHistogram(pCtrlTNM->dHistogram);
                    //pIspTNMC->getMappingCurve(pCtrlTNM->dMappingCurve);
                    pIspTNMC->enableLocalTNM(pCtrlTNM->bLocalTNM);
                    pIspTNMC->enableAdaptiveTNM(pCtrlTNM->bAdaptiveTNM);
                    pIspTNMC->enableAdaptiveTNM_FCM(pCtrlTNM->bAdaptiveTNM_FCM);
                    pIspTNMC->setLocalStrength(pCtrlTNM->dLocalStrength);
                    pIspTNMC->setAllowHISConfig(pCtrlTNM->bConfigureHis);
                    pIspTNMC->setSmoothHistogramMethod(pCtrlTNM->iSmoothHistogramMethod);
                    pIspTNMC->enableEqualization(pCtrlTNM->bEnableEqualization);
                    pIspTNMC->enableGamma(pCtrlTNM->bEnableGamma);
                    pIspTNMC->setEqualMaxBrightSupress(pCtrlTNM->dEqualMaxBrightSupress);
                    pIspTNMC->setEqualMaxDarkSupress(pCtrlTNM->dEqualMaxDarkSupress);
                    pIspTNMC->setEqualBrightSupressRatio(pCtrlTNM->dEqualBrightSupressRatio);
                    pIspTNMC->setEqualDarkSupressRatio(pCtrlTNM->dEqualDarkSupressRatio);
                    pIspTNMC->setEqualFactor(pCtrlTNM->dEqualFactor);
                    pIspTNMC->setOvershotThreshold(pCtrlTNM->dOvershotThreshold);
                    pIspTNMC->setWdrCeiling(pCtrlTNM->dWdrCeiling);
                    pIspTNMC->setWdrFloor(pCtrlTNM->dWdrFloor);
                    pIspTNMC->setMapCurveUpdateDamp(pCtrlTNM->dMapCurveUpdateDamp);
                    pIspTNMC->setMapCurvePowerValue(pCtrlTNM->dMapCurvePowerValue);
                    pIspTNMC->setGammaCurveMode(pCtrlTNM->iGammaCrvMode);
                    pIspTNMC->setGammaACM(pCtrlTNM->dGammaACM);
                    pIspTNMC->setGammaFCM(pCtrlTNM->dGammaFCM);
                    pIspTNMC->setBezierCtrlPnt(pCtrlTNM->dBezierCtrlPnt);
                    pIspTNMC->enableTnmCrvSimBrightnessContrast(pCtrlTNM->bTnmCrvSimBrightnessContrastEnable);
                    pIspTNMC->setTnmCrvBrightness(pCtrlTNM->dTnmCrvBrightness);
                    pIspTNMC->setTnmCrvContrast(pCtrlTNM->dTnmCrvContrast);
                    pIspTNMC->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetIICParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    IIC_MDLE_IIC* pModIIC = (IIC_MDLE_IIC*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;
    int iIICHandle;
    char szIICDevice[64] = { 0 };
    struct i2c_rdwr_ioctl_data stIICPackets;
    struct i2c_msg stIICMessages[2];
    unsigned char szSendBuf[64] = { 0 };
    unsigned char szRecvBuf[2] = { 0 };

    if (!pTcp ||
        !pModIIC ||
        //(0 == pModIIC->iBus) ||
        (0 > pModIIC->iBus) ||
        (5 < pModIIC->iBus) ||
        (0 == pModIIC->iAddrNumOfBytes) ||
        (2 < pModIIC->iAddrNumOfBytes) ||
        (0 == pModIIC->iDataNumOfBytes) ||
        (2 < pModIIC->iDataNumOfBytes) ||
        (0 == pModIIC->iSalveAddr)
        ) {
        LOGE("V2500 get IIC failed - Tcp=%p, pModIIC=%p, iBus=%d, iAddrNumOfBytes=%d, iDataNumOfBytes=%d, iSalveAddr=%02X\n",
             pTcp, pModIIC, pModIIC->iBus, pModIIC->iAddrNumOfBytes, pModIIC->iDataNumOfBytes, pModIIC->iSalveAddr);
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
            if ((ISP_IIC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_IIC_CMD_VER_V0:
                default:
                    sprintf(szIICDevice, "/dev/i2c-%d", pModIIC->iBus);
                    iIICHandle = open(szIICDevice, O_RDWR);
                    if (0 > iIICHandle) {
                        LOGE("V2500 get IIC : open %s failed!\n", szIICDevice);
                        return iRet;
                    }

                    if (2 == pModIIC->iAddrNumOfBytes) {
                        szSendBuf[0] = (unsigned char)((pModIIC->iRegAddr >> 8) & 0x00ff);
                        szSendBuf[1] = (unsigned char)(pModIIC->iRegAddr & 0x00ff);
                    } else {
                        szSendBuf[0] = (unsigned char)(pModIIC->iRegAddr & 0x00ff);
                    }
                    stIICMessages[0].addr  = (pModIIC->iSalveAddr >> 1);
                    stIICMessages[0].flags = 0;
                    stIICMessages[0].len   = pModIIC->iAddrNumOfBytes;
                    stIICMessages[0].buf   = szSendBuf;
                    stIICMessages[1].addr  = (pModIIC->iSalveAddr >> 1);
                    stIICMessages[1].flags = I2C_M_RD;
                    stIICMessages[1].len   = pModIIC->iDataNumOfBytes;
                    stIICMessages[1].buf   = szRecvBuf;

                    stIICPackets.msgs = stIICMessages;
                    stIICPackets.nmsgs = 2;

                    iRet = ioctl(iIICHandle, I2C_RDWR, &stIICPackets);
                    if (0 > iRet) {
                        if (2 == pModIIC->iAddrNumOfBytes) {
                            LOGE("V2500 get IIC : i2c read ioctl failed %d, iSalveAddr=0x%02X, iRegAddr=0x%04X, iAddrNumOfBytes=%d\n", iRet, pModIIC->iSalveAddr, pModIIC->iRegAddr, pModIIC->iAddrNumOfBytes);
                        } else {
                            LOGE("V2500 get IIC : i2c read ioctl failed %d, iSalveAddr=0x%02X, iRegAddr=0x%02X, iAddrNumOfBytes=%d\n", iRet, pModIIC->iSalveAddr, pModIIC->iRegAddr, pModIIC->iAddrNumOfBytes);
                        }
                        return iRet;
                    }

                    if (2 == pModIIC->iDataNumOfBytes) {
                        pModIIC->iRegValue = szRecvBuf[0];
                        pModIIC->iRegValue <<= 8;
                        pModIIC->iRegValue |= szRecvBuf[1];
                    } else {
                        pModIIC->iRegValue = szRecvBuf[0];
                    }

                    if (ISP_IIC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(IIC_MDLE_IIC);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get IIC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get IIC.iDebug: %d\n", pModIIC->iDebug);
                        LOGE("V2500 get IIC.iBus: %d\n", pModIIC->iBus);
                        LOGE("V2500 get IIC.iAddrNumOfBytes: %d\n", pModIIC->iAddrNumOfBytes);
                        LOGE("V2500 get IIC.iDataNumOfBytes: %d\n", pModIIC->iDataNumOfBytes);
                        LOGE("V2500 get IIC.iSalveAddr: %02X\n", pModIIC->iSalveAddr);
                        if (2 == pModIIC->iAddrNumOfBytes) {
                            LOGE("V2500 get IIC.iRegAddr: %04X\n", pModIIC->iRegAddr);
                        } else {
                            LOGE("V2500 get IIC.iRegAddr: %02X\n", pModIIC->iRegAddr);
                        }
                        if (2 == pModIIC->iDataNumOfBytes) {
                            LOGE("V2500 get IIC.iRegValue: %04X\n", pModIIC->iRegValue);
                        } else {
                            LOGE("V2500 get IIC.iRegValue: %02X\n", pModIIC->iRegValue);
                        }
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetIICParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    IIC_MDLE_IIC* pModIIC = (IIC_MDLE_IIC*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;
    int iIICHandle;
    char szIICDevice[64] = { 0 };
    struct i2c_rdwr_ioctl_data stIICPackets;
    struct i2c_msg stIICMessages[2];
    unsigned char szSendBuf[64] = { 0 };
    int iIdx;

    if (!pTcp ||
        !pModIIC ||
        //(0 == pModIIC->iBus) ||
        (0 > pModIIC->iBus) ||
        (5 < pModIIC->iBus) ||
        (0 == pModIIC->iAddrNumOfBytes) ||
        (2 < pModIIC->iAddrNumOfBytes) ||
        (0 == pModIIC->iDataNumOfBytes) ||
        (2 < pModIIC->iDataNumOfBytes) ||
        (0 == pModIIC->iSalveAddr)
        ) {
        LOGE("V2500 set IIC failed - Tcp=%p, pModIIC=%p, iBus=%d, iAddrNumOfBytes=%d, iDataNumOfBytes=%d, iSalveAddr=%02X\n",
             pTcp, pModIIC, pModIIC->iBus, pModIIC->iAddrNumOfBytes, pModIIC->iDataNumOfBytes, pModIIC->iSalveAddr);
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
            if ((ISP_IIC_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_IIC_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set IIC.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set IIC.iDebug: %d\n", pModIIC->iDebug);
                        LOGE("V2500 set IIC.iBus: %d\n", pModIIC->iBus);
                        LOGE("V2500 set IIC.iAddrNumOfBytes: %d\n", pModIIC->iAddrNumOfBytes);
                        LOGE("V2500 set IIC.iDataNumOfBytes: %d\n", pModIIC->iDataNumOfBytes);
                        LOGE("V2500 set IIC.iSalveAddr: %02X\n", pModIIC->iSalveAddr);
                        if (2 == pModIIC->iAddrNumOfBytes) {
                            LOGE("V2500 set IIC.iRegAddr: %04X\n", pModIIC->iRegAddr);
                        } else {
                            LOGE("V2500 set IIC.iRegAddr: %02X\n", pModIIC->iRegAddr);
                        }
                        if (2 == pModIIC->iDataNumOfBytes) {
                            LOGE("V2500 set IIC.iRegValue: %04X\n", pModIIC->iRegValue);
                        } else {
                            LOGE("V2500 set IIC.iRegValue: %02X\n", pModIIC->iRegValue);
                        }
                    }

                    sprintf(szIICDevice, "/dev/i2c-%d", pModIIC->iBus);
                    iIICHandle = open(szIICDevice, O_RDWR);
                    if (0 > iIICHandle) {
                        LOGE("V2500 set IIC : open %s failed!\n", szIICDevice);
                        return iRet;
                    }

                    iIdx = 0;
                    if (2 == pModIIC->iAddrNumOfBytes) {
                        szSendBuf[iIdx++] = (unsigned char)((pModIIC->iRegAddr >> 8) & 0x00ff);
                        szSendBuf[iIdx++] = (unsigned char)(pModIIC->iRegAddr & 0x00ff);
                    } else {
                        szSendBuf[iIdx++] = (unsigned char)(pModIIC->iRegAddr & 0x00ff);
                    }
                    if (2 == pModIIC->iDataNumOfBytes) {
                        szSendBuf[iIdx++] = (unsigned char)((pModIIC->iRegValue >> 8) & 0x00ff);
                        szSendBuf[iIdx++] = (unsigned char)(pModIIC->iRegValue & 0x00ff);
                    } else {
                        szSendBuf[iIdx++] = (unsigned char)(pModIIC->iRegValue & 0x00ff);
                    }
                    stIICMessages[0].addr  = (pModIIC->iSalveAddr >> 1);
                    stIICMessages[0].flags = 0;
                    stIICMessages[0].len   = pModIIC->iAddrNumOfBytes + pModIIC->iDataNumOfBytes;
                    stIICMessages[0].buf   = szSendBuf;

                    stIICPackets.msgs = stIICMessages;
                    stIICPackets.nmsgs = 1;

                    iRet = ioctl(iIICHandle, I2C_RDWR, &stIICPackets);
                    if (2 == pModIIC->iAddrNumOfBytes) {
                        LOGE("V2500 set IIC : i2c write ioctl failed %d, iSalveAddr=0x%02X, iRegAddr=0x%04X, iAddrNumOfBytes=%d, iRegValue=0x%04X, iDataNumOfBytes=%d\n", iRet, pModIIC->iSalveAddr, pModIIC->iRegAddr, pModIIC->iAddrNumOfBytes, pModIIC->iRegValue, pModIIC->iDataNumOfBytes);
                    } else {
                        LOGE("V2500 set IIC : i2c write ioctl failed %d, iSalveAddr=0x%02X, iRegAddr=0x%02X, iAddrNumOfBytes=%d, iRegValue=0x%02X, iDataNumOfBytes=%d\n", iRet, pModIIC->iSalveAddr, pModIIC->iRegAddr, pModIIC->iAddrNumOfBytes, pModIIC->iRegValue, pModIIC->iDataNumOfBytes);
                    }
                    //iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetOutParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_OUT* pModOUT = (ISP_MDLE_OUT*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (!pTcp || !pModOUT) {
        LOGE("V2500 get OUT failed - Tcp=%p, pModOUT=%p\n",
             pTcp, pModOUT);
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
            if ((ISP_OUT_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_OUT_CMD_VER_V0:
                default:
                    pModOUT->iDataExtractionPoint = m_eDataExtractionPoint;
                    if (ISP_BLC_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_BLC);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get OUT.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 get OUT.iDataExtractionPoint: %d\n", pModOUT->iDataExtractionPoint);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetOutParam(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_OUT* pModOUT = (ISP_MDLE_OUT*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (!pTcp || !pModOUT) {
        LOGE("V2500 set OUT failed - Tcp=%p, pModOUT=%p\n",
             pTcp, pModOUT);
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
            if ((ISP_OUT_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_OUT_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set OUT.datasize: %u\n", pTcp->datasize);
                        LOGE("V2500 set OUT.iDataExtractionPoint: %d\n", pModOUT->iDataExtractionPoint);
                    }

                    m_eDataExtractionPoint = (CI_INOUT_POINTS)pModOUT->iDataExtractionPoint;
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetIspVersion(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_VER* pModVer = (ISP_MDLE_VER*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (!pTcp || !pModVer) {
        LOGE("V2500 get ISP version failed - Tcp=%p, pModVer=%p\n", pTcp, pModVer);
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
            if ((ISP_VER_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_VER_CMD_VER_V0:
                default:
                    pModVer->iVersion = ISP_VERSION_V2500;
                    if (ISP_VER_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_VER);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set Version.iVersion = %d\n", pModVer->iVersion);
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetIqTuningDebugInfo(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DBG* pModDbg = (ISP_MDLE_DBG*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (!pTcp || !pModDbg) {
        LOGE("V2500 get Debug failed - Tcp=%p, pModVer=%p\n", pTcp, pModDbg);
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
            if ((ISP_DBG_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DBG_CMD_VER_V0:
                default:
                    pModDbg->bDebugEnable = m_bIqTuningDebugInfoEnable;
                    if (ISP_DBG_CMD_VER_V0 == pTcp->ver) {
                        pTcp->datasize = sizeof(ISP_MDLE_DBG);
                    }

                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 get Debug.bDebugEnable = %s\n", ((pModDbg->bDebugEnable) ? "true" : "false"));
                    }
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::SetIqTuningDebugInfo(IMG_UINT32 Idx, void* pData)
{
    TCPCommand* pTcp = (TCPCommand*)pData;
    ISP_MDLE_DBG* pModDbg = (ISP_MDLE_DBG*)((unsigned char*)pData + sizeof(TCPCommand));
    int iRet = -1;

    if (!pTcp || !pModDbg) {
        LOGE("V2500 set Debug failed - Tcp=%p, pModVer=%p\n", pTcp, pModDbg);
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
            if ((ISP_DBG_CMD_VER_MAX - 1) < pTcp->ver) {
                LOGE("Out of support version. Ver = %lu\n", pTcp->ver);
                return iRet;
            }
            switch (pTcp->ver) {
                case ISP_DBG_CMD_VER_V0:
                default:
                    if (m_bIqTuningDebugInfoEnable) {
                        LOGE("V2500 set Debug.bDebugEnable = %s\n", ((pModDbg->bDebugEnable) ? "true" : "false"));
                    }

                    m_bIqTuningDebugInfoEnable = pModDbg->bDebugEnable;
                    iRet = 0;
                    break;
            }
            break;
    }

    return iRet;
}

int IPU_V2500::GetIQ(IMG_UINT32 Idx, void* pdata)
{
    TCPCommand* pTcp = (TCPCommand*)pdata;
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("V2500get_iq_mod: %u\n", pTcp->mod);
        LOGE("V2500get_iq_cmd.ui3Type: %lu\n", pTcp->cmd.ui3Type);
    }

    switch (pTcp->mod - MODID_ISP_BASE) {
        case ISP_IIF:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_EXS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_BLC:
            iRet = GetBLCParam(Idx, pdata);
            break;

        case ISP_RLT:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_LSH:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_WBC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_FOS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DNS:
            iRet = GetDNSParam(Idx, pdata);
            break;

        case ISP_DPF:
            iRet = GetDPFParam(Idx, pdata);
            break;

        case ISP_ENS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_LCA:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_CCM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_MGM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_GMA:
            iRet = GetGMAParam(Idx, pdata);
            break;

        case ISP_WBS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_HIS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_R2Y:
            iRet = GetR2YParam(Idx, pdata);
            break;

        case ISP_MIE:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_VIB:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_TNM:
            iRet = GetTNMParam(Idx, pdata);
            break;

        case ISP_FLD:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_SHA:
            iRet = GetSHAParam(Idx, pdata);
            break;

        case ISP_ESC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DSC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_Y2R:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DGM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_WBC_CCM:
            iRet = GetWBCAndCCMParam(Idx, pdata);
            break;

        case ISP_AEC:
            iRet = GetAECParam(Idx, pdata);
            break;

        case ISP_AWB:
            iRet = GetAWBParam(Idx, pdata);
            break;

        case ISP_CMC:
            iRet = GetCMCParam(Idx, pdata);
            break;

        case ISP_TNMC:
            iRet = GetTNMCParam(Idx, pdata);
            break;

        case ISP_IIC:
            iRet = GetIICParam(Idx, pdata);
            break;

        case ISP_OUT:
            iRet = GetOutParam(Idx, pdata);
            break;

        case ISP_VER:
            iRet = GetIspVersion(Idx, pdata);
            break;

        case ISP_DBG:
            iRet = GetIqTuningDebugInfo(Idx, pdata);
            break;

        default:
            LOGE("Unknown module.\n");
            break;
    }

    return iRet;
}

int IPU_V2500::SetIQ(IMG_UINT32 Idx, void* pdata)
{
    TCPCommand* pTcp = (TCPCommand*)pdata;
    int iRet = -1;

    if (m_bIqTuningDebugInfoEnable) {
        LOGE("V2500set_iq_mod: %u\n", pTcp->mod);
        LOGE("V2500set_iq_cmd.ui3Type: %lu\n", pTcp->cmd.ui3Type);
    }

    switch (pTcp->mod - MODID_ISP_BASE) {
        case ISP_IIF:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_EXS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_BLC:
            iRet = SetBLCParam(Idx, pdata);
            break;

        case ISP_RLT:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_LSH:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_WBC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_FOS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DNS:
            iRet = SetDNSParam(Idx, pdata);
            break;

        case ISP_DPF:
            iRet = SetDPFParam(Idx, pdata);
            break;

        case ISP_ENS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_LCA:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_CCM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_MGM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_GMA:
            iRet = SetGMAParam(Idx, pdata);
            break;

        case ISP_WBS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_HIS:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_R2Y:
            iRet = SetR2YParam(Idx, pdata);
            break;

        case ISP_MIE:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_VIB:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_TNM:
            iRet = SetTNMParam(Idx, pdata);
            break;

        case ISP_FLD:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_SHA:
            iRet = SetSHAParam(Idx, pdata);
            break;

        case ISP_ESC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DSC:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_Y2R:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DGM:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_WBC_CCM:
            iRet = SetWBCAndCCMParam(Idx, pdata);
            break;

        case ISP_AEC:
            iRet = SetAECParam(Idx, pdata);
            break;

        case ISP_AWB:
            iRet = SetAWBParam(Idx, pdata);
            break;

        case ISP_CMC:
            iRet = SetCMCParam(Idx, pdata);
            break;

        case ISP_TNMC:
            iRet = SetTNMCParam(Idx, pdata);
            break;

        case ISP_IIC:
            iRet = SetIICParam(Idx, pdata);
            break;

        case ISP_OUT:
            iRet = SetOutParam(Idx, pdata);
            break;

        case ISP_VER:
            LOGE("Does not support at this version.\n");
            break;

        case ISP_DBG:
            iRet = SetIqTuningDebugInfo(Idx, pdata);
            break;

        default:
            LOGE("Unknown module.\n");
            break;
    }

    return iRet;
}

int IPU_V2500::BackupIspParameters(IMG_UINT32 Idx, ISPC::ParameterList & Parameters)
{

    if (IMG_SUCCESS == m_Cam[Idx].pCamera->saveParameters(Parameters)) {
        // Remove sensor parameters and group.
        Parameters.removeParameter("SENSOR_ACTIVE_SIZE");
        Parameters.removeParameter("SENSOR_BITDEPTH");
        Parameters.removeParameter("SENSOR_EXPOSURE_MS");
        Parameters.removeParameter("SENSOR_FRAME_RATE");
        Parameters.removeParameter("SENSOR_GAIN");
        Parameters.removeParameter("SENSOR_READ_NOISE");
        Parameters.removeParameter("SENSOR_VTOT");
        Parameters.removeParameter("SENSOR_WELL_DEPTH");
        Parameters.removeGroup("Sensor");
    } else {
        LOGE("V2500 failed to backup ISP parameters\n");
    }

    return 0;
}

int IPU_V2500::SaveIspParameters(IMG_UINT32 Idx, cam_save_data_t *saveData)
{
    static int savedFrames = 0;
    struct timeval stTv;
    struct tm* stTm;
    std::stringstream outputName;
#if defined(USE_ORIGINAL_SAVE_PARAMETERS)
    ISPC::ParameterList parameters;
#endif //#if defined(USE_ORIGINAL_SAVE_PARAMETERS)
    //ISPC::ModuleBLC *pIspMdleBLC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleBLC>();
    //ISPC::ModuleCCM *pIspMdleCCM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleCCM>();
    //ISPC::ModuleDNS *pIspMdleDNS = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    //ISPC::ModuleDPF *pIspMdleDPF = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();
    //ISPC::ModuleGMA *pIspMdleGMA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    //ISPC::ModuleHIS *pIspMdleHIS = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleHIS>();
    //ISPC::ModuleIIF *pIspMdleIIF = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleIIF>();
    //ISPC::ModuleLCA *pIspMdleLCA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleLCA>();
    //ISPC::ModuleLSH *pIspMdleLSH = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleLSH>();
    //ISPC::ModuleOUT *pIspMdleOUT = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleOUT>();
    //ISPC::ModuleR2Y *pIspMdleR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    //ISPC::ModuleRLT *pIspMdleRLT = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleRLT>();
    //ISPC::ModuleSHA *pIspMdleSHA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    //ISPC::ModuleTNM *pIspMdleTNM = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    //ISPC::ModuleWBC *pIspMdleWBC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();
    //ISPC::ModuleWBS *pIspMdleWBS = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBS>();
    //ISPC::ControlAE *pIspCtrlAE = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAE>();
    //ISPC::ControlAWB_PID *pIspCtrlAWB = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlAWB_PID>();
    ISPC::ControlCMC *pIspCtrlCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    //ISPC::ControlLSH *pIspCtrlLSH = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlLSH>();
    //ISPC::ControlTNM *pIspCtrlTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    int iRet = -1;

    if (((void *)0) == saveData) {
        LOGE("(%s,L%d) V2500 save ISP parameters failed - saveData is NULL !!!\n", __func__, __LINE__);
        return iRet;
    }
    if (!m_Cam[Idx].pCamera) {
        LOGE("(%s,L%d) V2500 save ISP parameters failed - Camera=%p\n", __func__, __LINE__, m_Cam[Idx].pCamera);
        return iRet;
    }

    gettimeofday(&stTv,NULL);
    stTm = localtime(&stTv.tv_sec);
#if 0
    sprintf(saveData->file_name, "sensor%d-isp-config_save_%03d_%04d%02d%02d_%02d%02d%02d_%03d.txt",
        Idx,
        savedFrames,
        stTm->tm_year + 1900,
        stTm->tm_mon + 1,
        stTm->tm_mday,
        stTm->tm_hour,
        stTm->tm_min,
        stTm->tm_sec,
        (int)(stTv.tv_usec / 1000)
        );
#else
    sprintf(saveData->file_name, "sensor%d-isp-config_save_%03d_%04d%02d%02d_%02d%02d%02d.txt",
        Idx,
        savedFrames,
        stTm->tm_year + 1900,
        stTm->tm_mon + 1,
        stTm->tm_mday,
        stTm->tm_hour,
        stTm->tm_min,
        stTm->tm_sec
        );
#endif
    outputName.str("");
    outputName << saveData->file_path << saveData->file_name;

#if defined(USE_ORIGINAL_SAVE_PARAMETERS)
    if (IMG_SUCCESS == m_Cam[Idx].pCamera->saveParameters(parameters)) {
        ISPC::ParameterFileParser::saveGrouped(parameters, outputName.str());
        LOGE("saving parameters to %s\n",
            outputName.str().c_str());
        savedFrames++;
        iRet = 0;
    } else {
        LOGE("failed to save parameters\n");
    }
#else
    if ((pIspCtrlCMC) && (pIspCtrlCMC->bCmcEnable)) {
        pIspCtrlCMC->save(m_Parameters[Idx], ISPC::ModuleBase::SAVE_VAL);
    }
    ISPC::ParameterFileParser::saveGrouped(m_Parameters[Idx], outputName.str());
    LOGE("saving parameters to %s\n",
        outputName.str().c_str());
    savedFrames++;
    iRet = 0;
#endif //#if defined(USE_ORIGINAL_SAVE_PARAMETERS)

    return iRet;
}
