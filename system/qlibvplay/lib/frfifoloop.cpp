#include <frfifoloop.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <define.h>

uint32_t calCrc32(  char *buf, uint32_t size);

//#define FRFIFOLOOP_TRC(args ...)  printf(args)
#define FRFIFOLOOP_TRC(args ...)
#define FRFIFOLOOP_DBG(args ...)  printf(args)
#define FRFIFOLOOP_ERR(args ...)  printf(args)

void save_fr_data_t(FILE *fd ,fr_data_t *pFrd,int unitNum){
    fprintf(fd,"\n------priv:%4d size:%6d hash:%8X %8lld \n",pFrd->fr.priv,pFrd->fr.size ,pFrd->hash,pFrd->fr.timestamp);
    for(int i = 0 ;i<unitNum ;i++){
        if(pFrd->unitIndexList[i] == -1){
            break;
        }
        fprintf(fd,"%4d",pFrd->unitIndexList[i]);
    }
    fprintf(fd, "\n+++++++++++save end \n");
    fflush(fd);
}
FrFifoLoop::~FrFifoLoop(){
    //TODO :: free resource
    if(this->FrDataBuf != NULL ){
        free(this->FrDataBuf);
    }
    if(this->FrDataList != NULL ){
        free(this->FrDataList);
    }
    if(this->UnitStateList != NULL ){
        free(this->UnitStateList);
    }
    if(this->UnitIndexList != NULL ){
        free(this->UnitIndexList);
    }
}
bool FrFifoLoop::Prepare(void){




#if 1
    int bufSize = 0;
    fr_data_t *pFrData = NULL ;
    this->FrDataBuf = (char *)malloc(this->TotalBufSize);
    if(this->FrDataBuf == NULL ){
        FRFIFOLOOP_ERR("malloc total buf error\n");
        goto Prepare_Error_Handler ;
    }
    memset(this->FrDataBuf, 0,this->TotalBufSize);

    this->TotalUnitNum =  this->TotalBufSize /this->UnitSize ;
    //malloc all unit state list
    bufSize = this->TotalUnitNum *sizeof(uint8_t );
    this->UnitStateList = (uint8_t * )malloc(bufSize);
    if (this->UnitStateList == NULL){
        FRFIFOLOOP_ERR("malloc unit state list error ->%d\n",bufSize);
        goto Prepare_Error_Handler ;
    }
    memset(this->UnitStateList ,0 ,bufSize);

    //malloc all struct fr_data_t list
    bufSize = sizeof(fr_data_t )* this->MaxBufNum ;
    this->FrDataList  =(fr_data_t *) malloc(bufSize );
    if(this->FrDataList == NULL ){
        FRFIFOLOOP_ERR("malloc unit state list error->%d\n",bufSize);
        goto Prepare_Error_Handler ;
    }
    memset(this->FrDataList ,0,bufSize);

    //malloc all fr_data_t->UnitIndexList
    this->UnitNumPerData =  this->MaxBufSize/this->UnitSize + 2 ;
    bufSize =  this->UnitNumPerData * sizeof(int) * this->MaxBufNum ;
    this->UnitIndexList = (int32_t *) malloc(bufSize);
    if(this->UnitIndexList == NULL ){
        FRFIFOLOOP_ERR("malloc unit state list error->%d\n",bufSize);
        goto Prepare_Error_Handler ;
    }
    memset(this->UnitIndexList ,-1,bufSize); //set -1 means point no memory
    //init add fr_data_t unit index list
    pFrData = (fr_data_t *) this->FrDataList ;
    for(int i =  0;i < this->MaxBufNum;i++){
        pFrData[i].unitIndexList =  this->UnitIndexList + i * this->UnitNumPerData ;
    }

    FRFIFOLOOP_TRC("FrDataBuf    ->%p\n",this->FrDataBuf);
    FRFIFOLOOP_TRC("FrDataList   ->%p\n",this->FrDataList);
    FRFIFOLOOP_TRC("UnitStateList->%p\n",this->UnitStateList);
    FRFIFOLOOP_TRC("UnitIndexList->%p\n",this->UnitIndexList);

    char buf[64];
    memset(buf,0,64);
    if(this->HashEnabled){
        sprintf(buf,"/nfs/push_%s.txt",this->Name);
        this->PushFd =  fopen(buf,"wb+");
        if(this->PushFd == NULL){
            printf("open trace file error ->%s\n",buf);
            return false ;
        }
        else{
            printf("push trace file ->%p ->%s<-\n",this->PushFd,buf);
        }
    }


    return true  ;
Prepare_Error_Handler :
    printf("fifo prepare error\n");
    if(this->FrDataBuf != NULL ){
        free(this->FrDataBuf);
    }
    if(this->FrDataList != NULL ){
        free(this->FrDataList);
    }
    if(this->UnitStateList != NULL ){
        free(this->UnitStateList);
    }
    if(this->UnitIndexList != NULL ){
        free(this->UnitIndexList);
    }
    return false ;
#else
    int ret = 0 ;
    ret = fr_alloc(&this->FrInst, this->Name, this->TotalBufSize,
                 FR_FLAG_NODROP |FR_FLAG_FLOAT
                 |FR_FLAG_MAX(this->MaxBufSize) );

    //ret = fr_alloc(&fr, "frlib-nodrop", 0xa00000,
    //            FR_FLAG_FLOAT | FR_FLAG_NODROP
    //
    //            | FR_FLAG_MAX(0x150000));
    if(ret < 0 ){
        FRFIFOLOOP_ERR("init FrFifoLoop error\n");
        return false ;
    }
    fr_INITBUF(&this->FrInBuf, &this->FrInst);
    fr_INITBUF(&this->FrOutBuf, &this->FrInst);

#endif
    return true;
}
FrFifoLoop::FrFifoLoop(int bufNum ,int bufSize ,int maxBufSize,int sizeRate ,int unitKbSize  ,const char *name) {
    this->MaxBufNum = bufNum ;
    this->BufSize = (bufSize/1024 +1 ) *1024 ;
    this->MaxBufSize = (maxBufSize /1024 +1 )*1024 ;
    this->UnitSize = unitKbSize * 1024 ;

    strncpy(this->Name ,name ,FRFIFOLOOP_MAX_NAME);
    this->Name[FRFIFOLOOP_MAX_NAME-1] =  0;

    this->TotalBufSize  = ( (  (this->MaxBufNum * this->BufSize /100) *sizeRate ) /this->UnitSize +1 )*this->UnitSize  ;
    this->CurFrNum = 0;
    FRFIFOLOOP_DBG("FrFifoLoop :%s-> curFrNum:%d maxBufNum:%d maxBufSize:%d bufSize:%d  total:%dkb.%db unitSize:%d\n",
            this->Name ,
            this->CurFrNum ,
            this->MaxBufNum,
            this->MaxBufSize,
            this->BufSize ,
            this->TotalBufSize/1024 ,
            this->TotalBufSize %1024,
            this->UnitSize);
    this->PushUnitIndex = 0 ;
    //this->PopUnitIndex = 0 ;
    this->BusyUnitNum =  0;
    this->HashEnabled =  false ;



    this->UnitStateList = NULL ;
    this->FrDataList = NULL ;
    this->FrDataBuf  = NULL ;
    this->PushDataIndex = 0 ;
    this->PopDataIndex = 0;

    this->PushFd = NULL;
    this->PopFd  = NULL;
    this->CacheTimeMsSecond = 0;
    this->PopFrTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;

    this->IncCounter = 0;
    this->PushCounter = 0;
    this->PopCounter = 0;
    this->PopNullCounter = 0;
}

#if 0
FrFifoLoop::FrFifoLoop(int msecond ,int bufSize){


}
#endif
int FrFifoLoop::GetQueueSize(void){
    this->QueueLock.lock();
    int ret = this->CurFrNum;
    this->QueueLock.unlock();
    return ret;
}
int FrFifoLoop::GetQueueFreeSize(void){
    return this->MaxBufNum - this->CurFrNum ;
}
int FrFifoLoop::GetFreeUnitNum(void){
    return this->TotalUnitNum - this->BusyUnitNum ;
}

int FrFifoLoop::Push(struct fr_buf_info fr) {
    int ret  = 1;
    fr_data_t  *pFrd  = NULL;
    this->QueueLock.lock();
    //try check if there are enough memory
#if 0
    if(this->CurFrNum >= this->MaxBufNum ){
        pFrd = this->FrDataList +this->PopDataIndex ;
        this->PopNullFr(pFrd);
        this->PopDataIndex++;
        if(this->PopDataIndex >= this->MaxBufNum ){
            this->PopDataIndex = 0 ;
        }
        this->CurFrNum--;
    }
#endif
    int freeSize = this->GetFreeUnitNum() * this->UnitSize ;
    if (fr.size >  freeSize) {
        FRFIFOLOOP_ERR("%s -> free size is lower %d %d %d\n",this->Name,freeSize,fr.size,this->CurFrNum);
        ret = -1;
    }
    else {
        uint64_t newFrTimeStamp = fr.timestamp;
        if(this->PopFrTimeStamp == MEDIA_MUXER_HUGE_TIMESDTAMP){
            this->PopFrTimeStamp = newFrTimeStamp;
        }
        if(this->CacheTimeMsSecond == 0){
            if(this->CurFrNum >= this->MaxBufNum){
                pFrd =  this->FrDataList + this->PopDataIndex;
                this->PopFrTimeStamp = pFrd->fr.timestamp;
                this->PopNullFr(pFrd);
                this->PopDataIndex++;
#if 0
                FRFIFOLOOP_ERR("push pop null ++++ index ->push %3d : pop %3d cur:%3d %s\n",
                      this->PushDataIndex,this->PopDataIndex, this->CurFrNum, this->Name);
#endif
            }
            else {
                this->CurFrNum++;
            }

        }
        else{
            //check cache time length
            if(newFrTimeStamp > this->PopFrTimeStamp){
                if((newFrTimeStamp - this->PopFrTimeStamp) > this->CacheTimeMsSecond){
                    pFrd =  this->FrDataList + this->PopDataIndex;
                    this->PopFrTimeStamp = pFrd->fr.timestamp;
                    this->PopNullFr(pFrd);
                    this->PopDataIndex++;
#if 0
                    FRFIFOLOOP_ERR("push pop null cache++++ index ->push %3d : pop %3d cur:%3d %s\n",
                            this->PushDataIndex,this->PopDataIndex, this->CurFrNum, this->Name);
                    FRFIFOLOOP_ERR("%s-->ts:  %8lld %8lld %8lld\n", this->Name,
                            newFrTimeStamp, this->PopFrTimeStamp, this->CacheTimeMsSecond);
#endif
                }
                else{
                    this->CurFrNum++;
                }
            }
            else{
                printf("#### error %s timestamp %8lld %8lld\n", this->Name,
                        newFrTimeStamp,
                        this->PopFrTimeStamp);
                this->CurFrNum++;
            }

        }
        if(this->PopDataIndex >= this->MaxBufNum){
            this->PopDataIndex = 0 ;
        }
#if 0
            if(this->CacheTimeMsSecond != 0){
                printf("push pop -> %3d %8lld -%8lld =%8lld ->%s\n", this->CurFrNum,
                        newFrTimeStamp,
                        this->PopFrTimeStamp,
                        newFrTimeStamp - this->PopFrTimeStamp,
                        this->Name);
            }
#endif


        pFrd = this->FrDataList + this->PushDataIndex ;
        //store
        if(this->HashEnabled ){
            pFrd->hash = calCrc32((char *)fr.virt_addr,fr.size);
            FRFIFOLOOP_TRC("*******hash ->%d %X\n",fr.size,pFrd->hash);
        }
        if( this->PushFr(&fr,pFrd) < 0 ){
            FRFIFOLOOP_ERR("pushfr error\n");
            ret =  -1;
        }
        else {
            this->PushDataIndex++;
            if(this->PushDataIndex >= this->MaxBufNum ) {
                this->PushDataIndex = 0;
            }
            if(this->HashEnabled){
                save_fr_data_t(this->PushFd,pFrd,this->UnitNumPerData);
                //printf("trace-> %s_%d\n",__FILE__,__LINE__);
            }
            FRFIFOLOOP_TRC("push ->%d %d %d\n",this->PushDataIndex,this->PopDataIndex,this->CurFrNum);
            ret = 1;
        }
    }
#if 0
    if(ret > 0  && this->PushCounter % this->MaxBufNum == 0){
        FRFIFOLOOP_ERR("###%s fr sumary: push:%4d pop:%4d null:%4d cur:%3d\n", this->Name,
                this->PushCounter, this->PopCounter, this->PopNullCounter, this->CurFrNum);
    }
#endif
    this->QueueLock.unlock();
    return ret;
}
int FrFifoLoop::Pop( struct fr_buf_info *pFr ) {
    int ret  = 1;
    fr_data_t  *pFrd = NULL ;
    this->QueueLock.lock();
    if(this->CurFrNum <= 0 ) {
        ret = 0;
    }
    else{
        FRFIFOLOOP_TRC("current fr data index ->push %d: pop%d\n",this->PushDataIndex,this->PopDataIndex);
        pFrd = this->FrDataList +this->PopDataIndex ;
        this->PopFrTimeStamp = pFrd->fr.timestamp;
        ret =  this->PopFr(pFr,pFrd);
        if(ret == 0 ){
            ret  = 0;
        }
        else if(ret > 0 ){
                //printf("trace-> %s_%d\n",__FILE__,__LINE__);
            //step next PopDataIndex
            this->PopDataIndex++;
            if(this->PopDataIndex >= this->MaxBufNum ){
                this->PopDataIndex = 0;
            }
            this->CurFrNum--;

            //    printf("trace-> %s_%d\n",__FILE__,__LINE__);
            if(this->HashEnabled ){
                uint32_t crc32  = calCrc32((char *)pFr->virt_addr,pFr->size);
                if(crc32 != pFrd->hash ){
                    FRFIFOLOOP_TRC("current index ->%d %d\n",this->PushDataIndex,this->PopDataIndex);
                    FRFIFOLOOP_ERR("?? hash error %d : %X %X  %4d\n",pFr->size,crc32 ,pFrd->hash,this->UnitNumPerData);
                    for(int i = 0 ;i< this->UnitNumPerData ;i++){
                        printf("%4d",pFrd->unitIndexList[i]);
                    }
                    printf("\n");
                }
                else {
                //    printf("pop frame hash ok ->%8lld size%4d %X %X\n",pFr->timestamp,pFr->size,crc32  ,pFrd->hash);
                }
            }
            printf("");
            FRFIFOLOOP_TRC("pop  ->%d %d %d\n",this->PushDataIndex,this->PopDataIndex,this->CurFrNum);
        }
        else {
            ret = -1;

        }
    }
    this->QueueLock.unlock();
    return ret ;
}
int FrFifoLoop::PushFr(struct fr_buf_info *pFr ,fr_data_t  *pFrd) {
    int block = pFr->size /this->UnitSize ;
    int extra = pFr->size %this->UnitSize ;
    int i  = 0;
    FRFIFOLOOP_TRC("Push fr ->%6d %3d %3d ->%p\n",pFr->size,block,extra,pFrd);
    //return current unit memory if it has
    for ( i = 0 ;i < this->UnitNumPerData ;i++ ){
        if(pFrd->unitIndexList[i] == -1 ){
            continue;
            //break ;
        }
        else {
            this->UnitStateList[pFrd->unitIndexList[i]] = 0; //mask as free
            pFrd->unitIndexList[i] = -1;
            this->BusyUnitNum--;
            //TODO : do i need update This->PopUnitIndex ?
        }
    }
    memset(pFrd->unitIndexList ,-1, sizeof(int32_t )* this->UnitNumPerData );
    //get block+1 units for store
    FRFIFOLOOP_TRC("push search unit -> ");
    int needBlock = block ;
    if(extra ){
        needBlock++;
    }
    for( i =0;i<needBlock;i++){
        while(1){
            this->PushUnitIndex++;
            if(this->PushUnitIndex >= this->TotalUnitNum ){
                this->PushUnitIndex = 0;
            }
            if(this->UnitStateList[this->PushUnitIndex] == 0){
                FRFIFOLOOP_TRC("%5d",this->PushUnitIndex);
                break ;
            }
            else {
        //        this->PushUnitIndex++;
                continue ;
            }
        }
        this->BusyUnitNum++;
        this->UnitStateList[this->PushUnitIndex] = 1; //mark as busy,other fr can not use it
        pFrd->unitIndexList[i] =  this->PushUnitIndex ;
    }
    FRFIFOLOOP_TRC("\n");
    FRFIFOLOOP_TRC("unit sum->%d %d\n",this->TotalUnitNum,this->BusyUnitNum);
    char *src = NULL ;
    char *dst = NULL ;
    FRFIFOLOOP_TRC("fill buf -> %p %d %d:%d\n",pFr->virt_addr,pFr->size,block ,extra);
    for( i = 0 ;i < block ;i++){
        src = (char *)pFr->virt_addr + i *this->UnitSize ;
        dst = this->FrDataBuf +  this->UnitSize * (pFrd->unitIndexList[i]) ;
        FRFIFOLOOP_TRC("\t %3d:%3d : %p->%p\n",i,pFrd->unitIndexList[i],src,dst);
        memcpy(dst, src ,this->UnitSize );
    }
    if(extra ){
        src = (char *)pFr->virt_addr + i *this->UnitSize ;
        dst = this->FrDataBuf +  this->UnitSize * pFrd->unitIndexList[i] ;
        FRFIFOLOOP_TRC("\te%3d:%3d : %p->%p\n",i,pFrd->unitIndexList[i],src,dst);
        memcpy(dst, src ,extra );
    }
    pFrd->fr.size = pFr->size ;
    pFrd->fr.timestamp =  pFr->timestamp ;
    pFrd->fr.priv = pFr->priv ;
    pFrd->fr.serial = pFr->serial;
    //pFrd->fr.mapped_size = pFr->mapped_size;
    pFrd->fr.serial = this->IncCounter;
    this->PushCounter++;
#if 0
    if(strstr(this->Name, "Audio") )
            printf("update fr push counter %s->%d\n", this->Name, this->IncCounter);
#endif
    this->IncCounter++;
    return 1 ;
}
int FrFifoLoop::PopFr(struct fr_buf_info *pFr ,fr_data_t  *pFrd) {
    int block,extra,i  ;
    block =  pFrd->fr.size / this->UnitSize ;
    extra =  pFrd->fr.size % this->UnitSize ;
    FRFIFOLOOP_TRC("Pop fr ->%6d %3d %3d ->%p\n",pFrd->fr.size,block,extra,pFr);

    FRFIFOLOOP_TRC("pop  search uint -> ");
    for(i = 0 ;i<this->UnitNumPerData ;i++){
        if (pFrd->unitIndexList[i] != -1 ){
            FRFIFOLOOP_TRC("%5d",pFrd->unitIndexList[i]);
        }
        else {
            continue;
        }
    }
    FRFIFOLOOP_TRC("\n");
    char *src = NULL ;
    char *dst = NULL ;
    FRFIFOLOOP_TRC("read buf -> %p %d %d:%d\n",pFr->virt_addr,pFr->size,block ,extra);
    for(i=0 ;i<block;i++){
        src = this->FrDataBuf +  this->UnitSize * (pFrd->unitIndexList[i]) ;
        dst = (char *)pFr->virt_addr + i *this->UnitSize ;
        FRFIFOLOOP_TRC("\t %3d:%3d : %p->%p\n",i,pFrd->unitIndexList[i],src,dst);
        memcpy(dst, src ,this->UnitSize );
    }
    if(extra ){
        dst = (char *)pFr->virt_addr + i *this->UnitSize ;
        src = this->FrDataBuf +  this->UnitSize * pFrd->unitIndexList[i] ;
        FRFIFOLOOP_TRC("\te%3d:%3d : %p->%p\n",i,pFrd->unitIndexList[i],src,dst);
        memcpy(dst, src ,extra );
    }


    pFr->timestamp = pFrd->fr.timestamp ;
    pFr->priv = pFrd->fr.priv ;
    pFr->size = pFrd->fr.size ;
    pFr->serial = pFrd->fr.serial;
    this->PopCounter++;
   // pFr->mapped_size = pFrd->fr.mapped_size;



//return memory unit to list
    for ( int i = 0 ;i < this->UnitNumPerData ;i++ ){
        if(pFrd->unitIndexList[i] == -1 ){
            continue ;
        }
        else {
            this->UnitStateList[pFrd->unitIndexList[i]] = 0; //mask as free
            pFrd->unitIndexList[i] = -1;
            this->BusyUnitNum--;
            //TODO : do i need update This->PopUnitIndex
        }
    }
    return 1 ;
}
void FrFifoLoop::SetHashState(bool enabled){
    this->HashEnabled = enabled ;
}




int FrFifoLoop::PopNullFr(fr_data_t  *pFrd) {
    FRFIFOLOOP_TRC("pop  search uint -> ");
    for(int i = 0 ;i<this->UnitNumPerData ;i++){
        if (pFrd->unitIndexList[i] != -1 ){
            FRFIFOLOOP_TRC("%5d",pFrd->unitIndexList[i]);
        }
        else {
            continue;
        }
    }
    FRFIFOLOOP_TRC("\n");
//return memory unit to list
    for ( int i = 0 ;i < this->UnitNumPerData ;i++ ){
        if(pFrd->unitIndexList[i] == -1 ){
            continue ;
        }
        else {
            this->UnitStateList[pFrd->unitIndexList[i]] = 0; //mask as free
            pFrd->unitIndexList[i] = -1;
            this->BusyUnitNum--;
            //TODO : do i need update This->PopUnitIndex
        }
    }
    this->PopNullCounter++;
    return 1 ;
}
int FrFifoLoop::Flush(void){
    int ret  = -1;
    fr_data_t  *pFrd = NULL ;
    this->QueueLock.lock();
    if(this->CurFrNum <= 0 ) {
        ret = -1;
    }
    else{
        while(this->CurFrNum > 0 ){
            FRFIFOLOOP_TRC("%s->current fr data index ->push %d: pop%d ->%d\n",this->Name,this->PushDataIndex,this->PopDataIndex,this->CurFrNum);
            pFrd = this->FrDataList +this->PopDataIndex ;
            ret =  this->PopNullFr(pFrd);
            if(ret == 0 ){
                ret  = 1;
                break;
            }
            else if(ret > 0 ){
                //step next PopDataIndex
                this->PopDataIndex++;
                if(this->PopDataIndex >= this->MaxBufNum ){
                    this->PopDataIndex = 0;
                }
                this->CurFrNum--;
                FRFIFOLOOP_TRC("pop  ->%d %d %d\n",this->PushDataIndex,this->PopDataIndex,this->CurFrNum);
            }
            else {
                ret = -1;
                break ;
            }
        }
    }
    this->PopFrTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->IncCounter = 0;
    this->PushCounter = 0;
    this->PopCounter = 0;
    this->PopNullCounter = 0;
    this->QueueLock.unlock();
    return ret;
}
void FrFifoLoop::SetCacheTime(uint64_t msecond){
    this->QueueLock.lock();
    this->CacheTimeMsSecond = msecond;
    this->QueueLock.unlock();
}
uint64_t FrFifoLoop::GetCacheTime(void){
    uint64_t ret = 0;
    this->QueueLock.lock();
    ret = this->CacheTimeMsSecond;
    this->QueueLock.unlock();
    return ret;
}


