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
#include <assert.h>
#include "G2.h"

#define ALLOCBUFSIZE 0x80000

DYNAMIC_IPU(IPU_G2, "g2");

int IPU_G2::G2InstanceCnt = 0 ;  //init  class instance counter

static void *HevcOutputThread(void *arg)
{
    Buffer bfout;
    Port *pOut  = NULL;
    enum DecRet dRet = DEC_OK;
    IPU_G2 *g2 = (IPU_G2 *)arg;
    pOut = g2->GetPort("frame");
    while(g2->OutputThreadRunFlag){
        dRet = HevcDecNextPicture(g2->pG2DecInst, &g2->pG2DecPicture);
        if(dRet == DEC_PIC_RDY){
            bfout = pOut->GetBuffer();
            bfout.fr_buf.phys_addr = g2->pG2DecPicture.output_picture_bus_address;
            bfout.fr_buf.priv = 0;
//            bfout.fr_buf.virt_addr = (void *)g2->pG2DecPicture.output_picture;
            bfout.fr_buf.size = g2->pG2DecPicture.pic_width * g2->pG2DecPicture.pic_height * 3 / 2;
            pOut->PutBuffer(&bfout);

            HevcDecPictureConsumed(g2->pG2DecInst, &g2->pG2DecPicture);
        }else if(dRet == DEC_END_OF_STREAM){
            continue;
        }else{
            continue;
        }
    }

    return NULL;
}

void IPU_G2::WorkLoop()
{
    Port *pIn = NULL, *pOut = NULL;
    Buffer bfin, bfout;
    enum DecRet ret = DEC_OK;
    bool HeaderUserflag = false;
    bool OneTimeRun = false;
    u32 u32Width, u32Height;

    pIn = GetPort("stream");
    pOut = GetPort("frame");
    PicDecodeNum = 0;
    u32Width = pOut->GetWidth();
    u32Height = pOut->GetHeight();
    memset(&pG2DecInput, 0, sizeof(HevcDecInput));
    memset(&pG2DecOutput, 0, sizeof(HevcDecOutput));
    memset(&pG2DecInfo, 0, sizeof(HevcDecInfo));
    memset(&pG2DecPicture, 0, sizeof(HevcDecPicture));

    LOGE("G2 workLoop\n");

    while(IsInState(ST::Running)){
        if (Headerflag == false && HeaderUserflag == false){
            usleep(20000);
            continue;
        } else if(Headerflag == true)
        {
            //high priority to decode header
        }
        else if(HeaderUserflag) {
            try{
                if(pIn->IsBind()){
                    bfin = pIn->GetBuffer(&bfin);
                }else {
                    bfin = decBuf->GetReferenceBuffer(&bfin);
                }
            }catch(const char *err){
                usleep(20000);
                continue;
            }
        }

DECODE_NEXT:
        if(Headerflag){
            pG2DecInput.stream = (u8*)StreamHeader;
            pG2DecInput.stream_bus_address = (u32)pG2DecInput.stream;
            pG2DecInput.data_len = StreamHeaderLen;
            pG2DecInput.buffer = (u8 *)((u32)pG2DecInput.stream & (~0xf));
            pG2DecInput.buffer_bus_address = pG2DecInput.stream_bus_address & (~0xf);
            pG2DecInput.buff_len = pG2DecInput.data_len + (pG2DecInput.stream_bus_address & 0xf);
        }else if(HeaderUserflag){
            pG2DecInput.stream = (u8 *)bfin.fr_buf.virt_addr;
            pG2DecInput.stream_bus_address = (u32)bfin.fr_buf.phys_addr;
            pG2DecInput.data_len = bfin.fr_buf.size;
            pG2DecInput.buffer = (u8 *)((u32)pG2DecInput.stream & (~0xf));
            pG2DecInput.buffer_bus_address = pG2DecInput.stream_bus_address & (~0xf);
            pG2DecInput.buff_len = pG2DecInput.data_len + (pG2DecInput.stream_bus_address & 0xf);
        }else{
            LOGE("ERROR should never be here\n");
            continue;
        }

        /* Picture ID is the picture number in decoding order */
        pG2DecInput.pic_id = PicDecodeNum;
        do{
            ret = HevcDecDecode(pG2DecInst, &pG2DecInput, &pG2DecOutput);
            switch (ret) {
                case DEC_STREAM_NOT_SUPPORTED:
                {
                    LOGE("ERROR: UNSUPPORTED STREAM!\n");
                    break;
                }
                case DEC_ADVANCED_TOOLS:
                {
                    /* ASO/STREAM ERROR was noticed in the stream. The decoder has to
                     * reallocate resources */
                    assert(pG2DecOutput.data_left);
                    /* we should have some data left */
                    /* Used to indicate that picture decoding needs to finalized
                       prior to corrupting next picture */

                    /* Used to indicate that picture decoding needs to finalized prior to
                     * corrupting next picture
                     * pic_rdy = 0; */
                    break;
                }
                case DEC_PIC_DECODED:
                {
                    /* Increment decoding number for every decoded picture */
                    PicDecodeNum++;

                    if (!OutputThreadRunFlag) {
                          OutputThreadRunFlag = true;
                        pthread_create(&G2OutputThread, NULL, HevcOutputThread, this);
                    }
                    break;
                }
                case DEC_HDRS_RDY:
                    LOGE("DEC_HDRS_RDY\n");
                case DEC_PENDING_FLUSH:
                    LOGE("DEC_PENDING_FLUSH or DEC_HDRS_RDY\n");
                    /* Stream headers were successfully decoded -> stream information is available for query now */
                    if (HevcDecGetInfo(pG2DecInst, &pG2DecInfo) != DEC_OK)
                    {
                        LOGE("Error in getting stream info\n");
                        break;
                    }
                    LOGE("Width %d Height %d\n", pG2DecInfo.pic_width, pG2DecInfo.pic_height);

                    LOGE("Cropping params: (%d, %d) %dx%d\n",
                            pG2DecInfo.crop_params.crop_left_offset,
                            pG2DecInfo.crop_params.crop_top_offset,
                            pG2DecInfo.crop_params.crop_out_width,
                            pG2DecInfo.crop_params.crop_out_height);

                    LOGE("MonoChrome = %d\n", pG2DecInfo.mono_chrome);
                    LOGE("DPB mode   = %d\n", pG2DecInfo.dpb_mode);
                    LOGE("Pictures in DPB = %d\n", pG2DecInfo.pic_buff_size);
                    LOGE("Pictures in Multibuffer PP = %d\n", pG2DecInfo.multi_buff_pp_size);
                    LOGE("video_range %d, matrix_coefficients %d\n", pG2DecInfo.video_range, pG2DecInfo.matrix_coefficients);
                    if(u32Width != pG2DecInfo.pic_width ||(ABS_DIFF(u32Height,pG2DecInfo.pic_height) >=16))
                    {
                        LOGE("Resolution Change (%d %d)->(%d %d)\n",u32Width, u32Height, pG2DecInfo.pic_width, pG2DecInfo.pic_height);
                        LOGE("!!!H265DEC SEND VIDEO_FRAME_DUMMY becauseof resolution changenext IPU must handle!!!\n");
                        bfout = pOut->GetBuffer();
                        bfout.fr_buf.priv = VIDEO_FRAME_DUMMY;
                        bfout.fr_buf.stAttr.s32Width = pG2DecInfo.pic_width;
                        bfout.fr_buf.stAttr.s32Height = pG2DecInfo.pic_height;
                        pOut->PutBuffer(&bfout);
                        u32Width = pG2DecInfo.pic_width;
                        u32Height = pG2DecInfo.pic_height;
                        /*
                            SPS change sleep to wait next ipu consume becauseof No Synchronization mechanism between IPU
                            even though this may caused not smooth in sps change
                            the rigth solution is next ipu notify resume decode when it processed VIDEO_FRAME_DUMMY buffer Done
                        */
                        usleep(150*1000);
                    }
                    break;
                case DEC_STRM_PROCESSED:
                {
                    if(!OneTimeRun){
                        OneTimeRun = true;
                        break;
                    }
                    LOGE("DEC_STRM_PROCESSED\n");
                    goto out;
                }
                case DEC_NONREF_PIC_SKIPPED:
                case DEC_STRM_ERROR:
                {
                    /* Used to indicate that picture decoding needs to finalized prior to
                     * corrupting next picture
                    */
                    LOGE("DEC_STRM_ERROR\n");
                    break;
                }
                case DEC_OK:
                    /* nothing to do, just call again */
                    break;
                case DEC_HW_TIMEOUT:
                    LOGE("Timeout\n");
                    break;
                case DEC_HW_BUS_ERROR:
                    LOGE("DEC_HW_BUS_ERROR\n");
                    break;
                default:
                    //Log("FATAL ERROR: %d\n", ret);
                    break;
                }
            //at leaset start code length
            if (pG2DecOutput.data_left > 5) {
                LOGD("data left(%d) .datalen:%d ret:%d\n", pG2DecOutput.data_left, pG2DecInput.data_len, ret);
                if(ret == DEC_HDRS_RDY || ret == DEC_PENDING_FLUSH) {
                    //return value DEC_HDRS_RDY and DEC_PENDING_FLUSH  skipped decode need resend to decode
                    pG2DecInput.stream = (u8 *)bfin.fr_buf.virt_addr;
                    pG2DecInput.stream_bus_address = (u32)bfin.fr_buf.phys_addr;
                    pG2DecInput.data_len = bfin.fr_buf.size;
                    pG2DecInput.buffer = (u8 *)((u32)pG2DecInput.stream & (~0xf));
                    pG2DecInput.buffer_bus_address = pG2DecInput.stream_bus_address & (~0xf);
                    pG2DecInput.buff_len = pG2DecInput.data_len + (pG2DecInput.stream_bus_address & 0xf);
                    if(ret == DEC_PENDING_FLUSH)
                    {
                        /*
                            SPS change sleep to wait next ipu consume becauseof No Synchronization mechanism between IPU
                            even though this may caused not smooth in sps change
                            the rigth solution is next ipu notify resume decode when it processed VIDEO_FRAME_DUMMY buffer Done
                        */
                        usleep(25*1000);
                    }
                } else {
                    pG2DecInput.data_len = 0;
                }
            } else {
                pG2DecInput.data_len = 0;
            }
        }while(pG2DecInput.data_len > 0);
out:
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
            if(pIn->IsBind())
                pIn->PutBuffer(&bfin);
            else {
                decBuf->PutReferenceBuffer(&bfin);
            }
        }
    }
    Headerflag = false;
    HeaderUserflag = false;
    OutputThreadRunFlag = false;
}


IPU_G2::IPU_G2(std::string name, Json* js)
{
    Port *pOut = NULL, *pIn = NULL;
    Name = name;

    if( NULL != js){
    }

    pIn = CreatePort("stream", Port::In);
    pIn->SetPixelFormat(Pixel::Format::H265ES);
    pOut = CreatePort("frame", Port::Out);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    pOut->Enable();
    pOut->SetResolution(1920, 1080);
    pOut->SetBufferType(FRBuffer::Type::VACANT, 3);
    pOut->SetPixelFormat(Pixel::Format::NV12);

    OutputThreadRunFlag = false;
    G2InstanceCnt++;
    Headerflag = false;
    decBuf = NULL;
    dwl_inst = NULL;
}

IPU_G2::~IPU_G2() {
}

void IPU_G2::Prepare()
{
    enum DecRet ret;
    char dec_name[24] = {0};
    struct DWLInitParam dwl_params = {DWL_CLIENT_TYPE_HEVC_DEC};
    Port* pOut = GetPort("frame");
    Port* pIn = GetPort("stream");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::H265ES)
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
        LOGE("g2 decBuf malloc buffer\n");
        sprintf(dec_name, "%s-stream", Name.c_str());
        if(decBuf == NULL){
            decBuf = new FRBuffer(dec_name, FRBuffer::Type::FLOAT_NODROP,  0x244000, ALLOCBUFSIZE);
            if (decBuf ==  NULL){
                LOGE("failed to allocate decBuf.\n");
                return;
            }
        }
    }
    StreamHeader = (char*)malloc(128);
    memset(StreamHeader, 0, 128);
    /* init HevcDec */
    dwl_inst = DWLInit(&dwl_params);
    pG2DecCfg.no_output_reordering = 0;
    pG2DecCfg.use_video_freeze_concealment = 0;
#ifdef DOWN_SCALER
    pG2DecCfg.dscale_cfg.down_scale_x = 1;
    pG2DecCfg.dscale_cfg.down_scale_y = 1;
#endif
    pG2DecCfg.use_video_compressor = 0;
    pG2DecCfg.use_ringbuffer = 0;
    pG2DecCfg.use_fetch_one_pic = 1;
    pG2DecCfg.output_format = DEC_OUT_FRM_RASTER_SCAN;
    pG2DecCfg.pixel_format = DEC_OUT_PIXEL_DEFAULT;

    ret = HevcDecInit(&pG2DecInst, dwl_inst, &pG2DecCfg);
    if(ret != DEC_OK) {
        LOGE("HevcDecInit failed\n");
        throw VBERR_BADPARAM;
    }
}

void IPU_G2::Unprepare()
{
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

    HevcDecEndOfStream(pG2DecInst);

    if (pG2DecInst != NULL) {
        HevcDecRelease(pG2DecInst);
        pG2DecInst = NULL;
    }
    if(dwl_inst != NULL)
        DWLRelease(dwl_inst);
    if(StreamHeader != NULL)
    {
        free(StreamHeader);
        StreamHeader = NULL;
    }
    if(decBuf)
    {
        delete decBuf;
        decBuf = NULL;
    }
}

bool IPU_G2::SetStreamHeader(struct v_header_info *strhdr)
{
    if(strhdr->header){
        memcpy(StreamHeader, strhdr->header, strhdr->headerLen);
        Headerflag = true;
        StreamHeaderLen = strhdr->headerLen;
        LOGE("headerLen:%d\n", strhdr->headerLen);
        return true;
    }
    return false;
}
