/*
编码器获取码流的处理



*/

int proc_enc_stream_thread()
{
    ...............
    
    /* One video frame may comprise of more than one slice. */
    for ( i = 0 ; i < pVStream->u32PackCount; i++ )
    {
        /*@@@@@@ 判断该slice是I slice or普通slice*/
        switch(pVStream->pstPack[i].DataType.enH264EType)
        {
            case H264E_NALU_PSLICE:
                payload_type = VIDEO_NORMAL_FRAME;
                 break;
            default:
                payload_type = VIDEO_KEY_FRAME;
                break;
        }

        /*@@@@@@ 判断该slice是该帧的首个，中间or最后一个slice*/
        /*送入一个slice后清除该slice类型标志*/
        CLEAR_VIDEO_SLICE(slice_type);
        if(0 == i)
        {
            SET_VIDEO_START_SLICE(slice_type);
            if(1 == pVStream->u32PackCount )
            {
                SET_VIDEO_END_SLICE(slice_type);
               //printf(" slice is %d \n",slice_type);
            }
        }
        else if (pVStream->u32PackCount-1 == i)
        {
            SET_VIDEO_END_SLICE(slice_type);
        }
        else
        {
            SET_VIDEO_NORMAL_SLICE(slice_type);
        }
    
        /*@@@@@@ 判断sdk的视频数据是否出现环回，
        否则，直接返回地址
        */
        if(pVStream->pstPack[i].u32Len[1]<= 0)
        {
            pWriteStartAddr = pVStream->pstPack[i].pu8Addr[0];
            u32WriteLen = pVStream->pstPack[i].u32Len[0];
        }
        /*若是，则分配空间将环回数据cpy到一段连续空间上
          改种情况应较少出现*/
        else
        {
            u32WriteLen = pVStream->pstPack[i].u32Len[0]+pVStream->pstPack[i].u32Len[1];
            pWriteStartAddr= (HI_CHAR *)malloc(sizeof(HI_CHAR)*u32WriteLen);
            if (HI_NULL == pWriteStartAddr)
            {
                printf("[failed]to malloc for frame: seq %d byte %d \n",pVStream->u32Seq,u32WriteLen);
                return HI_FAILURE;
            }
            else
            {
                bHasMalloc = HI_TRUE;
            }
            memcpy(pWriteStartAddr, pVStream->pstPack[i].pu8Addr[0], pVStream->pstPack[i].u32Len[0]);
            memcpy(pWriteStartAddr + pVStream->pstPack[i].u32Len[0], pVStream->pstPack[i].pu8Addr[1],
                   pVStream->pstPack[i].u32Len[1]);
            
           // printf("to malloc for frame: seq %d type %d \n",pVStream->u32Seq,u32WriteLen);
        }

        
        /*@@@@@@ MBUF通道和VS通道一一对应*/
        HI_MT_WriteBuf(s32MufChn, payload_type, (const HI_ADDR)(pWriteStartAddr),
                   u32WriteLen, pVStream->pstPack[i].u64PTS,&slice_type);

                                                 
        if(HI_TRUE == bHasMalloc)
        {
            free(pWriteStartAddr);
            pWriteStartAddr = NULL;
            bHasMalloc = HI_FALSE;
            //printf("to free for frame: seq %d type %d \n",pVStream->u32Seq,u32WriteLen);
        }
    }                        

 .....................
}



