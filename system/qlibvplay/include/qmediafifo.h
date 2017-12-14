#include <mutex>
#include <fr/libfr.h>
#include <list>
#ifndef  __Q_MEDIA_FIFO_H__
#define  __Q_MEDIA_FIFO_H__
#define Q_MEDIA_FIFO_MAX_NAME  32

typedef enum {
    EN_LIST_TYPE_GET = 0,
    EN_LIST_TYPE_FIFO,
    EN_LIST_TYPE_KEEP,
    EN_LIST_TYPE_MAX,
} EN_LIST_TYPE;

typedef struct {
    struct fr_buf_info fr ;
    void *data;
    uint32_t hash ;
}fr_data_t;
class QMediaFifo{
    public:
        QMediaFifo(char *name);
        ~QMediaFifo();


        void SetMaxCacheTime(uint64_t t);
        uint64_t GetMaxCacheTime();
/*
        void SetMaxCacheNum(int32_t n);
        int32_t GetMaxCacheNum();
*/
        int PushItem(struct fr_buf_info *fr);
        int GetItem(struct fr_buf_info *fr);
        int PutItem(struct fr_buf_info *fr);

        void SetHashState(bool enable);
//        void ResetOutItem();
        void Flush();
        int GetQueueSize();
        void ShowSummary();
        int GetCurCacheTimeLen();
        int GetDropFrameSize();
        void MonitorDropFrames(bool enable);
        /*
        * @brief Set time of keep items in ListKeepItem to ListFifoItem for recorde again.
        * @param s32Time: cache time for keep items(second).
        * @return: void
        */
        void SetKeepFramesTime(int s32Time);

        /*
        * @brief Restore items in ListKeepItem to ListFifoItem for recorde again.
        * @return true: Success.
        * @return false: Failure.
        */
        bool RestoreKeepFrames();

        /*
        * @brief Show items in list.
        * @param enType: list type.
        * @return true: Success.
        * @return false: Failure.
        */
        bool ShowList(EN_LIST_TYPE enType);

    private:
        uint64_t PushTs;
        uint64_t GetTs;
        uint64_t MaxCacheTime;
        uint64_t CurCacheTimeLen;
        uint64_t u64KeepFrameTime;
        char Name[Q_MEDIA_FIFO_MAX_NAME];

        std::list <fr_data_t> ListGetItem;

        std::mutex AccessLock;
        std::list <fr_data_t> ListFifoItem;
        std::list <fr_data_t> ListKeepItem;
        bool HashEnabled;
        bool MonitorFrame;

        void HashItem(fr_data_t *pFrd);
        bool CheckHashItem(fr_data_t *pFrd);
        void UpdateCurCacheLen();

        int PushCounter;
        int GetCounter;
        int PutCounter;
        int FullCounter;

        int UsedMemSize;
        /*
        * @brief Return used buffer for release buffer.
        * @param fr: fr buffer info.
        * @return 0: Success.
        * @return -1: Failure.
        */
        int ReleaseItem(struct fr_buf_info *fr);

        /*
        * @brief check item in list.
        * @param pStList: item list.
        * @param stItem: item.
        * @return true: Item in list.
        * @return false: Item not in list.
        */
        bool CheckItemInList(std::list <fr_data_t> *pStList, fr_data_t stItem);

};
#endif
