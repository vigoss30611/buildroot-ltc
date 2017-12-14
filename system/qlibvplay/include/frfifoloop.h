#include <queue>
#include <mutex>
#include <fr/libfr.h>
#ifndef  __FR_FIFO_LOOP_H__
#define  __FR_FIFO_LOOP_H__
#define FRFIFOLOOP_MAX_NAME   32
typedef struct {
    struct fr_buf_info fr ;
    int32_t *unitIndexList ;
    uint32_t hash ;
}fr_data_t;
class FrFifoLoop{
    public :
        FrFifoLoop(int bufNum ,int bufSize ,int maxBufSize,int sizeRate/*percent  */ ,int unitSize  ,const char *name) ;
    //    FrFifoLoop(int msecond ,int bufSize);
        ~FrFifoLoop();


        int GetQueueSize(void);

        int GetQueueFreeSize(void);



#if 0
        struct fr_buf_info  GetFifoInFr(void);
        bool PutFifoInFr(struct fr_buf_info fr);

        struct fr_buf_info  GetFifoOutFr(void);
        bool PutFifoOutFr(struct fr_buf_info fr);
#endif
        bool Prepare(void);


        int Push(struct fr_buf_info fr );
        int Pop( struct fr_buf_info *fr );
        int Flush(void);
        void SetHashState(bool enabled);

        int GetFreeUnitNum(void);
        void SetCacheTime(uint64_t msecond);
        uint64_t GetCacheTime(void);
    private :
        int frNum ;
        struct fr_buf_info *frBufList;
        std::queue<size_t>  fifoFrameQueue;
        std::queue<size_t>  freeFrameQueue;
        std::mutex  queueLock ;
        size_t  *busyFrameList;

        uint64_t CacheTimeMsSecond;

        uint64_t PopFrTimeStamp;


        struct fr_info FrInst ;
        struct fr_buf_info FrInBuf ;
        struct fr_buf_info FrOutBuf ;
        char Name[FRFIFOLOOP_MAX_NAME];
        int MaxBufNum ;
        int UsedBufNum ;

        int32_t  BufSize ;
        int32_t  bufSize ;
        int32_t  MaxBufSize ;
        int32_t  TotalBufSize ;
        int curFrNum;
        int CurFrNum;
    //    int CurrentSize ;


        int UnitSize ;
        int UnitNumPerData ;
        int TotalUnitNum ;
        int BusyUnitNum ;
        int32_t *UnitIndexList ;

        bool HashEnabled ;

        int PushUnitIndex ; //for quick search
    //    int PopUnitIndex ;  //
        int PushDataIndex ;
        int PopDataIndex ;

        std::mutex  QueueLock ;

        uint8_t  *UnitStateList ;
        fr_data_t  *FrDataList ;
        char *FrDataBuf ;

        int PushFr(struct fr_buf_info *fr ,fr_data_t  *fd) ;
        int PopFr(struct fr_buf_info *fr ,fr_data_t  *fd) ;
        int PopNullFr(fr_data_t  *fd) ;

        FILE *PushFd ;
        FILE *PopFd ;

        int PushCounter;
        int PopCounter;
        int PopNullCounter;

        int IncCounter;


};
#endif


