#include <qmediafifo.h>
#include <assert.h>
#include <stdlib.h>
#include <define.h>
#include <string.h>
QMediaFifo::QMediaFifo(char *name){
    this->GetTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->ListFifoItem.empty();
    this->ListGetItem.empty();
    this->HashEnabled = false;
    this->PushCounter = 0;
    this->GetCounter = 0;
    this->PutCounter = 0;
    this->FullCounter = 0;
    this->UsedMemSize = 0;
    this->MonitorFrame= true;
    int len = strlen(name);
    if(len >= Q_MEDIA_FIFO_MAX_NAME){
        len = Q_MEDIA_FIFO_MAX_NAME -1;
        printf("fifo name has been cut short ->%d\n", Q_MEDIA_FIFO_MAX_NAME);
    }
    memcpy(this->Name, name, len);
    this->Name[len] = 0;
    this->CurCacheTimeLen = 0;
}
QMediaFifo::~QMediaFifo(){
    this->Flush();
}


void QMediaFifo::SetMaxCacheTime(uint64_t t){
    this->AccessLock.lock();
    this->MaxCacheTime = t;
    this->AccessLock.unlock();
}
uint64_t QMediaFifo::GetMaxCacheTime(){
    uint64_t ret;
    this->AccessLock.lock();
    ret = this->MaxCacheTime;
    this->AccessLock.unlock();
    return ret;

}
/*
    void SetMaxCacheNum(int32_t n);
    int32_t GetMaxCacheNum();
*/
int QMediaFifo::PushItem(struct fr_buf_info *fr){
    assert(fr!= NULL);
    assert(fr->virt_addr != NULL);
    assert(fr->size >  0);
    fr_data_t frd ;
    memset(&frd,0, sizeof(fr_data_t));
    int ret  = 0;
    void *p = malloc(fr->size);
    if(p == NULL){
        printf("%s-->malloc media fifo item mem error ->%d\n", this->Name, fr->size);
        return -1;
    }
#if 1
    frd.fr = *fr;
    //memcpy(&frd.fr, fr, sizeof(struct fr_buf_info));
    frd.fr.virt_addr = p;
    frd.fr.buf = p;
#else
    frd.fr.timestamp = fr->timestamp;
    frd.fr.size = fr->size;
    frd.fr.serial = fr->serial;
    frd.fr.priv = fr->priv;
    frd.fr.virt_addr = p;
    frd.fr.buf = p;
#endif
    memcpy(p, fr->virt_addr, fr->size);
    this->AccessLock.lock();
    if(this->GetTs == MEDIA_MUXER_HUGE_TIMESDTAMP){
        this->GetTs = fr->timestamp;
        frd.fr.serial = 0;
    }
    if(this->GetTs > fr->timestamp){
        printf("%s input ts error ->cur:%8lld get:%8lld\n", this->Name, fr->timestamp, this->GetTs);
    }
   // if(fr->size == 4){LOGGER_DBG("dummy\n")};
    if(this->MaxCacheTime != 0){
        if((fr->timestamp - this->GetTs) >= this->MaxCacheTime && this->ListFifoItem.size() != 0){
            //cache enough and remove the last
#if 0
            LOGGER_WARN("fifo %s full ->%8lld %8lld  %8lld  %5d\n",
                    this->Name,
                    fr->timestamp,
                    this->GetTs,
                    this->CurCacheTimeLen,
                    this->ListFifoItem.size()
                  );
#endif
            fr_data_t temp = this->ListFifoItem.back();
    //if(fr->size == 4){LOGGER_DBG("dummy\n")};
            if(this->HashEnabled){
                //check hash value
                this->CheckHashItem(&temp);
            }
            this->FullCounter++;
#if 1
            if (this->MonitorFrame)
            {
                LOGGER_WARN("#####%s fifo drop frame ->index:%d push:%8lld drop:%8lld %8p, GetTs:%llu,cacheTime:%llu,size:%d,CurCacheTimeLen:%llu\n",
                        this->Name,
                        this->PushCounter,
                        fr->timestamp,
                        temp.fr.timestamp,
                        temp.fr.virt_addr,
                        this->GetTs,
                        this->MaxCacheTime,
                        this->ListFifoItem.size(),
                        CurCacheTimeLen
                        );
            }
#endif
            if(temp.fr.virt_addr != NULL){
                this->UsedMemSize -= temp.fr.size;
                free(temp.fr.virt_addr);
            }
            this->GetTs = temp.fr.timestamp;
            this->ListFifoItem.pop_back();
        }
    }
    // printf("malloc ->%4d %8d  %p\n", this->PushCounter, fr->size, p);
    this->PushCounter++;
    frd.fr.serial = this->PushCounter;
    this->UsedMemSize += frd.fr.size;
    if(this->HashEnabled){
        //gen hash value
        this->HashItem(&frd);
    }
#if 0
    printf("fifo %s push ->%3d %8d %p serial: %d\n",
            this->Name,
            frd.fr.serial,
            frd.fr.size,
            frd.fr.virt_addr,
            frd.fr.serial);
#endif
    this->ListFifoItem.push_front(frd);
    this->UpdateCurCacheLen();
    this->AccessLock.unlock();
    return 1;
}
int QMediaFifo::GetItem(struct fr_buf_info *fr){
    fr_data_t  frd = {0};
    int ret = 0;
    this->AccessLock.lock();
    if(this->ListFifoItem.size() == 0){
        ret = 0;
    }
    else{
        frd = this->ListFifoItem.back();
        if(this->HashEnabled){
            //check hash
            this->CheckHashItem(&frd);
        }
        this->GetTs = frd.fr.timestamp;
        this->ListFifoItem.pop_back();
        //put in the get item list for later check
        this->ListGetItem.push_front(frd);
        this->GetCounter++;
        this->UpdateCurCacheLen();
#if 0
        printf("get ->%3d %8d %p\n", frd.fr.serial,
                frd.fr.size,
                frd.fr.virt_addr);
#endif
        *fr = frd.fr;
        ret = 1;
    }
    this->AccessLock.unlock();
    return ret;
}

int QMediaFifo::ReleaseItem(struct fr_buf_info *fr)
{
    assert(fr != NULL);
    assert(fr->virt_addr != NULL);
    assert(fr->size > 0);

    bool found = false;
    int ret = 0;
    std::list<fr_data_t>::iterator itor;
    int num = 0;

    num = this->ListGetItem.size();
    if(num == 0)
    {
        printf("%s wrong api call method, must get then put\n", this->Name);
        ret = -1;
    }
    else
    {
        this->PutCounter++;
        itor = this->ListGetItem.begin();
        while(itor != this->ListGetItem.end())
        {
            if ((itor->fr.virt_addr != NULL) && (itor->fr.virt_addr == fr->virt_addr))
            {
                this->UsedMemSize -= itor->fr.size;
                free(itor->fr.virt_addr);
                fr->virt_addr = fr->buf = NULL;
                found = true;
                break;
            }
            itor++;
        }
        if(found == true)
        {
            this->ListGetItem.erase(itor);
            ret = 1;
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

bool QMediaFifo::CheckItemInList(std::list <fr_data_t> *pStList, fr_data_t stItem)
{
    std::list<fr_data_t>::iterator itor;

    if (pStList == NULL)
    {
        return false;
    }

    for (itor = pStList->begin();
         itor != pStList->end();
         itor++)
    {
        if (itor->fr.virt_addr == stItem.fr.virt_addr)
        {
            return true;
        }
    }
    return false;
}

int QMediaFifo::PutItem(struct fr_buf_info *fr)
{
    assert(fr != NULL);
    assert(fr->virt_addr != NULL);
    assert(fr->size > 0);

    fr_data_t frd = {0};
    std::list<fr_data_t>::iterator itor;
    int ret = 1;

    this->AccessLock.lock();
    if (this->u64KeepFrameTime <= 0)
    {
        ReleaseItem(fr);
    }
    else
    {
        memset(&frd,0, sizeof(fr_data_t));
        frd.fr = *fr;

        if (CheckItemInList(&(this->ListKeepItem), frd) == false)
        {
            this->ListKeepItem.push_front(frd);
        }
        frd = this->ListKeepItem.back();
        while ((frd.fr.timestamp + this->u64KeepFrameTime < this->GetTs) &&
            (this->ListGetItem.size() > 0))
        {
            #if 0
            printf("Remove %s fifo->%5d size:%8d %p time:%llu getTs:%llu list queue size:%d, keep queue size:%d get queue size:%d\n",
                this->Name, frd.fr.serial, frd.fr.size, frd.fr.virt_addr,
                frd.fr.timestamp, this->GetTs,
                this->ListFifoItem.size(),
                this->ListKeepItem.size(),
                this->ListGetItem.size());
            #endif
            this->ListKeepItem.pop_back();
            ReleaseItem(&(frd.fr));
            if (this->ListKeepItem.size() == 0)
            {
                break;
            }
            frd = this->ListKeepItem.back();
        }
    }
    this->AccessLock.unlock();
    return ret;
}

void QMediaFifo::SetHashState(bool enable){
    this->AccessLock.lock();
    this->HashEnabled = enable;
    this->AccessLock.unlock();
}
void QMediaFifo::Flush(){
    std::list<fr_data_t>::iterator itor;
    this->AccessLock.lock();
    this->ShowSummary();
    printf("flush %s fifo item ->%d\n", this->Name, this->ListFifoItem.size());
    itor = this->ListFifoItem.begin();
    while(itor != this->ListFifoItem.end()){
        if(this->HashEnabled){
            //this->CheckHashItem((fr_data_t *)itor);
        }
        printf("flush %s fifo->%5d size:%8d %p\n", this->Name, itor->fr.serial, itor->fr.size, itor->fr.virt_addr);
        if(itor->fr.virt_addr != NULL){
            this->UsedMemSize -= itor->fr.size;
            free(itor->fr.virt_addr);
        }
        itor++;
    }
    this->ListFifoItem.clear();
    printf("fifo %s flush get item ->%d\n", this->Name, this->ListGetItem.size());
    itor = this->ListGetItem.begin();
    while(itor != this->ListGetItem.end()){
        if(this->HashEnabled){
            //this->CheckHashItem(itor);
        }
        printf("fifo %s flush get ->%5d size:%8d %p\n", this->Name, itor->fr.serial, itor->fr.size, itor->fr.virt_addr);
        if(itor->fr.virt_addr != NULL){
            this->UsedMemSize -= itor->fr.size;
            free(itor->fr.virt_addr);
        }
        itor++;
    }
    this->ListGetItem.clear();
    this->GetTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->PushCounter = 0;
    this->GetCounter = 0;
    this->PutCounter = 0;
    this->FullCounter = 0;
    printf("fifo %s flush ok\n", this->Name);
    this->ShowSummary();
    this->CurCacheTimeLen = 0;
    this->AccessLock.unlock();
}
int QMediaFifo::GetQueueSize(){
    int ret;
    this->AccessLock.lock();
    ret = this->ListFifoItem.size();
    this->AccessLock.unlock();
    return ret;
}
int QMediaFifo::GetCurCacheTimeLen(){
    int ret = 0;
    this->AccessLock.lock();
    ret = (int)this->CurCacheTimeLen;
    this->AccessLock.unlock();
    return ret;
}

void QMediaFifo::HashItem(fr_data_t *pFrd){
    //pFrd->hash = calCrc32(pFrd->fr.virt_addr, pFrd->fr.size) | pFrd->fr.serial;
    pFrd->hash = calCrc32((char *)pFrd->fr.virt_addr, pFrd->fr.size);
}
bool QMediaFifo::CheckHashItem(fr_data_t *pFrd){
    uint32_t hash = calCrc32((char *)pFrd->fr.virt_addr, pFrd->fr.size);
    if(hash != pFrd->hash){
        printf("fifo %s hash error ->%3d ts:%8lld size:%8d num:%5d\n",
                this->Name,
                pFrd->fr.serial,
                pFrd->fr.timestamp,
                pFrd->fr.size,
                this->ListFifoItem.size()
                );
    }
    else{
#if 0
        printf("hash ok ->%3d ts:%8lld size:%8d num:%5d\n",
                pFrd->fr.serial,
                pFrd->fr.timestamp,
                pFrd->fr.size,
                this->ListFifoItem.size()
                );
#endif

    }
    return true;
}
void QMediaFifo::ShowSummary(){
    printf("##########show %s fifo summary\n", this->Name);
    printf("fifo:%s ->mem%8d push:%d get:%d put:%d full:%d\n",
            this->Name,
            this->UsedMemSize,
            this->PushCounter,
            this->GetCounter,
            this->PutCounter,
            this->FullCounter
            );
}
void QMediaFifo::UpdateCurCacheLen(){
    fr_data_t in, out;
    in = this->ListFifoItem.back();
    out  = this->ListFifoItem.front();
    this->CurCacheTimeLen = out.fr.timestamp - in.fr.timestamp;
#if 0
    printf("%s ->cache time -> in:%8lld out:%8lld %8lld num:%3d\n",
            this->Name,
            in.fr.timestamp,
            out.fr.timestamp,
            this->CurCacheTimeLen,
            this->ListFifoItem.size()
            );
#endif
}

int QMediaFifo::GetDropFrameSize(){
    int ret = 0;
    this->AccessLock.lock();
    ret = this->FullCounter;
    this->AccessLock.unlock();
    return ret;
}

void QMediaFifo::MonitorDropFrames(bool enable)
{
    this->AccessLock.lock();
    this->MonitorFrame = enable;
    printf("fifo:%s ->MonitorDropFrames :%d\n",
            this->Name,
            this->MonitorFrame
            );
    this->AccessLock.unlock();
    return;
}

void QMediaFifo::SetKeepFramesTime(int s32Time)
{
    this->AccessLock.lock();
    if (s32Time < 0)
    {
        printf("time error: %d\n", s32Time);
        this->AccessLock.unlock();
        return;
    }
    this->u64KeepFrameTime = s32Time;
    printf("fifo:%s keep frames for %llu ms\n", this->Name, this->u64KeepFrameTime);
    this->AccessLock.unlock();
    return;
}

bool QMediaFifo::RestoreKeepFrames()
{
    this->AccessLock.lock();
    std::list<fr_data_t>::iterator itor;
    std::list<fr_data_t>::iterator stGetItor;
    bool bFoundItem = false;
    fr_data_t stFrontItem;

    memset(&stFrontItem, 0, sizeof(stFrontItem));
    if (ListFifoItem.size() > 0)
    {
        stFrontItem = ListFifoItem.front();
    }
    for (itor = this->ListKeepItem.begin();
         itor != this->ListKeepItem.end();
         itor++)
    {
        bFoundItem = false;
        for (stGetItor = this->ListGetItem.begin();
             stGetItor != this->ListGetItem.end();
             stGetItor++)
        {
            if (stGetItor->fr.virt_addr == itor->fr.virt_addr)
            {
                bFoundItem = true;
                this->ListGetItem.erase(stGetItor);
                break;
            }
        }
        if (!bFoundItem)
        {
            printf("fifo %s item not find in get list, %5d size:%8d %p priv:%8d time:%llu\n",
                this->Name,
                itor->fr.serial, itor->fr.size, itor->fr.virt_addr,
                itor->fr.priv,
                itor->fr.timestamp
                );
        }
        else
        {
            if ((stFrontItem.fr.buf == NULL) && (ListFifoItem.size() > 0))
            {
                stFrontItem = ListFifoItem.front();
            }
            if ((ListFifoItem.size() == 0) ||
                (stFrontItem.fr.timestamp <= itor->fr.timestamp + this->MaxCacheTime))
            {
                ListFifoItem.push_back(*itor);
            }
            else
            {
                this->ReleaseItem(&(itor->fr));
            }
        }
        #if 0
        printf("fifo %s restore ->%5d size:%8d %p priv:%8d time:%llu\n",
            this->Name,
            itor->fr.serial, itor->fr.size, itor->fr.virt_addr,
            itor->fr.priv,
            itor->fr.timestamp
            );
        #endif
    }
    printf("fifo %s after restore keep size:%d get size:%d\n",
            this->Name,
            this->ListKeepItem.size(),
            this->ListGetItem.size());
    if (this->ListKeepItem.size() != 0)
    {
        this->ListKeepItem.clear();
    }
    this->AccessLock.unlock();
    return true;
}

bool QMediaFifo::ShowList(EN_LIST_TYPE enType)
{
    std::list <fr_data_t>::iterator itor;
    std::list <fr_data_t> *pStList;
    char pu8ListName[50] = {0};

    if (enType == EN_LIST_TYPE_GET)
    {
        pStList = &this->ListGetItem;
        sprintf(pu8ListName, "%s", "GetList");
    }
    else if (enType == EN_LIST_TYPE_FIFO)
    {
        pStList = &this->ListFifoItem;
        sprintf(pu8ListName, "%s", "FifoList");
    }
    else if (enType == EN_LIST_TYPE_KEEP)
    {
        pStList = &this->ListKeepItem;
        sprintf(pu8ListName, "%s", "KeepList");
    }
    else
    {
        return false;
    }
    printf("fifo %s show %s(%d):\n",
        this->Name,
        pu8ListName,
        pStList->size());
    for (itor = pStList->begin();
         itor != pStList->end();
         itor++)
    {
        printf("fifo %s ->%5d size:%8d %p priv:%8d time:%llu\n",
            this->Name,
            itor->fr.serial, itor->fr.size, itor->fr.virt_addr,
            itor->fr.priv,
            itor->fr.timestamp
            );
    }
    printf("fifo %s show %s end\n",
        this->Name,
        pu8ListName);
    return true;
}