// Copyright (C) 2016 InfoTM, devin.zhu@infotm.com
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
#include <stdio.h>
#include <qsdk/videobox.h>
#include "G1264.h"

#define ALLOCBUFSIZE 0x80000
#define DISABLE_FLOAT_FR 0
DYNAMIC_IPU(IPU_G1264, "g1264");

int IPU_G1264::G1264InstanceCount = 0;

IPU_G1264::IPU_G1264(std::string name, Json *js) {

    Port *pOut = NULL, *pIn = NULL;
    Name = name;

    if (js == NULL) {
    }

    //create InPort & OutPort for IPU_G1264
    LOGE("G1264 create Inport and Outport!\n");
    pIn = CreatePort("stream" ,Port::In);
    pIn->SetPixelFormat(Pixel::Format::H264ES);
    pOut = CreatePort("frame", Port::Out);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    pOut->Enable();
    pOut->SetResolution(1920, 1080);
    pOut->SetBufferType(FRBuffer::Type::VACANT, 3);
    pOut->SetPixelFormat(Pixel::Format::NV12);
    pOut->Export();

    PicDecodeNum = 0;
    Headerflag = false;
    G1264InstanceCount++;
    decBuf = NULL;
}

IPU_G1264::~IPU_G1264() {

}

void IPU_G1264::Prepare() {

    H264DecRet ret;
    char dec_name[24] = {0};
    Port* pOut = GetPort("frame");
    Port* pIn = GetPort("stream");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();


    if(enPixelFormat != Pixel::Format::H264ES)
    {
        LOGE("stream Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = pOut->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21)
    {
        LOGE("frame Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }


    if(!pIn->IsBind()){
        LOGE("G1264 decBuf malloc buffer\n");
        sprintf(dec_name, "%s-stream", Name.c_str());
        if(decBuf == NULL){
#if DISABLE_FLOAT_FR
            decBuf = new FRBuffer(dec_name, FRBuffer::Type::FIXED_NODROP, 3, ALLOCBUFSIZE);
#else
            decBuf = new FRBuffer(dec_name, FRBuffer::Type::FLOAT_NODROP,  0x244000, ALLOCBUFSIZE);
#endif
            if (decBuf ==  NULL){
                LOGE("failed to allocate decBuf.\n");
                return;
            }
        }
    }
    StreamHeader = (char *)malloc(128);
    memset(StreamHeader, 0, 128);

    ret = H264DecInit(&pG1264DecInst, 0, 0, 0, DEC_REF_FRM_RASTER_SCAN, 0);

    if (ret != H264DEC_OK) {
        LOGE("H264DecInit failded\n");
        throw VBERR_BADPARAM;
    }
}


void IPU_G1264::Unprepare() {
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("frame");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("stream");
    if (pIpuPort->IsBind() && pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if (pG1264DecInst != NULL) {
        H264DecRelease(pG1264DecInst);
    }
    if(decBuf)
    {
        delete decBuf;
        decBuf = NULL;
    }
    G1264InstanceCount--;
    if(StreamHeader != NULL)
    {
        free(StreamHeader);
        StreamHeader = NULL;
    }
}

bool IPU_G1264::SetStreamHeader(struct v_header_info *strhdr) {

    if (strhdr->header) {
        memcpy(StreamHeader, strhdr->header, strhdr->headerLen);
        Headerflag = true;
        StreamHeaderLen = strhdr->headerLen;
        return true;
    }
    return false;
}

void IPU_G1264::WorkLoop() {

    Port *portIn = NULL;
    Port *portOut = NULL;
    Buffer bufferIn,bufferOut;
    H264DecRet ret;
    bool HeaderUserflag = false;
    uint32_t u32Width = 0, u32Height = 0;

    portIn = GetPort("stream");
    portOut = GetPort("frame");

    LOGE("start G1264 WorkLoop!\n");
    u32Width = portOut->GetWidth();
    u32Height = portOut->GetHeight();

    memset(&G1264DecInput,   0, sizeof(H264DecInput));
    memset(&G1264DecOutput,  0, sizeof(H264DecOutput));
    memset(&G1264DecInfo,    0, sizeof(H264DecInfo));
    memset(&G1264DecPicture, 0, sizeof(H264DecPicture));

    while (IsInState(ST::Running)) {
        if (Headerflag == false && HeaderUserflag == false){
            usleep(20000);
            continue;
        } else if(Headerflag == true)
        {
            //high priority to decode header
        }else if(HeaderUserflag) {
            try {
                if(portIn->IsBind())
                    bufferIn = portIn->GetBuffer(&bufferIn);
                else{
                    bufferIn = decBuf->GetReferenceBuffer(&bufferIn);
                }
            } catch (const char *err) {
                usleep(20000);
                continue;
            }
        }

DECODE_NEXT:
        if (Headerflag) {
            G1264DecInput.pStream = (u8 *)StreamHeader;
            G1264DecInput.streamBusAddress = (u32)G1264DecInput.pStream;
            G1264DecInput.dataLen = StreamHeaderLen;
        } else if (HeaderUserflag) {
            G1264DecInput.pStream = (u8 *)bufferIn.fr_buf.virt_addr;
            G1264DecInput.streamBusAddress = (u32)bufferIn.fr_buf.phys_addr;
            G1264DecInput.dataLen = bufferIn.fr_buf.size;
       } else {
            LOGE("ERROR should never be here\n");
            continue;
       }

       do {
           G1264DecInput.picId = PicDecodeNum;
           ret = H264DecDecode(pG1264DecInst, &G1264DecInput, &G1264DecOutput);

           switch (ret) {
               case H264DEC_STRM_PROCESSED:
                   //LOGE("H264DEC_STRM_PROCESSED\n");
                   break;
               case H264DEC_HDRS_RDY:
                    H264DecGetInfo(pG1264DecInst, &G1264DecInfo);
                    LOGE("H264DEC_HDRS_RDY\n");
                    LOGE("G1264DecInfoWidth %d G1264DecInfoHeight %d\n", G1264DecInfo.picWidth, G1264DecInfo.picHeight);
                    if (u32Width != G1264DecInfo.picWidth || (ABS_DIFF(u32Height,G1264DecInfo.picHeight) >=16))
                    {
                        LOGE("!!!H264DEC SEND VIDEO_FRAME_DUMMY becauseof resolution changenext IPU must handle!!!\n");
                        bufferOut = portOut->GetBuffer();
                        bufferOut.fr_buf.priv = VIDEO_FRAME_DUMMY;
                        bufferOut.fr_buf.stAttr.s32Width = G1264DecInfo.picWidth;
                        bufferOut.fr_buf.stAttr.s32Height = G1264DecInfo.picHeight;
                        u32Width = G1264DecInfo.picWidth;
                        u32Height = G1264DecInfo.picHeight;
                        portOut->PutBuffer(&bufferOut);
                        usleep(6*1000);
                    }
                    break;
               case H264DEC_PIC_DECODED:
                   //LOGE("H264DEC_PIC_DECODED\n");
                   //obtain picture data in display order, if any available
                   while (H264DecNextPicture(pG1264DecInst, &G1264DecPicture, 0) == H264DEC_PIC_RDY) {
                       bufferOut = portOut->GetBuffer();
                       bufferOut.fr_buf.phys_addr = G1264DecPicture.outputPictureBusAddress;
                       bufferOut.fr_buf.size = G1264DecPicture.picWidth*G1264DecPicture.picHeight*3/2;
                       bufferOut.fr_buf.priv = 0;
                       portOut->PutBuffer(&bufferOut);
                   }

                   PicDecodeNum++;

                   break;
               case H264DEC_PENDING_FLUSH://when smooth mode
                   break;
               default:
                   LOGE("H264DEC_default ret:%d\n",ret);
                   break;
           }

           //check if all data are consumed && ret == H264DEC_HDRS_RDY
           if (G1264DecOutput.dataLeft > 0 && ret > 0) {
               G1264DecInput.dataLen = G1264DecOutput.dataLeft;
               G1264DecInput.pStream = G1264DecOutput.pStrmCurrPos;
               G1264DecInput.streamBusAddress = G1264DecOutput.strmCurrBusAddress;
            } else {
               G1264DecInput.dataLen = 0;
            }
       } while (G1264DecInput.dataLen > 0);

       while (H264DecNextPicture(pG1264DecInst, &G1264DecPicture, 0) == H264DEC_PIC_RDY && ret > 0) {
           bufferOut = portOut->GetBuffer();
           bufferOut.fr_buf.phys_addr = G1264DecPicture.outputPictureBusAddress;
           bufferOut.fr_buf.size = G1264DecPicture.picWidth*G1264DecPicture.picHeight*3/2;
           bufferOut.fr_buf.priv = 0;
           portOut->PutBuffer(&bufferOut);
       }
        if (Headerflag && HeaderUserflag)
        {
            //handle play next stream, header and first I frame
            //may be receive at the same loop becuase pIn->getbuffer is block
            Headerflag = false;
            goto DECODE_NEXT;
        }
        if (Headerflag) {
            HeaderUserflag = true;
            Headerflag = false;
        }
        else if (HeaderUserflag)
        {
            if(portIn->IsBind())
                portIn->PutBuffer(&bufferIn);
            else
                decBuf->PutReferenceBuffer(&bufferIn);
        }
    }

    Headerflag = false;
    HeaderUserflag = false;
}
