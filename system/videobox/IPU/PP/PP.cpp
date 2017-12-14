// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <System.h>
#include "PP.h"

DYNAMIC_IPU(IPU_PP, "pp");

#define ENABLE 1
#define DISABLE 0

#define PP_PIXEL_FORMAT_YUV420_MASK         0x020000U

int IPU_PP::InstanceCnt = 0;

static int switchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::RGBA8888)    //Pixel::Format::RGBA8888
        return PP_PIX_FMT_BGR32; // 0x41001  PP_PIX_FMT_RGB32
    else if(srcFormat == Pixel::Format::NV12) //Pixel::Format::NV12
        return PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;// 0x20001
    else if(srcFormat == Pixel::Format::NV21) //Pixel::Format::NV21
        return PP_PIX_FMT_YCRCB_4_2_0_SEMIPLANAR;  //0x20003
    else if(srcFormat == Pixel::Format::RGB565)//Pixel::Format::RGB565
        return PP_PIX_FMT_RGB16_5_6_5;
    else if(srcFormat == Pixel::Format::YUYV422)
           return PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
     else
        throw VBERR_BADPARAM;
}

void IPU_PP::SetOutputParam(Port *pt, int type, int enc)
{
    switch(type){
        case STPRM_OLMASK0:
            if(enc != DISABLE){
                pPpConf.ppOutMask1.enable = ENABLE;
                pPpConf.ppOutMask1.width = pt->GetWidth();
                pPpConf.ppOutMask1.height = pt->GetHeight();
                pPpConf.ppOutMask1.alphaBlendEna = ENABLE;
                pPpConf.ppOutMask1.blendOriginX = 0;
                pPpConf.ppOutMask1.blendOriginY = 0;
                pPpConf.ppOutMask1.blendWidth = pt->GetWidth();
                pPpConf.ppOutMask1.blendHeight = pt->GetHeight();

                pPpConf.ppOutMask1.originX = pt->GetPipX();
                pPpConf.ppOutMask1.originY = pt->GetPipY();
            }else{
                pPpConf.ppOutMask1.enable = DISABLE;
            }
            break;
        case STPRM_OLMASK1:
            if(enc != DISABLE){
                pPpConf.ppOutMask2.enable = ENABLE;
                pPpConf.ppOutMask2.width = pt->GetWidth();
                pPpConf.ppOutMask2.height = pt->GetHeight();
                pPpConf.ppOutMask2.alphaBlendEna = ENABLE;
                pPpConf.ppOutMask2.blendOriginX = 0;
                pPpConf.ppOutMask2.blendOriginY = 0;
                pPpConf.ppOutMask2.blendWidth = pt->GetWidth();
                pPpConf.ppOutMask2.blendHeight = pt->GetHeight();

                pPpConf.ppOutMask2.originX = pt->GetPipX();
                pPpConf.ppOutMask2.originY = pt->GetPipY();
            }else{
                pPpConf.ppOutMask2.enable = DISABLE;
            }
            break;
        case STPRM_FRMBUF:
            if(enc != DISABLE){
                pPpConf.ppOutFrmBuffer.enable = ENABLE;
                pPpConf.ppOutFrmBuffer.frameBufferWidth = pt->GetWidth();
                pPpConf.ppOutFrmBuffer.frameBufferHeight= pt->GetHeight();
                pPpConf.ppOutFrmBuffer.writeOriginX = pt->GetPipX();
                pPpConf.ppOutFrmBuffer.writeOriginY = pt->GetPipY();
            }else{
                pPpConf.ppOutFrmBuffer.enable = DISABLE;
            }
            break;
        default:
            LOGE("Set Mask param error!\n");
    }
}

void IPU_PP::WorkLoop()
{
    int skip = 0;
    PPResult ppRet;
    Buffer bfin, bfout, bfol0, bfol1;
    Port *pin, *pout, *pol0in, *pol1in;
    bool ol0Available, ol1Available, ol0EnableFlag, ol1EnableFlag;

    ppRet = PPGetConfig(pPpInst, &(pPpConf));
    if(ppRet != PP_OK) {
        LOGE("PPGetConfig failed while process an image:\n");
        return ;
    }

    pin = GetPort("in");
    pout = GetPort("out");
    pol0in = GetPort("ol0");
    pol1in = GetPort("ol1");

    pPpConf.ppInImg.width = pin->GetWidth();
    pPpConf.ppInImg.height = pin->GetHeight();
    pPpConf.ppInImg.videoRange = 1;
    pPpConf.ppInImg.pixFormat = switchFormat(pin->GetPixelFormat());

    pPpConf.ppOutImg.width = pout->GetPipWidth();
    pPpConf.ppOutImg.height = pout->GetPipHeight();
    pPpConf.ppOutImg.pixFormat = switchFormat(pout->GetPixelFormat());

    pPpConf.ppOutRgb.alpha = 0xFF;
    pPpConf.ppOutRgb.rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_601;

    while(IsInState(ST::Running)){
        ol0EnableFlag = ol1EnableFlag = false;
        ol0Available = ol1Available = false;

        if(pol0in->IsEnabled())
            ol0EnableFlag = true;
        if(pol1in->IsEnabled())
            ol1EnableFlag = true;

        try {
            if(ol0EnableFlag){
                bfol0 = pol0in->GetBuffer();
                SetOutputParam(pol0in, STPRM_OLMASK0, ENABLE);
                ol0Available = true;
            }

            if(ol1EnableFlag){
                bfol1 = pol1in->GetBuffer();
                SetOutputParam(pol1in, STPRM_OLMASK1, ENABLE);
                ol1Available = true;
            }
        } catch (const char* err) {}

        try {
            if(pout->IsEmbezzling()){
                bfin = pin->GetBuffer();
                SetOutputParam(pout, STPRM_FRMBUF, ENABLE);
            }else{
                bfin = pin->GetBuffer(&bfin);
            }
        } catch (const char* err) {
            usleep(20000);
            continue;
        }

        if(skip) {
            pin->PutBuffer(&bfin);
            if(ol0EnableFlag && ol0Available)
                pol0in->PutBuffer(&bfol0);
            if(ol1EnableFlag && ol1Available)
                pol1in->PutBuffer(&bfol1);
        } else {
            try {
                if(pout->IsEmbezzling()){
                    bfout = pout->GetBuffer(&bfout);
                }else{
                    bfout = pout->GetBuffer();
                }
            } catch (const char* err) {
                goto out;
            }

            if((pin->GetPipWidth()!=0) && pin->GetPipHeight()!=0){
                pPpConf.ppInCrop.enable = ENABLE;
                pPpConf.ppInCrop.originX = pin->GetPipX();
                pPpConf.ppInCrop.originY = pin->GetPipY();
                pPpConf.ppInCrop.width = pin->GetPipWidth();
                pPpConf.ppInCrop.height = pin->GetPipHeight();
            }

            if(GetRotateValue() != PP_ROTATION_NONE){
                pPpConf.ppInRotation.rotation = GetRotateValue();
            }

            pPpConf.ppInImg.bufferBusAddr = bfin.fr_buf.phys_addr;
            pPpConf.ppInImg.bufferCbBusAddr = pPpConf.ppInImg.bufferBusAddr
                    + pPpConf.ppInImg.width * pPpConf.ppInImg.height;
            pPpConf.ppInImg.bufferCrBusAddr = pPpConf.ppInImg.bufferCbBusAddr
                    + (pPpConf.ppInImg.width * pPpConf.ppInImg.height) / 4;

            pPpConf.ppOutImg.bufferBusAddr = bfout.fr_buf.phys_addr;
            if(pPpConf.ppOutImg.pixFormat & PP_PIXEL_FORMAT_YUV420_MASK){
                if(pout->IsEmbezzling()){
                    pPpConf.ppOutImg.bufferChromaBusAddr = pPpConf.ppOutImg.bufferBusAddr
                        + pPpConf.ppOutFrmBuffer.frameBufferWidth * pPpConf.ppOutFrmBuffer.frameBufferHeight;
                }else{
                    pPpConf.ppOutImg.bufferChromaBusAddr = pPpConf.ppOutImg.bufferBusAddr
                        + pPpConf.ppOutImg.width * pPpConf.ppOutImg.height;
                }
            }

            if(ol0EnableFlag && ol0Available){
                pPpConf.ppOutMask1.blendComponentBase = bfol0.fr_buf.phys_addr;
            }

            if(ol1EnableFlag && ol1Available){
                pPpConf.ppOutMask2.blendComponentBase = bfol1.fr_buf.phys_addr;
            }

            ppRet = PPSetConfig(pPpInst, &pPpConf);
            if(ppRet != PP_OK) {
                LOGE("PPSetConfig failed while process an image :%d\n", ppRet);
                break;
            }

            ppRet = PPGetResult(pPpInst);
            if(ppRet != PP_OK) {
                LOGE("PPGetResult failed while process an image :%d\n", ppRet);
                break;
            }

            #if 0
            static int i = 0 ;

            i++;
            if(i % 300 == 0){
                WriteFile(bfin.fr_buf.virt_addr, bfin.fr_buf.size, "/tmp/src_nv12_%d_%d.yuv", bfin.fr_buf.size, i);
            }
            #endif

            bfout.SyncStamp(&bfin);
            pout->PutBuffer(&bfout);

out:
            if(ol0EnableFlag && ol0Available)
                pol0in->PutBuffer(&bfol0);
            if(ol1EnableFlag && ol1Available)
                pol1in->PutBuffer(&bfol1);
            pin->PutBuffer(&bfin);
        }
    }

}

IPU_PP::IPU_PP(std::string name, Json *js)
{
    Port *pOut;
    Name = name;
    const char *pRotate = NULL;

    this->Rotate = PP_ROTATION_NONE;

    if(NULL != js){
        pRotate = js->GetString("rotate");

        if(NULL != pRotate){
            if(!strcmp(pRotate, "90")){
                Rotate = PP_ROTATION_RIGHT_90;
            }else if(!strcmp(pRotate, "180")){
                Rotate = PP_ROTATION_180;
            }else if(!strcmp(pRotate, "270")){
                Rotate = PP_ROTATION_LEFT_90;
            }else if(!strcmp(pRotate, "H")){
                Rotate = PP_ROTATION_HOR_FLIP;
            }else if(!strcmp(pRotate, "V")){
                Rotate = PP_ROTATION_VER_FLIP;
            }else{
                LOGE("Rotate Param Error\n");
                throw VBERR_BADPARAM;
            }
        }
    }

    CreatePort("in", Port::In);
    CreatePort("ol0", Port::In);
    CreatePort("ol1", Port::In);
    pOut = CreatePort("out", Port::Out);

    pOut->SetBufferType(FRBuffer::Type::FIXED, 3);
    pOut->SetPixelFormat(Pixel::Format::NV12);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    pOut->Export();

    InstanceCnt++;
}

void IPU_PP::Prepare()
{
    PPResult  ppRet;

    CheckPPAttribute();

    ppRet = PPInit(&this->pPpInst, 0);
    if(ppRet != PP_OK) {
        LOGE("PPInit failed with %d\n", ppRet);
        throw VBERR_BADPARAM;
    }
}

void IPU_PP::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("ol0");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("ol1");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    PPRelease(this->pPpInst);
}

bool IPU_PP::CheckPPAttribute()
{
    Port *pIn, *pOut, *pOl0, *pOl1;

    pIn = GetPort("in");
    pOut = GetPort("out");
    pOl0 = GetPort("ol0");
    pOl1 = GetPort("ol1");

    if((pIn->GetPipX() < 0) ||
        (pIn->GetPipY() < 0) ||
        (pIn->GetWidth() < 0) ||
        (pIn->GetHeight() < 0) ||
        (pIn->GetPipX() % 16 != 0) ||
        (pIn->GetPipY() % 16 != 0) ||
        (pIn->GetWidth() % 8 != 0) ||
        (pIn->GetHeight() % 8 != 0) ||
        (pIn->GetPipX()>pIn->GetWidth()) ||
        (pIn->GetPipY()>pIn->GetHeight()) ||
        (pIn->GetPipWidth()>pIn->GetWidth()) ||
           (pIn->GetPipHeight()>pIn->GetHeight()) ||
           ((pIn->GetPipX()+pIn->GetPipWidth())>pIn->GetWidth()) ||
           ((pIn->GetPipY()+pIn->GetPipHeight())>pIn->GetHeight())){
        LOGE("Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if((pIn->GetPixelFormat()!=Pixel::Format::NV12)){
        LOGE("Input Format params error\n");
        throw VBERR_BADPARAM;
    }

    if((pOut->GetPixelFormat()!=Pixel::Format::NV12) &&
        (pOut->GetPixelFormat()!=Pixel::Format::RGBA8888)){
        LOGE("Out Format Params error\n");
        throw VBERR_BADPARAM;
    }

    if(pOl0->IsEnabled()){
        if(pOut->GetPixelFormat()!=Pixel::Format::RGBA8888)
            throw VBERR_BADPARAM;

        if(pOl0->GetPixelFormat()!=Pixel::Format::RGBA8888)
            throw VBERR_BADPARAM;

        if((pOl0->GetPipX()<0)||(pOl0->GetPipX()>pIn->GetWidth()) ||
            (pOl0->GetPipY()<0)||(pOl0->GetPipY()>pIn->GetHeight()) ||
            ((pOl0->GetPipX()+pOl0->GetWidth())>pIn->GetWidth()) ||
            ((pOl0->GetPipY()+pOl0->GetHeight())>pIn->GetHeight())){
            LOGE("ol0 params error\n");
            throw VBERR_BADPARAM;
        }
    }

    if(pOl1->IsEnabled()){
        if(pOut->GetPixelFormat()!=Pixel::Format::RGBA8888)
            throw VBERR_BADPARAM;

        if(pOl1->GetPixelFormat()!=Pixel::Format::RGBA8888)
            throw VBERR_BADPARAM;

        if((pOl1->GetPipX()<0)||(pOl1->GetPipX()>pIn->GetWidth()) ||
            (pOl1->GetPipY()<0)||(pOl1->GetPipY()>pIn->GetHeight()) ||
            ((pOl1->GetPipX()+pOl1->GetWidth())>pIn->GetWidth()) ||
            ((pOl1->GetPipY()+pOl1->GetHeight())>pIn->GetHeight())){
            LOGE("ol1 params error\n");
            throw VBERR_BADPARAM;
        }
    }

    //no matter this is Embezzling and unEmbezzling,the scale has decided the PipWidth and PipHeight
    if(pOut->IsEmbezzling()){
        if((pOut->GetPipX()<0)||(pOut->GetPipX()>pOut->GetWidth()) ||
            (pOut->GetPipY()<0)||(pOut->GetPipY()>pOut->GetHeight()) ||
            (pIn->GetWidth()>pOut->GetWidth())||(pIn->GetHeight()>pOut->GetHeight())) {
            LOGE("out Embezzling params error\n");
            LOGE("pIn(%d, %d), pOut(Pipx,y(%d, %d), w,h(%d, %d))\n",
                pIn->GetWidth(), pIn->GetHeight(), pOut->GetPipX(), pOut->GetPipY(), pOut->GetWidth(), pOut->GetHeight());
            throw VBERR_BADPARAM;
        }
        if(pOut->GetPixelFormat()!=Pixel::Format::NV12)
            throw VBERR_BADPARAM;

        if((pOut->GetPipWidth()<16) ||
            (pOut->GetPipWidth()>4096)||
            (pOut->GetPipHeight()<16) ||
            (pOut->GetPipHeight()>4096) ||
            (pOut->GetPipWidth()>(3*pIn->GetWidth() - 2)) ||
            (pOut->GetPipHeight()>(3*pIn->GetHeight() - 2))){
            throw VBERR_BADPARAM;
        }

    }else{
        pOut->SetPipInfo(0, 0, pOut->GetWidth(), pOut->GetHeight());
        if((pOut->GetPipWidth()<16) ||
            (pOut->GetPipWidth()>4096)||
            (pOut->GetPipHeight()<16) ||
            (pOut->GetPipHeight()>4096) ||
            (pOut->GetPipWidth()>(3*pIn->GetWidth() - 2)) ||
            (pOut->GetPipHeight()>(3*pIn->GetHeight() - 2))){
            LOGE("out_Pip(%d,%d)\n", pOut->GetPipWidth(), pOut->GetPipHeight());
            LOGE("in(%d,%d)\n", (3*pIn->GetWidth() - 2), (3*pIn->GetHeight() - 2));
            throw VBERR_BADPARAM;
        }
    }

    return true;
}

bool IPU_PP::IsRotateEnabled()
{
    if(this->Rotate != PP_ROTATION_NONE)
        return true;

    return false;
}

int IPU_PP::GetRotateValue()
{
    return this->Rotate;
}

int IPU_PP::SetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "out", 3) == 0)
        GetPort("out")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
    else if(strncmp(vpinfo->portname, "in", 2) == 0)
        GetPort("in")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

int IPU_PP::GetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "out", 3) == 0){
        vpinfo->x = GetPort("out")->GetPipX();
        vpinfo->y = GetPort("out")->GetPipY();
        vpinfo->w = GetPort("out")->GetPipWidth();
        vpinfo->h = GetPort("out")->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "in", 2) == 0){
        vpinfo->x = GetPort("in")->GetPipX();
        vpinfo->y = GetPort("in")->GetPipY();
        vpinfo->w = GetPort("in")->GetPipWidth();
        vpinfo->h = GetPort("in")->GetPipHeight();
    }

    return 0;
}

