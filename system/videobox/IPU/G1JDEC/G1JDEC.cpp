/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Jpeg HardWare Decode
 *
 * Author:
 *        devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/21/2017 init
 */

#include <System.h>
#include <qsdk/videobox.h>
#include "G1JDEC.h"

#define ALLOCBUFSIZE 0x80000
static char s_ps8FrBufName[32];

//#define DUMP

DYNAMIC_IPU(IPU_G1JDEC, "g1jdec");

IPU_G1JDEC::IPU_G1JDEC(std::string name, Json *js)
{
    Name = name;
    Mode = IPU_G1JDEC::Auto;
    const char *pu8String = NULL;

    if (NULL != js)
    {
        pu8String = js->GetString("mode");
        if (pu8String != NULL && strncmp(pu8String, "trigger", 8) == 0)
        {
            Mode = IPU_G1JDEC::Trigger;
        }
        else
        {
            Mode = IPU_G1JDEC::Auto;
        }
    }

    pIn = CreatePort("stream", Port::In);
    pIn->SetPixelFormat(Pixel::Format::MJPEG);

    pOut = CreatePort("frame", Port::Out);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    pOut->SetResolution(1920, 1088);
    pOut->SetBufferType(FRBuffer::Type::FIXED, 3);
    pOut->SetPixelFormat(Pixel::Format::NV12);
    pOut->Export();

}

void IPU_G1JDEC::Prepare()
{

    if (GetPort("stream")->GetPixelFormat() != Pixel::Format::MJPEG)
    {
        LOGE("stream Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    s32LumSize = 0;
    s32ChromaSize = 0;
    s32ResChange = 0;
    s64FrameCount = 0;
    bDecodeFinish = false;
    bTrigger = false;
    bPPEnble = false;

    memset(&stJpegDecIn, 0, sizeof(JpegDecInput));
    memset(&stJpegDecOut, 0, sizeof(JpegDecOutput));
    memset(&stImageInfo, 0, sizeof(JpegDecImageInfo));
    memset(&stJDecInBuffer, 0, sizeof(Buffer));
    pJpegDecInst = NULL;
    pPPInst = NULL;
    stJDecInFRBuf = NULL;
    stJDecOutFRBuf = NULL;

    if (!pIn->IsBind())
    {
        sprintf(s_ps8FrBufName, "%s-stream", Name.c_str());
        if (stJDecInFRBuf == NULL)
        {
            LOGE("alloc stJDecInFRBuf %s\n",s_ps8FrBufName);
            stJDecInFRBuf = new FRBuffer(s_ps8FrBufName, FRBuffer::Type::FIXED_NODROP,  3, ALLOCBUFSIZE);
            if (stJDecInFRBuf ==  NULL)
            {
                LOGE("Allocate jpeg decoder buffer (%s) fail!\n",s_ps8FrBufName);
                return;
            }
        }
    }

}

IPU_G1JDEC::~IPU_G1JDEC()
{

}

void IPU_G1JDEC::Unprepare()
{
    if (pIn && pIn->IsEnabled())
    {
        pIn->FreeVirtualResource();
    }
    if (pOut && pOut->IsEnabled())
    {
        pOut->FreeVirtualResource();
    }
    if (pPPInst)
    {
        PPRelease(pPPInst);
        pPPInst = NULL;
    }
    if (pJpegDecInst)
    {
        JpegDecRelease(pJpegDecInst);
        pJpegDecInst = NULL;
    }
    if (stJDecInFRBuf)
    {
        delete stJDecInFRBuf;
        stJDecInFRBuf = NULL;
    }

}

void IPU_G1JDEC::WorkLoop()
{
    while (IsInState(ST::Running))
    {
        bDecodeFinish = false;
        if (Mode == IPU_G1JDEC::Trigger)
        {
            if (!bTrigger)
            {
                usleep(10 * 1000);
                continue;
            }
        }

        try
        {
            if (pIn->IsBind())
            {
                stBufferIn = pIn->GetBuffer(&stBufferIn);
            }
            else
            {
                stBufferIn = stJDecInFRBuf->GetReferenceBuffer(&stBufferIn);
            }
        }
        catch(const char *ps8Error)
        {
            usleep(10 * 1000);
            continue;
        }
        LOGD("FrameCount:%lld\n", s64FrameCount);
        /*Hardware decoding*/
        StartJpegDecode();

        s64FrameCount++;
        if (enJpegDecRet != JPEGDEC_OK)
        {
            LOGE("Jpeg decode fail! Ret(%d)\n",enJpegDecRet);
        }

        bDecodeFinish = true;
        if (pIn->IsBind())
        {
            pIn->PutBuffer(&stBufferIn);
        }
        else
        {
            stJDecInFRBuf->PutReferenceBuffer(&stBufferIn);
        }
        bTrigger = false;
        if (Mode == IPU_G1JDEC::Trigger)
        {
            LOGE("End trigger!\n");
        }
    }
}

JpegDecRet IPU_G1JDEC::StartJpegDecode()
{
    int s32CombinedFlag = 0;

    stBufferOut.fr_buf.virt_addr = NULL;
    enJpegDecRet = JpegDecInit(&pJpegDecInst, NULL, 0);
    if (enJpegDecRet != JPEGDEC_OK)
    {
        LOGE("Jpeg decoder init fail! %d\n",enJpegDecRet);
        enJpegDecRet = JPEGDEC_ERROR;
        goto error;
    }

    /*feed mjpeg data to decoder*/
    stJDecInBuffer.virt_addr = stBufferIn.fr_buf.virt_addr;
    stJDecInBuffer.phys_addr = stBufferIn.fr_buf.phys_addr;
    stJDecInBuffer.size = stBufferIn.fr_buf.size;

    stJpegDecIn.streamBuffer.pVirtualAddress = (u32 *)stJDecInBuffer.virt_addr;
    stJpegDecIn.streamBuffer.busAddress = stJDecInBuffer.phys_addr;
    stJpegDecIn.streamLength = stJDecInBuffer.size;
    stJpegDecIn.bufferSize = 0;

    /*get jpeg base info*/
    enJpegDecRet = JpegDecGetImageInfo(pJpegDecInst, &stJpegDecIn, &stImageInfo);
    if (enJpegDecRet == JPEGDEC_OK)
    {
        CalcSize();
        LOGD("outImgW:%d outImgH:%d format:0x%x disImgW:%d disImgH:%d\n",
            stImageInfo.outputWidth, stImageInfo.outputHeight, stImageInfo.outputFormat, stImageInfo.displayWidth, stImageInfo.displayHeight);
    }
    else
    {
        LOGE("Get Image Info fail!(%d)\n",enJpegDecRet);
        enJpegDecRet = JPEGDEC_ERROR;
        goto error;
    }

    stParamsIn.s32Width = stImageInfo.outputWidth;
    stParamsIn.s32Height = stImageInfo.outputHeight;
    stParamsIn.s32Format = stImageInfo.outputFormat;

    switch(stParamsIn.s32Format)
    {
        case JPEGDEC_YCbCr422_SEMIPLANAR:
            stParamsIn.s32Format = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
            break;
        case JPEGDEC_YCbCr420_SEMIPLANAR:
            stParamsIn.s32Format = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
            break;
        default:
            LOGE("format change need add\n");
            //other format need support,here just support nv12 and yuv422
            break;
    }

    stParamsOut.s32Width = pOut->GetWidth();
    stParamsOut.s32Height = pOut->GetHeight();
    stParamsOut.s32Format = pOut->GetPixelFormat();
    switch(stParamsOut.s32Format)
    {
        case Pixel::Format::NV12:
            stParamsOut.s32Format = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
            break;
        case Pixel::Format::YUV422SP:
            stParamsOut.s32Format = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
            break;
        default:
            LOGE("output format not support(%d)!\n", stParamsOut.s32Format);
            enJpegDecRet = JPEGDEC_ERROR;
            goto error;
    }

    if ((stParamsIn.s32Width != stParamsOut.s32Width ||
        stParamsIn.s32Height != stParamsOut.s32Height ||
        stParamsIn.s32Format != stParamsOut.s32Format) &&
        stParamsOut.s32Width > 0 &&
        stParamsOut.s32Height > 0)
    {
        bPPEnble = true;
    }
    else
    {
        bPPEnble = false;
    }

    if (bPPEnble)
    {
        switch(stParamsOut.s32Format)
        {
            case PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR:
                stParamsOut.s32LumSize = stParamsOut.s32Width*stParamsOut.s32Height;
                stParamsOut.s32YUVSize = stParamsOut.s32LumSize*3/2;
                break;
            case PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR:
                stParamsOut.s32LumSize = stParamsOut.s32Width*stParamsOut.s32Height;
                stParamsOut.s32YUVSize = stParamsOut.s32LumSize*2;
                break;
            default:
                LOGE("output format not support(%d)!\n", stParamsOut.s32Format);
                enJpegDecRet = JPEGDEC_ERROR;
                goto error;
        }
    }
    /* Decode Full Image */
    stJpegDecIn.decImageType = JPEGDEC_IMAGE;
    stJpegDecIn.sliceMbSet = 0;

    stBufferOut = pOut->GetBuffer();
    stBufferOut.SyncStamp(&stBufferIn);

    /*pp pipeline*/
    if (bPPEnble)
    {
        enPPDecRet = PPDecInit(&pPPInst);
        if (enPPDecRet != PP_OK)
        {
            LOGE("PP init fail!(%d)\n",enPPDecRet);
            enJpegDecRet = JPEGDEC_ERROR;
            goto error;
        }

        enPPDecRet = PPDecCombinedModeEnable(pPPInst, pJpegDecInst, PP_PIPELINED_DEC_TYPE_JPEG);

        if (enPPDecRet != PP_OK)
        {
            LOGE("Failed to enable PP-DEC pipeline: %d\n",enPPDecRet);
            enJpegDecRet = JPEGDEC_ERROR;
            goto error;
        }
        else
        {
            s32CombinedFlag = 1;
        }

        enPPDecRet = PPDecUpdateConfig();
        if (enPPDecRet != PP_OK)
        {
            LOGE("PP update config fail!\n");
            enJpegDecRet = JPEGDEC_ERROR;
            goto error;
        }
    }

    /*when jdec + pp pipeline disable,decoder will output*/
    if (!bPPEnble)
    {
        stJpegDecIn.pictureBufferY.busAddress= stBufferOut.fr_buf.phys_addr;
        stJpegDecIn.pictureBufferY.pVirtualAddress = (u32 *)(stBufferOut.fr_buf.virt_addr);
        stJpegDecIn.pictureBufferCbCr.busAddress = stBufferOut.fr_buf.phys_addr + s32LumSize;
        stJpegDecIn.pictureBufferCbCr.pVirtualAddress = (u32 *)((char *)stBufferOut.fr_buf.virt_addr + s32LumSize);
    }


    do
    {
        LOGD("start decode!\n");
        enJpegDecRet = JpegDecDecode(pJpegDecInst, &stJpegDecIn, &stJpegDecOut);
        LOGD("end decode!\n");

        switch(enJpegDecRet)
        {
            case JPEGDEC_STRM_PROCESSED:
                LOGD("Jdec stream processed!\n");
                break;
            case JPEGDEC_FRAME_READY:
                LOGD("Jdec frame ready!\n");
                break;
            case JPEGDEC_SLICE_READY:
                LOGD("Jdec slice ready!\n");
                break;
            case JPEGDEC_SCAN_PROCESSED:
                LOGD("Jdec scan processed!\n");
                break;
            default:
                LOGE("Jdec fail:%d\n",enJpegDecRet);
                enJpegDecRet = JPEGDEC_ERROR;
                goto error;
        }
    } while(enJpegDecRet != JPEGDEC_FRAME_READY);

    enJpegDecRet = JPEGDEC_OK;

    LOGD("YAdr:0x%x CbCrAdr:0x%x\n",stJpegDecOut.outputPictureY.pVirtualAddress, stJpegDecOut.outputPictureCbCr.pVirtualAddress);

    if (bPPEnble)
    {
        enPPDecRet = PPGetResult(pPPInst);
        if (enPPDecRet != PP_OK)
        {
            LOGE("pp process fail!(%d)\n",enPPDecRet);
        }

        #ifdef DUMP
        WriteFile(stBufferOut.fr_buf.virt_addr, stParamsOut.s32YUVSize,"/mnt/sd0/G1Jdec_%d_%d_%lld.yuv",
            pPPConf.ppOutImg.width, pPPConf.ppOutImg.height, s64FrameCount);
        #endif
        stBufferOut.fr_buf.size = stParamsOut.s32YUVSize;
        stBufferOut.fr_buf.priv = 0;
    }
    else
    {
        stBufferOut.fr_buf.size = s32LumSize + s32ChromaSize;
        stBufferOut.fr_buf.priv = 0;
    }

    if (s32ResChange)
    {
        stBufferOut.fr_buf.priv = VIDEO_FRAME_DUMMY;
        stBufferOut.fr_buf.stAttr.s32Width = stImageInfo.outputWidth;
        stBufferOut.fr_buf.stAttr.s32Height = stImageInfo.outputHeight;
    }
    else
    {
        stBufferOut.fr_buf.priv = 0;
    }

error:
    if (stBufferOut.fr_buf.virt_addr)
    {
        pOut->PutBuffer(&stBufferOut);

    }
    if (s32CombinedFlag)
    {
        enPPDecRet = PPDecCombinedModeDisable(pPPInst, pJpegDecInst);
    }

    if (pPPInst)
    {
        PPRelease(pPPInst);
        pPPInst = NULL;
    }
    if (pJpegDecInst)
    {
        JpegDecRelease(pJpegDecInst);
        pJpegDecInst = NULL;
    }
    return enJpegDecRet;

}

void IPU_G1JDEC::CalcSize()
{

    int s32Size = 0;

    s32Size = s32LumSize;
    s32LumSize = (stImageInfo.outputWidth * stImageInfo.outputHeight);
    LOGD("info.w:%d info.h:%d\n",stImageInfo.outputWidth, stImageInfo.outputHeight);
    if (s32Size != s32LumSize)
    {
        LOGE("Jpeg size change!\n");
        s32ResChange = 1;
    }
    else
    {
        s32ResChange = 0;
        return;
    }

    if (stImageInfo.outputFormat!= JPEGDEC_YCbCr400)
    {
        if (stImageInfo.outputFormat == JPEGDEC_YCbCr420_SEMIPLANAR ||
           stImageInfo.outputFormat == JPEGDEC_YCbCr411_SEMIPLANAR)
        {
            s32ChromaSize = (s32LumSize / 2);
        }
        else if (stImageInfo.outputFormat == JPEGDEC_YCbCr444_SEMIPLANAR)
        {
            s32ChromaSize = s32LumSize * 2;
        }
        else
        {
            s32ChromaSize = s32LumSize;
        }
    }
}

bool IPU_G1JDEC::DecodePhoto(bool* bEnable)
{
    LOGE("Begin to decode:%d\n", *bEnable);
    bTrigger = *bEnable;
    return true;
}

bool IPU_G1JDEC::DecodeFinish(bool* bFinish)
{
    *bFinish = bDecodeFinish;
    if (bDecodeFinish)
    {
        bDecodeFinish = false;
    }
    return true;
}

PPResult IPU_G1JDEC::PPDecInit(PPInst *pPPInst)
{

    if (pPPInst == NULL)
    {
        return PP_PARAM_ERROR;
    }

    enPPDecRet = PPInit(pPPInst, 0);

    if (enPPDecRet != PP_OK)
    {
        LOGE("Failed to init the PP: %d\n",enPPDecRet);
        *pPPInst = NULL;

        return enPPDecRet;
    }

    return PP_OK;
}

PPResult IPU_G1JDEC::PPDecUpdateConfig()
{
    enPPDecRet = PPGetConfig(pPPInst, &pPPConf);
    if (enPPDecRet != PP_OK)
    {
        LOGE("Get pp config fail!\n");
        return enPPDecRet;
    }
    pPPConf.ppInImg.width = stParamsIn.s32Width;
    pPPConf.ppInImg.height = stParamsIn.s32Height;
    pPPConf.ppInImg.pixFormat = stParamsIn.s32Format;
    pPPConf.ppInImg.videoRange = 1;

    pPPConf.ppOutImg.width = stParamsOut.s32Width;
    pPPConf.ppOutImg.height = stParamsOut.s32Height;
    pPPConf.ppOutImg.pixFormat = stParamsOut.s32Format;

    pPPConf.ppOutImg.bufferBusAddr = stBufferOut.fr_buf.phys_addr;
    pPPConf.ppOutImg.bufferChromaBusAddr = stBufferOut.fr_buf.phys_addr + stParamsOut.s32LumSize;

    LOGD("pPPConf.ppInImg.width:%d pPPConf.ppInImg.height:%d pPPConf.ppInImg.pixFormat:0x%x\n",
        pPPConf.ppInImg.width, pPPConf.ppInImg.height, pPPConf.ppInImg.pixFormat);
    LOGD("pPPConf.ppOutImg.width:%d pPPConf.ppOutImg.height:%d pPPConf.ppOutImg.pixFormat:0x%x\n",
        pPPConf.ppOutImg.width, pPPConf.ppOutImg.height, pPPConf.ppOutImg.pixFormat);

    enPPDecRet = PPSetConfig(pPPInst, &pPPConf);
    if (enPPDecRet != PP_OK)
    {
        LOGE("PPSetConfig failed while process an image :%d\n", enPPDecRet);
        return enPPDecRet;
    }

    return PP_OK;
}
