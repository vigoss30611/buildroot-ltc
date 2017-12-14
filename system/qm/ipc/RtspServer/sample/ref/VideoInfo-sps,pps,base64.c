HI_S32 ENCSaveVideoInfo(VS_CHN s32Chn,ENC_STREAM_S *pstruEncStream)
{

    HI_U8 nal_type;
    HI_S32 i;
    HI_U32 dataLen;
    HI_CHAR* pstrTemp;
    HI_CHAR* pstrSPS = NULL;
    HI_CHAR* pstrPPS = NULL;
    ENC_VIDEO_PARA *pstruVideoPara = NULL;
    pstruVideoPara = ENCGetVideoDataInfoPtr(s32Chn);

    if(pstruVideoPara->bSetBase64 == HI_TRUE)
    {
        //printf("the sps was caled \n");
        return HI_SUCCESS;
    }
    if(pstruEncStream->enSyncType == SYNC_TYPE_DISSYNC)
    {/*非同步*/
        VENC_STREAM_S* pVStream = NULL;
        pVStream = pstruEncStream->struDisSyncStream.pstruVencStream;
        
        /*是否有视频数据*/
        if(NULL != pVStream)
        {
            
            for(  i =0 ;i< pVStream->u32PackCount;i++)
            {
                nal_type = H264_Get_NalType(*((pVStream->pstPack[i].pu8Addr[0]) + 4));   /*获取slice的NALU类型*/

                printf(" the nalutype is :%d \n",nal_type);
                #if 1
                /*存文件时,第一各包必须是SPS*/
                if ((pstruVideoPara->bSetSeq != HI_TRUE)&&( nal_type == NAL_TYPE_SPS))
                {
                    
                    pstrTemp = (pVStream->pstPack[i].pu8Addr[0]) + 4;
                    dataLen = (pVStream->pstPack[i].u32Len[0]) - 4;
                    
                    printf(" the sps  addr %X nal addr pstrTemp %X len %d\n",
                        pVStream->pstPack[i].pu8Addr[0],pstrTemp,dataLen);
                    
                    memcpy(pstruVideoPara->SeqBase64,pstrTemp,dataLen);
                    pstrTemp = (pVStream->pstPack[i].pu8Addr[1]) ;
                    
                    memcpy(pstruVideoPara->SeqBase64+dataLen,pstrTemp,pVStream->pstPack[i].u32Len[1]);
                    pstruVideoPara->u32SPSLen = dataLen + pVStream->pstPack[i].u32Len[1];
                    pstruVideoPara->bSetSeq = HI_TRUE;
                
                }
                else if((pstruVideoPara->bSetPict != HI_TRUE)&&( nal_type == NAL_TYPE_PPS))
                {
                    pstrTemp = (pVStream->pstPack[i].pu8Addr[0]) + 4;
                    dataLen = (pVStream->pstPack[i].u32Len[0]) - 4;
                    
                    printf(" the pps addr %X nal addr pstrTemp %X len %d\n",
                        pVStream->pstPack[i].pu8Addr[0],pstrTemp,dataLen);
                    
                    memcpy(pstruVideoPara->PictBase64,pstrTemp,dataLen);
                    pstrTemp = (pVStream->pstPack[i].pu8Addr[1]) ;
                    memcpy(pstruVideoPara->PictBase64+dataLen,pstrTemp,pVStream->pstPack[i].u32Len[1]);
                    pstruVideoPara->u32PPSLen = dataLen + pVStream->pstPack[i].u32Len[1];
                    pstruVideoPara->bSetPict = HI_TRUE;
                }
                #endif
            }
            if((pstruVideoPara->bSetPict == HI_TRUE)&&(pstruVideoPara->bSetSeq == HI_TRUE))
            {
                pstrSPS = MP4ToBase64(pstruVideoPara->SeqBase64,pstruVideoPara->u32SPSLen);
                pstrPPS = MP4ToBase64(pstruVideoPara->PictBase64,pstruVideoPara->u32PPSLen);
                sprintf(pstruVideoPara->Base64,"%s,%s",pstrSPS,pstrPPS);
                pstruVideoPara->u32Base64Len = strlen(pstruVideoPara->Base64);
                printf("the sps pps is :%s \n",pstruVideoPara->Base64);
                pstruVideoPara->bSetBase64 = HI_TRUE;
                free(pstrPPS);
                free(pstrSPS);
             

            }
           // DEBUG_CHNREC_PRINT("REC",LOG_LEVEL_INFO,"REC_WriteBuffer write video sum len:%d\n",u32VideoSum);
        }  
    }
    else
    {
    }
    return HI_SUCCESS;
}