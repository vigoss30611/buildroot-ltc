#include "shm_util.h"
#include "av_shm.h"
#include "av_video_common.h"

#define DA_LOG(x...) printf("[DANA]"x);

#ifdef SPORTDV_PROJECT
const unsigned int g_videoWidth[ENTRY_VIDOE_INFO_CNT]={1920, 1920, 64};
const unsigned int g_videoHeight[ENTRY_VIDOE_INFO_CNT]={1080, 1088, 64};
#else
const unsigned int g_videoWidth[ENTRY_VIDOE_INFO_CNT]={1920, 1920, 1280};
const unsigned int g_videoHeight[ENTRY_VIDOE_INFO_CNT]={1080, 1088, 720};
#endif
MEM_MNG_INFO *g_pSharedMem = NULL;
VIDEO_BLK_INFO *g_pBlkAddr[ENTRY_INFO_CNT]= {0};
OSA_IpcShmHndl g_shm_hndl={0};

MEM_MNG_INFO *MMng_GetShMem()
{
	if(g_pSharedMem!=NULL)return g_pSharedMem;

	OSA_IpcShmHndl shm;
	int memsize = MMng_caculate_all(NULL);
	//g_pSharedMem = (MEM_MNG_INFO*)OSA_ipcShmOpen(&shm, SHM_KEY, memsize, OSA_IPC_FLAG_OPEN);
	DA_LOG("%s  key:%x, addr:%p - %p\n", __FUNCTION__,  SHM_KEY, g_pSharedMem, (void*)g_pSharedMem+memsize);
	return g_pSharedMem;
}


void*  MMng_AllocShMem(int size) 
{
    DA_LOG("%s size:%d, key:%x\n", __FUNCTION__, size, SHM_KEY);
    //g_pSharedMem = (MEM_MNG_INFO*)OSA_ipcShmOpen(&g_shm_hndl, SHM_KEY, size, OSA_IPC_FLAG_CREATE);
	if(g_pSharedMem == NULL)
	{
		g_pSharedMem = (MEM_MNG_INFO*)MMng_GetShMem();
	}
    else
    {
        //for newly created mem, need to init
        memset(g_pSharedMem, 0, size);
    }
	return g_pSharedMem;
}
void  MMng_FreeShMem() 
{
    //int ret = OSA_ipcShmClose(&g_shm_hndl);
	//DA_LOG("%s ret:%d, key:%x\n", __FUNCTION__, ret, SHM_KEY);
}




void MMng_dumpVInfo(SHM_ENTRY_TYPE idx)
{
	VIDEO_BLK_INFO *vinfo = MMng_GetBlkInfo(idx);
    if(vinfo == NULL) return;
	if(vinfo == NULL) 
	{    
		DA_LOG("%s NULL ptr\n", __FUNCTION__);
		return;
	}
        DA_LOG("\n-----\n%s(%d*%d) start. vinfo[%d]:%p\n", __FUNCTION__ , vinfo->width, vinfo->height,idx, vinfo);
        DA_LOG("\tblk:%d*%d, free:%d, currBlk:%d\n", vinfo->blk_sz , vinfo->blk_num,  vinfo->blk_free, vinfo->cur_blk);
        DA_LOG("\tframe_num:%d, idle:%d, currFrm:%d, serial:%d\n",
			vinfo->frame_num, vinfo->idle_frame_cnt, vinfo->cur_frame, vinfo->cur_serial);
        DA_LOG("\tgop IFrm:%d(%d-%d), Ser:%d(%d-%d)\n-----\n\n",
			vinfo->gop.lastest_I, 
			vinfo->gop.last_Start,			vinfo->gop.last_End,
			vinfo->gop.lastest_I_serial,
			vinfo->gop.last_Start_serial,			vinfo->gop.last_End_serial);
}
			
 void MMng_dumpMemInfo(MEM_MNG_INFO * pMemInfo)
{
	
	if(pMemInfo == NULL) 
	{    
		DA_LOG("%s NULL ptr\n", __FUNCTION__);
		return;
	}

    DA_LOG("%s start\n", __FUNCTION__);
    DA_LOG("\t video_info_nums:%d\n",pMemInfo->video_info_nums);
    int i;
    for(i=0;i<pMemInfo->video_info_nums;i++)
    {
		DA_LOG("\t %d : size=%ld\n", i, pMemInfo->video_info_size[i]);
    }

}
void MMng_dumpFrmInfo(SHM_ENTRY_TYPE idx, int serialNO, const char* str)
{
    
	VIDEO_BLK_INFO *vinfo = MMng_GetBlkInfo(idx);
    if(vinfo == NULL) return;
    
    VIDEO_FRAME *pFrmInfo = MMng_GetFrame_BySerial(serialNO, idx);
	if(pFrmInfo == NULL) 
	{    
		DA_LOG("%s NULL ptr %s\n", __FUNCTION__, str);
		return;
	}
    DA_LOG("%s start. %s\n", __FUNCTION__, str);
    DA_LOG("\t serial:%d\n", pFrmInfo->serial);
    int i;
    for(i=0;i<ENTRY_INFO_CNT;i++)
    {
        if(pFrmInfo->ref_serial[i] == SHM_INVALID_SERIAL)continue;
        DA_LOG("\t\t serial[%d]:%d\n", i, pFrmInfo->ref_serial[i]);
    }
        
    DA_LOG("\t frmtype:%d\n", pFrmInfo->fram_type);
    DA_LOG("\t blkindex:%d\n", pFrmInfo->blkindex);
    DA_LOG("\t blks:%d\n", pFrmInfo->blks);
    DA_LOG("\t realsize:%d\n", pFrmInfo->realsize);
    DA_LOG("\t flag:%d\n", pFrmInfo->flag);
    DA_LOG("\t timestamp:%lld\n", pFrmInfo->timestamp_frm);
}

 void MMng_dumpBusyFrame(VIDEO_BLK_INFO *pVInfo)
{
	if(pVInfo == NULL) 
	{    
		DA_LOG("%s NULL ptr\n", __FUNCTION__);
		return;
	}

	int i;
	char *buf = malloc(2000);
	int pos = 0;

	pos += sprintf(buf+pos, "\tBusy Frame: (");
	for(i=0;i<pVInfo->frame_num;i++)
	{
    	VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) continue;
		pos += sprintf(buf+pos, " %.3d ", i);
	}
	pos += sprintf(buf+pos, ") ");
	DA_LOG("%s \n",  buf);

    pos=0;
	pos += sprintf(buf+pos, " \n\t\t Serial (");
	for(i=0;i<pVInfo->frame_num;i++)
	{
		VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) continue;
		pos += sprintf(buf+pos, " %.3d ", pFrm->serial);
	}
	pos += sprintf(buf+pos, ") ");
	DA_LOG("%s \n",  buf);

    pos=0;
	pos += sprintf(buf+pos, " \n\t\t type   (");
	for(i=0;i<pVInfo->frame_num;i++)
	{
		VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) continue;
		pos += sprintf(buf+pos, " %.3d ", pFrm->fram_type);
	}
	pos += sprintf(buf+pos, ") ");
	DA_LOG("%s \n",  buf);

    pos=0;
	pos += sprintf(buf+pos, " \n\t\t startB (");
	for(i=0;i<pVInfo->frame_num;i++)
	{
		VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) continue;
		pos += sprintf(buf+pos, " %.3d ", pFrm->blkindex);
	}
	pos += sprintf(buf+pos, ") ");
	DA_LOG("%s \n",  buf);

    pos=0;
	pos += sprintf(buf+pos, " \n\t\t BlkCnt (");
	for(i=0;i<pVInfo->frame_num;i++)
	{
		VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) continue;
		pos += sprintf(buf+pos, " %.3d ",  pFrm->blks);
	}
	pos += sprintf(buf+pos, ") ");

	DA_LOG("%s %s \n", __FUNCTION__, buf);
	free(buf);
}

 void MMng_dumpIdleCnt(VIDEO_BLK_INFO *pVInfo)
{
	int i;
	int idleCnt= 0;
	int busyCnt= 0;
	for(i=0;i<pVInfo->frame_num;i++)
	{
    	VIDEO_FRAME *pFrm = &pVInfo->frame[i];
		if(pFrm->fram_type == EMPTY_FRAME) idleCnt++;
		else busyCnt++;
	}	
	if(pVInfo->idle_frame_cnt != idleCnt)
		DA_LOG("%s Error! idlecnt:%d(real:%d), busycnt:%d, Match!\n", __FUNCTION__, idleCnt,pVInfo->idle_frame_cnt, busyCnt);
	//else DA_LOG("%s idlecnt:%d, busycnt:%d, Match!\n", __FUNCTION__, idleCnt, busyCnt);
}

void MMng_reset(VIDEO_FRAME * pframe)
{
    int cnt = 0;

    pframe->fram_type = EMPTY_FRAME;
    pframe->serial = -1;
    pframe->blkindex = -1;
    pframe->blks = -1;
    pframe->realsize = -1;
    pframe->flag = 0;
    memset(&pframe->timestamp_frm, 0, sizeof(struct timeval));
    for (cnt = 0; cnt < ENTRY_INFO_CNT; cnt++) {
        pframe->ref_serial[cnt] = SHM_INVALID_SERIAL;
    }
}

static void MMng_Config_Video(VIDEO_BLK_INFO *vinfo,   int idx)
{
	DA_LOG("%s[%d] begin\n", __FUNCTION__, idx);
    int blkCnt = SHM_AV_BLK_CNT;
    int width = g_videoWidth[idx];
    int height = g_videoHeight[idx];
	int blksz = width * height /SHM_AV_BLK_SIZE_DIV_PARAM;
    
    vinfo->video_type = idx;
    //vinfo->size = blksz*blkCnt;
    vinfo->extrasize = 0;
    vinfo->width = width;
    vinfo->height = height;
    vinfo->blk_sz = blksz;
    vinfo->blk_num = blkCnt;
    vinfo->blk_free = blkCnt;
    vinfo->frame_num = blkCnt;
    vinfo->idle_frame_cnt = blkCnt;
    vinfo->cur_frame = -1;
    vinfo->cur_serial = -1;
    vinfo->cur_blk = -1; 
    vinfo->del_serial = -1;
	vinfo->gop.last_Start = -1;
	vinfo->gop.last_Start_serial = -1;
	vinfo->gop.last_End = -1;
	vinfo->gop.last_End_serial = -1;
	vinfo->gop.lastest_I = -1;
	vinfo->gop.lastest_I_serial = -1;
 
    int cnt2 = 0;
    for (cnt2 = 0; cnt2 < vinfo->blk_num; cnt2++) {
        MMng_reset(& (vinfo->frame[cnt2])); 
    }
	DA_LOG("%s[%d] end\n", __FUNCTION__, idx);
}
static void MMng_Config_Audio(VIDEO_BLK_INFO *vinfo, SHM_ENTRY_TYPE shmType)
{
	DA_LOG("%s begin\n", __FUNCTION__);
    int blksz = SHM_AUDIO_BLK_SIZE;
    int blkCnt = SHM_AUDIO_BLK_CNT;
	vinfo->video_type = shmType;
    //vinfo->size = blksz*blkCnt;
    vinfo->extrasize = 0;
     vinfo->blk_sz = blksz;
    vinfo->blk_num = blkCnt;
    vinfo->blk_free = blkCnt;
    vinfo->frame_num = blkCnt;
    vinfo->idle_frame_cnt = blkCnt;
    vinfo->cur_frame = -1;
    vinfo->cur_serial = -1;
    vinfo->cur_blk = -1; 
    vinfo->del_serial = -1;
 
    int cnt2 = 0;
    for (cnt2 = 0; cnt2 < vinfo->blk_num; cnt2++) {
        MMng_reset(& (vinfo->frame[cnt2])); 
    }
	DA_LOG("%s end\n", __FUNCTION__);
}
static void MMng_Config_Jpeg(VIDEO_BLK_INFO *vinfo)
{
	DA_LOG("%s begin\n", __FUNCTION__);
    int blksz = SHM_JPEG_BLK_SIZE;
    int blkCnt = SHM_AV_BLK_CNT;
    vinfo->video_type = ENTRY_JPEG_INFO;
    //vinfo->size = blksz*blkCnt;
    vinfo->extrasize = 0;
     vinfo->blk_sz = blksz;
    vinfo->blk_num = blkCnt;
    vinfo->blk_free = blkCnt;
    vinfo->frame_num = blkCnt;
    vinfo->idle_frame_cnt = blkCnt;
    vinfo->cur_frame = -1;
    vinfo->cur_serial = -1;
    vinfo->cur_blk = -1; 
    vinfo->del_serial = -1;
 
    int cnt2 = 0;
    for (cnt2 = 0; cnt2 < vinfo->blk_num; cnt2++) {
        MMng_reset(& (vinfo->frame[cnt2])); 
    }
	DA_LOG("%s end\n", __FUNCTION__);
}

int MMng_Config_All(MEM_MNG_INFO * pMemInfo)
{
    DA_LOG("%s start...\n", __FUNCTION__);

    char *pData = (char*)pMemInfo + sizeof(MEM_MNG_INFO);
    int shmType = 0;
    for (shmType = 0; shmType < ENTRY_INFO_CNT; shmType++) {
        VIDEO_BLK_INFO *pVinfo = (VIDEO_BLK_INFO*)pData;
		DA_LOG("\t%s vinfo[%d]:%lu(0x%lx)\n", __FUNCTION__, shmType, (unsigned long)((void*)pVinfo-(void*)pMemInfo), (unsigned long)((void*)pVinfo-(void*)pMemInfo));

		switch(shmType){
		case ENTRY_AUDIO_PREVIEW:
        case ENTRY_AUDIO_PLAYBACK:
			MMng_Config_Audio(pVinfo, shmType);
			break;
		case ENTRY_JPEG_INFO:
			MMng_Config_Jpeg(pVinfo);
			break;
		
        case ENTRY_VIDOE_INFO_HIG:
        case ENTRY_VIDOE_INFO_MED:
        case ENTRY_VIDOE_INFO_LOW:   
			MMng_Config_Video(pVinfo, shmType);
			break;				
		}
        

         //MMng_dumpVInfo(cnt);
         pData +=  pMemInfo->video_info_size[shmType];
    } 
    return 0; 
}

static int MMng_Config_Video_Jpeg(MEM_MNG_INFO *pMemInfo,SHM_ENTRY_TYPE shmTypeC) {
    DA_LOG("%s start...\n", __FUNCTION__);

    char *pData = (char*)pMemInfo + sizeof(MEM_MNG_INFO);
    int shmType=0;
    for (shmType = 0; shmType < ENTRY_INFO_CNT; shmType++) {
        VIDEO_BLK_INFO *pVinfo = (VIDEO_BLK_INFO*)pData;
        DA_LOG("\t%s vinfo[%d]:%lu(0x%lx)\n", __FUNCTION__, shmType, (unsigned long)((void*)pVinfo-(void*)pMemInfo), (unsigned long)((void*)pVinfo-(void*)pMemInfo));

        switch(shmType){
            case ENTRY_AUDIO_PREVIEW:
            case ENTRY_AUDIO_PLAYBACK:
			//MMng_Config_Audio(pVinfo, shmType);
			break;
		case ENTRY_JPEG_INFO:
			if(shmTypeC!=ENTRY_VIDOE_INFO_MED)
			MMng_Config_Jpeg(pVinfo);
			break;
		
        case ENTRY_VIDOE_INFO_HIG:
        case ENTRY_VIDOE_INFO_MED:
        case ENTRY_VIDOE_INFO_LOW: 
			if(shmTypeC==shmType)
			MMng_Config_Video(pVinfo, shmType);
			break;				
		}
        

         //MMng_dumpVInfo(cnt);
         pData +=  pMemInfo->video_info_size[shmType];
    } 
    return 0; 
}

int MMng_Video_Jpeg_Reset(SHM_ENTRY_TYPE shmType) {
    if (g_pSharedMem) { 
        return MMng_Config_Video_Jpeg(g_pSharedMem,shmType);
    }

    return 0;
}

static int MMng_caculate_Audio_size( )
{
	int hdrSize = sizeof(VIDEO_BLK_INFO) - 4;
	int oneEntrySize =  SHM_AUDIO_BLK_SIZE;
	int allEntrySize = oneEntrySize*SHM_AUDIO_BLK_CNT;
	int totalSize = hdrSize+allEntrySize;
	DA_LOG("%s: (hdr:%d, entry:%d, allentry:%d, Ainfolenth:%d)\n",
				__FUNCTION__,   hdrSize, oneEntrySize, allEntrySize, totalSize);
	return totalSize;
}

static int MMng_caculate_JPEG_size()
{
	int hdrSize = sizeof(VIDEO_BLK_INFO) - 4;
	int oneEntrySize =  SHM_JPEG_BLK_SIZE;
	int allEntrySize = oneEntrySize*SHM_AV_BLK_CNT;
	int totalSize = hdrSize+allEntrySize;
	DA_LOG("%s: (hdr:%d, entry:%d, allentry:%d, Jpeginfolenth:%d)\n",
				__FUNCTION__,   hdrSize, oneEntrySize, allEntrySize, totalSize);
	return totalSize;
}

static int MMng_caculate_Video_size( int videoIdx)
{
				
	int hdrSize = sizeof(VIDEO_BLK_INFO) - 4;
	int oneEntrySize =  g_videoWidth[videoIdx] * g_videoHeight[videoIdx] /SHM_AV_BLK_SIZE_DIV_PARAM;
	int allEntrySize = oneEntrySize*SHM_AV_BLK_CNT;
	int totalSize = hdrSize+allEntrySize;
	DA_LOG("%s[%d]: (hdr:%d, entry:%d, allentry:%d, Vinfolenth:%d)\n",
				__FUNCTION__, videoIdx,  hdrSize, oneEntrySize, allEntrySize, totalSize);
	return totalSize;
}


int MMng_caculate_all(MEM_MNG_INFO *pMemInfo)
{
	int totalLength = sizeof(MEM_MNG_INFO);
	int entryLength;
	int i;

    if(pMemInfo != NULL) {
        memset(pMemInfo, 0, sizeof(*pMemInfo));
        pMemInfo->video_info_nums = ENTRY_INFO_CNT;
    }
	for(i=0;i<ENTRY_INFO_CNT;i++)
	{
		switch(i){
        case ENTRY_AUDIO_PREVIEW:
        case ENTRY_AUDIO_PLAYBACK:
			entryLength = MMng_caculate_Audio_size();
			if(pMemInfo!=NULL)pMemInfo->video_info_size[i] = entryLength;
			break;
		case ENTRY_JPEG_INFO:
			entryLength = MMng_caculate_JPEG_size();
			if(pMemInfo!=NULL)pMemInfo->video_info_size[i] = entryLength;
			break;
		default:
			entryLength = MMng_caculate_Video_size(i);
			if(pMemInfo!=NULL)pMemInfo->video_info_size[i] = entryLength;
			break;				
		}
		totalLength += entryLength;			
	}
	return totalLength;
}

int MMng_GetShmEntryType(unsigned int streamIndex)
{
    if (streamIndex >= E_STREAM_ID_CNT) {
        return ENTRY_VIDOE_INFO_NULL;
    }

    switch(streamIndex) {
        case E_STREAM_ID_STORAGE: 
            return ENTRY_VIDOE_INFO_HIG;
            
        case E_STREAM_ID_PREVIEW:
        case E_STREAM_ID_PLAYBACK:
            // playback and preview share the same a/v sharemem region
            return ENTRY_VIDOE_INFO_MED;
        case E_STREAM_ID_MJPEG:
            return ENTRY_VIDEO_INFO_MJPEG;
    
        default:
            return ENTRY_VIDOE_INFO_NULL;
    }
}

MEM_MNG_INFO* MMng_Init()
{
	static int s_init = 0;
	if(s_init == 1) return g_pSharedMem;

	DA_LOG("MMng_Init log.\n");
	DA_LOG("begin");
	MEM_MNG_INFO memInfo;
    int totalsize = MMng_caculate_all(&memInfo);
    g_pSharedMem = (MEM_MNG_INFO*)MMng_AllocShMem(totalsize);
    if (g_pSharedMem == 0) goto MEM_INIT_FAIL;
    memcpy(g_pSharedMem, &memInfo, sizeof(MEM_MNG_INFO));
    MMng_dumpMemInfo(&memInfo);

    if (MMng_Config_All(g_pSharedMem) != 0) goto MEM_INIT_FAIL;
    s_init = 1;
    DA_LOG("MMng_Init return:%p\n", g_pSharedMem);
    return g_pSharedMem;

 MEM_INIT_FAIL:
    DA_LOG(" MEM_INIT_FAIL g_pSharedMem:%p\n", g_pSharedMem);
    return NULL;
}

int MMng_Free(VIDEO_BLK_INFO * pVidInfo, int frame_id)
{
    int blkStart = -1;

    if (frame_id >= pVidInfo->frame_num || frame_id < 0) {
        DA_LOG("invalidate frame_id\n");
        return -1;
    }

    VIDEO_FRAME *pFrm = &pVidInfo->frame[frame_id];
    if (pFrm->fram_type == EMPTY_FRAME) {
        DA_LOG("\tfree frame already, id:%d\n", frame_id);
        return 0;
    }

    if (pFrm->blks > 0) {
        pVidInfo->blk_free += pFrm->blks;
        blkStart = pFrm->blkindex;
        DA_LOG("%s frame[%d], serial:%d  total blk_free:%d, freeBlk(%d-%d), \n", 
            __FUNCTION__, frame_id, pFrm->serial, pVidInfo->blk_free,
			blkStart, blkStart + pFrm->blks-1 );
    }
	
	if (pVidInfo->gop.last_Start > 0) {
			if (pVidInfo->gop.last_Start == frame_id) {
				pVidInfo->gop.last_Start = -1;
				pVidInfo->gop.last_Start_serial = SHM_INVALID_SERIAL;
				pVidInfo->gop.last_End = -1;
				pVidInfo->gop.last_End_serial = SHM_INVALID_SERIAL;
			}
		}
	
	if (pVidInfo->gop.lastest_I > 0) {
			if (pVidInfo->gop.lastest_I == frame_id) {
				pVidInfo->gop.lastest_I = -1;
				pVidInfo->gop.last_End_serial = SHM_INVALID_SERIAL;
	
			}
	}
	pVidInfo->del_serial = pFrm->serial;

    MMng_reset(&(pVidInfo->frame[frame_id]));

    pVidInfo->idle_frame_cnt ++;
    return blkStart;
}

int MMng_Insert(void *pData, int size, int blks,
               int frame_type, SHM_ENTRY_TYPE idx)
{
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);

    {
        static int cnt = 0;
        cnt ++;
        if(cnt %500 == 1)DA_LOG("MMng_Insert[%d]. pData:%x, size:%d, blks:%d, ftype:%d, idx:%d\n", cnt, pData,  size,  blks,  frame_type,  idx);
    }
    if (pVidInfo == NULL || frame_type >= END_FRAME_TYPE || blks <= 0) {
        return -1;
    }

    //TODO "newFrameNo calculate error,frame_num can not be a fixed value;
    int newSerialNo = pVidInfo->cur_serial + 1;
    int newFrameNo  = (pVidInfo->cur_frame + 1) % pVidInfo->frame_num;
    int newBlkNo    = (pVidInfo->cur_blk + 1) % pVidInfo->blk_num;
    int hdrSz = 0;

    // copy data first
    int i;
    if (frame_type != DUMMY_FRAME) {
        unsigned long WriteAddr = (unsigned long)&pVidInfo->dataAddrsss +  newBlkNo * pVidInfo->blk_sz;
        if (!WriteAddr || pData == NULL) {
            DA_LOG("MMng_memcpy fail!!\n");
            return -1;
        }
		DA_LOG("MMng_Insert. memcpy, toAddr:%p, size:%d, fromAddr:0x%x, newBlkNo:0x%x,blk_sz:0x%x \n", (void*)WriteAddr,size, pData, newBlkNo , pVidInfo->blk_sz);

        if ((ENTRY_VIDOE_INFO_MED == idx) && (frame_type == I_FRAME)) {            
            unsigned char* hdrData = NULL;
            MMng_Get_Extra(MMng_GetShmEntryType(E_STREAM_ID_PREVIEW),
                            (unsigned char **) &hdrData, &hdrSz);
            memcpy(WriteAddr,hdrData,hdrSz);
            WriteAddr+=hdrSz;
			if(hdrSz==0)
			{
			  printf("MMng_Insert error streamIndex=%d,size=%d\n",E_STREAM_ID_PREVIEW,hdrSz);
			}
        }
        memcpy((void *)WriteAddr, pData, size);
    }

    for(i=0;i<ENTRY_INFO_CNT;i++)
    {
        VIDEO_BLK_INFO *pBlkInfo = MMng_GetBlkInfo(i);
        if(pBlkInfo == NULL)continue;
        //DA_LOG("\t i=%d, pBlkInfo:%p\n", i, pBlkInfo);
        //DA_LOG("\t cur_frame:0x%x\n", pBlkInfo->cur_frame);
        if(pBlkInfo->cur_frame == -1)continue;
        if(pBlkInfo->cur_frame <0 || pBlkInfo->cur_frame >= pBlkInfo->blk_num)
        {
            DA_LOG("ERROR!!!!!! MMng_Insert. pData:%x, size:%d, blks:%d, ftype:%d, idx:%d, cur_frame:%x,\n", pData,  size,  blks,  frame_type,  idx, pBlkInfo->cur_frame);
            sleep(3);
            continue;
        }
        pBlkInfo->frame[pBlkInfo->cur_frame].ref_serial[idx] = newSerialNo;
    }

    /*
        * assign serial number after memcpy done.
        * this prevents the case that a request get related frame by serial,
        * however memcpy operation is not completed.
        */
    pVidInfo->cur_blk = newBlkNo;
    pVidInfo->cur_frame = newFrameNo;
    DA_LOG("\tMMng_Insert cur_frame:0x%x, frame_num:0x%x\n", pVidInfo->cur_frame, pVidInfo->frame_num);
    pVidInfo->idle_frame_cnt --;

    pVidInfo->frame[newFrameNo].serial = newSerialNo;
    pVidInfo->frame[newFrameNo].fram_type = frame_type;
    pVidInfo->frame[newFrameNo].blkindex = newBlkNo;
    pVidInfo->frame[newFrameNo].blks = blks;
    pVidInfo->frame[newFrameNo].realsize = size+hdrSz;
    pVidInfo->frame[newFrameNo].timestamp_frm = pVidInfo->timestamp_blk;
    //pVidInfo->frame[newFrameNo].timestamp_frm = OSA_getCurTimeInMsec_64();
    DA_LOG("MMng_Insert[type:%d, frm:%d, ser:%d] time:%lld, size:%d, pvid:%p\n", 
        frame_type, pVidInfo->cur_frame, pVidInfo->cur_serial,
        pVidInfo->frame[pVidInfo->cur_frame].timestamp_frm, size, pVidInfo);


	if (frame_type == I_FRAME) {
		if (pVidInfo->gop.last_Start < 0) {
			pVidInfo->gop.last_Start = newFrameNo;
		    pVidInfo->gop.lastest_I = newFrameNo;

			pVidInfo->gop.last_Start_serial = newSerialNo;
		    pVidInfo->gop.lastest_I_serial  = newSerialNo;

			pVidInfo->gop.last_End = SHM_INVALID_SERIAL;
	        pVidInfo->gop.last_End_serial = SHM_INVALID_SERIAL;

		} else {
			pVidInfo->gop.last_Start = pVidInfo->gop.lastest_I;
			pVidInfo->gop.last_Start_serial = pVidInfo->gop.lastest_I_serial;
			if ((pVidInfo->cur_frame - 1) > 0) {
				pVidInfo->gop.last_End = newFrameNo - 1;
			} else {
				pVidInfo->gop.last_End = pVidInfo->frame_num - 1;
			}
			pVidInfo->gop.last_End_serial = newSerialNo - 1;

			pVidInfo->gop.lastest_I = newFrameNo;
			pVidInfo->gop.lastest_I_serial = newSerialNo;

		}
	}


    //MMng_SetFlag(pVidInfo);

    /* Update  global info */
    pVidInfo->cur_blk = (pVidInfo->cur_blk + blks - 1) % pVidInfo->blk_num;
    pVidInfo->blk_free -= blks;

    //MMng_dumpBusyFrame(pVidInfo);

    // update serial at last step    
    if (frame_type != DUMMY_FRAME) {
        pVidInfo->cur_serial ++;
    }
    return 0;
}



int MMng_Write(SHM_ENTRY_TYPE idx, int frame_type,unsigned char *pData, int size  )
{
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);
    if(pVidInfo == NULL) return -1;
    int blk_use = 0;//(size + pVidInfo->blk_sz - 1) / pVidInfo->blk_sz;
    int IsContiguousBlk = 0;
    int free_cnt = 0;
    int hdrSz = 0;
    static int cnt = 0;
    cnt++;

    if ((ENTRY_VIDOE_INFO_MED == idx) && (frame_type == I_FRAME)) {            
        unsigned char* hdrData = NULL;
        MMng_Get_Extra(MMng_GetShmEntryType(E_STREAM_ID_PREVIEW),
                        (unsigned char **) &hdrData, &hdrSz);
    }

    blk_use = (size+ hdrSz + pVidInfo->blk_sz - 1) / pVidInfo->blk_sz;

    if (blk_use > pVidInfo->blk_num || size == 0) {
        DA_LOG("%s size:%d, type:%d, blk_use:%d, blk_num:%d, free:%d, pVidInfo:0x%x\n", 
            __FUNCTION__, size, frame_type, blk_use, pVidInfo->blk_num, pVidInfo->blk_free, pVidInfo);
        DA_LOG("MEM_MNG_WRITE_FAIL. blkuse %d> blknum:0x%x?, size:%d ==0?. \n" , blk_use , pVidInfo->blk_num , size);
        goto MEM_MNG_WRITE_FAIL;
    }

    do { 
        DA_LOG("\t MMng_Write[%d], idx:%d, blk_use:%d, pVidInfo->blk_free:0x%x, free_cnt:%d, idlecnt:0x%x, pVidInfo:0x%x\n", cnt, idx, blk_use , pVidInfo->blk_free, free_cnt,pVidInfo->idle_frame_cnt, pVidInfo);
        MMng_dumpIdleCnt(pVidInfo);
        if (blk_use > pVidInfo->blk_free) {
            //int nextframe = pVidInfo->cur_frame + 1  ; //fengwu
            int nextframe = pVidInfo->cur_frame + 1 + pVidInfo->idle_frame_cnt;
            //int freeframe = (nextframe + free_cnt) % pVidInfo->frame_num;
            int freeframe = (nextframe ) % pVidInfo->frame_num;
                        /* Not enough free blk */
            if(0)DA_LOG(" calling free. currframe:%d, nextframe:%d, frame2free:%d, idlecnt:%d\n",
            		pVidInfo->cur_frame,nextframe,freeframe,pVidInfo->idle_frame_cnt);

            if (MMng_Free(pVidInfo, freeframe) < 0) {                
                DA_LOG("MEM_MNG_WRITE_FAIL. MMng_Free <0. \n" );
                goto MEM_MNG_WRITE_FAIL;
            } else {
                free_cnt++;
                //DA_LOG("%s freeframe:%d,freeD:%d\n", __FUNCTION__, freeframe, free_cnt);
                continue;
            }
        } else {
            int nextblk = (pVidInfo->cur_blk + 1) % pVidInfo->blk_num;
            if ((nextblk + blk_use) > pVidInfo->blk_num) {
                /* No Contiguous Blk */
                int dummy_blk_cnt = pVidInfo->blk_num - nextblk;
                DA_LOG("\t insert dummpy frm, blkcnt:%d, nextblk:%d, SHM_ENTRY_TYPE:%d\n", dummy_blk_cnt, nextblk, idx);
                if (MMng_Insert(NULL, 0, dummy_blk_cnt, DUMMY_FRAME, idx) < 0) {                    
                    DA_LOG("MEM_MNG_WRITE_FAIL. insert <0. \n" );
                    goto MEM_MNG_WRITE_FAIL;
                } else {
                    free_cnt = 0;
                    continue;
                }

            } else {
                IsContiguousBlk = 1;
            }

        }

    } while (!IsContiguousBlk);

    //MMng_Set_timestamp(idx, OSA_getCurTimeInMsec_64());
    if (MMng_Insert(pData, size, blk_use, frame_type, idx) <
        0) {
        DA_LOG("MEM_MNG_WRITE_FAIL. insert2 <0. \n" );
        goto MEM_MNG_WRITE_FAIL;
    }

    return 0;

 MEM_MNG_WRITE_FAIL:
    return -1;
}

void* MMng_Read_internal(VIDEO_FRAME * pFrame,   VIDEO_BLK_INFO * pVidInfo, unsigned char** ppData, int *sz)
{

	if (pVidInfo == NULL || pFrame == NULL || ppData == NULL) {
		DA_LOG("MMng_Video_ReadFrame: invalidate pointer.pVidInfo:%p , pFrame:%p, ppData:%p\n", 
            pVidInfo, pFrame,ppData);
		return NULL;
	}

	if (pFrame->fram_type == DUMMY_FRAME || pFrame->fram_type == EMPTY_FRAME) {
		DA_LOG("MMng_Video_ReadFrame: invalidate frame_type\n");
		return NULL;
	}

	*ppData = (void *)((void*)&(pVidInfo->dataAddrsss) + pFrame->blkindex * pVidInfo->blk_sz);

	DA_LOG("%s data addr:%p, blkidx:%d, bldsz:%d*%d, realsize:%d, pSrc:%p\n",  
		__FUNCTION__,		&(pVidInfo->dataAddrsss), pFrame->blkindex , 
		pVidInfo->blk_sz, pFrame->blks, pFrame->realsize,		*ppData);
 
    if(sz != NULL)*sz=pFrame->realsize;
 	return *ppData;
}


void MMng_Read(SHM_ENTRY_TYPE idx, int serialNO, unsigned char** ppData, int *sz, struct timeval *ptime, int *pIsKey)
{
    if(sz != NULL) *sz = 0;
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(pBlk == NULL) return;

    VIDEO_FRAME *pFrm =NULL;
    if(serialNO == SHM_INVALID_SERIAL) pFrm= MMng_GetFrame_current(pBlk);
    else pFrm= MMng_GetFrame_BySerial(serialNO, idx);
    if(pFrm == NULL) return;

    if(ptime != NULL) 
    {
        *ptime = pFrm->timestamp_frm;
        //DA_LOG("MMng_Read sid:%d, serial:%d, time:%lld\n", idx, serialNO, *ptime);
    }

    if(pIsKey != NULL) *pIsKey = pFrm->fram_type == I_FRAME?1:0 ;

    MMng_Read_internal(pFrm, pBlk, ppData, sz);
}
    

VIDEO_BLK_INFO *MMng_GetBlkInfo(SHM_ENTRY_TYPE idx)
{
    if(idx >=ENTRY_INFO_CNT)
    {
        DA_LOG("error! idx %d is more than %d\n", idx, ENTRY_INFO_CNT);
        return NULL;
    }

    if(g_pBlkAddr[idx]!=NULL)return g_pBlkAddr[idx];
    
	void *pAddr = (void*)g_pSharedMem;
	pAddr =  (void*)(pAddr+sizeof(MEM_MNG_INFO));
	//printf("MMng_GetBlkInfo, paddr:%p\n", pAddr);
	int i;
	for(i=0;i<idx;i++)
	{
		pAddr = pAddr + g_pSharedMem->video_info_size[i];
		//printf("MMng_GetBlkInfo[%d], paddr:%p\n", i, pAddr);
	}

    g_pBlkAddr[idx] = pAddr;
	return pAddr;
}


/**
 * @brief	Get total memory size
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	totalsize
 */
unsigned long MMng_GetMemSize()
{
    MEM_MNG_INFO * pMemInfo = MMng_GetShMem();
	unsigned long totalsize =  sizeof(MEM_MNG_INFO);
	int cnt = 0;
	for (cnt = 0; cnt < ENTRY_INFO_CNT; cnt++) {
			 totalsize +=  pMemInfo->video_info_size[cnt];
             DA_LOG("av_shm[%d]:sz:%d, vnum:%d\n", cnt, pMemInfo->video_info_size[cnt],pMemInfo->video_info_nums); 
	} 

	return totalsize;
}



/**
 * @brief	Get current serial number
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current serial number
 */
int MMng_GetSerial_new(SHM_ENTRY_TYPE idx, int **ppRefSial)
{
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(pBlk == NULL) return SHM_INVALID_SERIAL;        
	int currSerial = pBlk->cur_serial;

    // get ref serial
    if(ppRefSial != NULL) {
        *ppRefSial = pBlk->frame[pBlk->cur_frame].ref_serial;
    }
    return currSerial;
}

int MMng_GetSerial_Del(SHM_ENTRY_TYPE idx)
{
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(pBlk == NULL) return SHM_INVALID_SERIAL;
	return pBlk->del_serial;
}

/* return : 0 - false, 1 - true */
int MMng_IsValidSerial(SHM_ENTRY_TYPE idx, int serial)
{
    
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(serial >( pBlk->cur_serial+1)) return 0;
        
    int serialDel = MMng_GetSerial_Del(idx);
    if(serialDel == SHM_INVALID_SERIAL) return 1;
    if(serial <= serialDel) 
    {
        DA_LOG("MMng_IsValidSerial serial:%d, serialDel:%d\n", serial, serialDel);
        return 0;
    }
    return 1;
}

int MMng_GetSerial_I(SHM_ENTRY_TYPE idx, int **ppRefSial)
{
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(pBlk == NULL) return SHM_INVALID_SERIAL;
    //printf("MMng_GetSerial_I pBlk:%p\n", pBlk);
    if(pBlk->gop.lastest_I == SHM_INVALID_SERIAL) return SHM_INVALID_SERIAL;
    printf("MMng_GetSerial_I lastest_I:%d\n", pBlk->gop.lastest_I);
    // get ref serial
    if(ppRefSial != NULL) {
        *ppRefSial = pBlk->frame[pBlk->gop.lastest_I].ref_serial;
    }
	return pBlk->gop.lastest_I_serial;
}

int MMng_GetAudioSerialByTimestamp(SHM_ENTRY_TYPE idx, struct timeval timestamp)
{
    long long time = timestamp.tv_sec * 1000000ll + timestamp.tv_usec;
    VIDEO_BLK_INFO *pBlk = MMng_GetBlkInfo(idx);
    if(pBlk == NULL) return SHM_INVALID_SERIAL;
    printf("MMng_GetSerial_I pBlk:%p\n", pBlk);

    int index = pBlk->cur_frame;
    do{
        struct timeval temp_time = pBlk->frame[index].timestamp_frm;
        long long time1 = temp_time.tv_sec * 1000000ll + temp_time.tv_usec; 

        if(time1 <= time) {
            //printf("find valid serial:%d  \n", pBlk->frame[index].serial);
            return pBlk->frame[index].serial;    
        }
    }while(index--);
    

    return -1;
}

/**
 * @brief	Get current frame
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current frame
 */
VIDEO_FRAME *MMng_GetFrame_current(VIDEO_BLK_INFO * video_info)
{
	if (video_info == NULL)		return NULL;
	if (video_info->cur_frame < 0)		return NULL;
	return &(video_info->frame[video_info->cur_frame]);
}
/**
 * @brief	Get video frame by serial
 * @param	"int serial" : serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	frame address ; NULL
 */
VIDEO_FRAME *MMng_GetFrame_BySerial(int serial, SHM_ENTRY_TYPE idx)
{
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);
	if (serial < 0 || pVidInfo == NULL) {
		DA_LOG("MMng_GetFrameBySerial: Fail . serial:%d, idx:%d, pVidInfo:%p\n", 
            serial, idx, pVidInfo);
		return NULL;
	}

    if(serial > pVidInfo->cur_serial) return NULL;
    
	int cnt = 0;
/*
	for (cnt = 0; cnt < pVidInfo->frame_num; cnt++) {
		if (serial == pVidInfo->frame[cnt].serial) {
			if (pVidInfo->frame[cnt].fram_type == DUMMY_FRAME ||
			    pVidInfo->frame[cnt].fram_type == EMPTY_FRAME) {
				DA_LOG("MMng_GetFrameBySerial: invalidate frame_type\n");
				return NULL;
			}
			return &(pVidInfo->frame[cnt]);
		}
	}
*/
	//Log_set_mode(LOG_MODE_ALL);
    int oldCurrFrame = pVidInfo->cur_frame;
	for(cnt = pVidInfo->cur_frame; cnt > (pVidInfo->cur_frame - pVidInfo->frame_num); cnt --)
	{
		int frmIdx = cnt;
		if(cnt < 0) frmIdx = cnt + pVidInfo->frame_num;
		if (serial == pVidInfo->frame[frmIdx].serial) {
			if (pVidInfo->frame[frmIdx].fram_type == DUMMY_FRAME ||
			    pVidInfo->frame[frmIdx].fram_type == EMPTY_FRAME) {
				DA_LOG("MMng_GetFrameBySerial %d. videoType:%d, invalidate frame_type(%d). this is caused by cirbuf rewind. continue\n", 
                    serial, pVidInfo->video_type,  pVidInfo->frame[frmIdx].fram_type);
				continue;
			}
			//DA_LOG("MMng_GetFrameBySerial[%d] .cSer:%d, cFrm:%d(old:%d), search serial succeed! search cnt:%d, frmSz:%d\n", 
            //    serial,pVidInfo->cur_serial,pVidInfo->cur_frame, oldCurrFrame, pVidInfo->cur_frame - cnt, pVidInfo->frame[frmIdx].realsize);
			return &(pVidInfo->frame[frmIdx]);
		}
	}
	
	DA_LOG("MMng_GetFrameBySerial:%d. Search no frame!!!!!!!!!!!!!!!!!!!!!!!  cur_frame:0x%x, curr serial:0x%x, idx:%d, pVidInfo:0x%x\n", 
        serial, pVidInfo->cur_frame, pVidInfo->cur_serial, idx, pVidInfo);
    //MMng_dumpVInfo(idx);
    //MMng_dumpBusyFrame(pVidInfo);
	return NULL;

}

/**
 * @brief	Get current frame
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current frame
 */
VIDEO_FRAME *MMng_GetFrame_LastI(VIDEO_BLK_INFO * video_info)
{
	if (video_info == NULL)		return NULL;
	if (video_info->gop.lastest_I < 0)		return NULL;
	return &(video_info->frame[video_info->gop.lastest_I]);
}


int MMng_Put_Extra(SHM_ENTRY_TYPE idx, unsigned char *pData, int size)
{
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);
    if(pVidInfo == NULL) return 0;
	if(pData == NULL) return 0;
	if(size <= 0) return 0;
	
	memcpy(pVidInfo->extradata, pData, size);
	pVidInfo->extrasize = size;

	return size;
}

int MMng_Get_Extra(SHM_ENTRY_TYPE idx, unsigned char **ppData, int *sz)
{
	if(ppData == NULL) return 0;	
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);
    if(pVidInfo == NULL) return 0;
	
	if(pVidInfo->extrasize <=0 ) return 0;

	*ppData = pVidInfo->extradata;
	if(sz != NULL)*sz = pVidInfo->extrasize;
	
	return pVidInfo->extrasize;
}

int MMng_Set_timestamp(SHM_ENTRY_TYPE idx, struct timeval timestamp)
{
    VIDEO_BLK_INFO *pVidInfo = MMng_GetBlkInfo(idx);
    if(pVidInfo == NULL) return -1;
    pVidInfo->timestamp_blk = timestamp;
    return 0;
}


