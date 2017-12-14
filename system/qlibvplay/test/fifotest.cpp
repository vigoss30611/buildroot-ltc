#include <qmediafifo.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <define.h>


#define BUF_NORNMA_SIZE  (0x1000 *500)  //20k  //0x1000 = 4k
#define BUF_MAX_SIZE  (0x1000  *2000)   //40k
#define BUF_NUM  30
#define BUF_UNIT_SIZE  1


int getRandom(int min ,int max){
    srand(( unsigned ) time(NULL)) ;
    return rand()%max+ +min;
}

static int totalTime  =  100000;
static int pushTime    =  90000 ;
int GetUsleepTime( int type ){
    static int counter = 0;
    counter++;
    if(counter > 1000){
        counter = 0 ;
//        pushTime= totalTime-pushTime ;
        pushTime = getRandom(100,totalTime);
        printf("!!!!!!!!!!!!!!!!!!update sleep time -> w:%d r:%d\n",pushTime,totalTime-pushTime);
    }
    if(type == 1) {//read thread  x{
        return totalTime- pushTime ;
    }
    else {
        return pushTime ;
    }
}
void randomFillBuf(char *buf,int size ){
    srand(( unsigned ) time(NULL) +size ) ;
    int start = rand()%size ;
    for(int i =0 ;i<size ;i++){
        buf[i] =  start++;
    }
}

void fillFr(struct fr_buf_info *fr ,int size){
    randomFillBuf((char *)fr->virt_addr, size);
    fr->size = size ;
//    fr->priv = size -1 ;
    fr->timestamp = size +1;

}
void *writeFrFifo(void *arg){
    QMediaFifo  *fifo = (QMediaFifo *)arg ;
    printf("write -> %d \n",fifo->GetQueueSize());
    int size  = 0;
    struct fr_buf_info fr = {0};
    char *temp = (char *)malloc(BUF_MAX_SIZE);
    if(temp == NULL){
        printf("malloc basic error\n");
        return NULL;
    }
    fr.virt_addr = temp ;
    int counter  = 0;
    while(1){
        size = getRandom(1 , BUF_NORNMA_SIZE);
    //    printf("randon size ->%dk\n",size/1024);
        fillFr(&fr,size);
        fr.size = size ;
        counter++;
        fr.priv  = counter ;
        fr.timestamp = get_timestamp();
        if(counter% 100  == 0){
            printf("write -> %6d  %d  %d %d\n",counter ,fifo->GetQueueSize(),pushTime,totalTime-pushTime);
            printf("wait for user consume it\n");
          //  sleep(3);
            fifo->Flush();
            printf(" !!!!!!!!!!!!!!flush begin\n");
            sleep(4);
            printf("&&&&&&&&&&&&&flush  over\n");
            sleep(2);
        }
        if(fifo->PushItem(&fr)< 0){
            printf("put fifo in error \n");
            continue ;
        }
        usleep(GetUsleepTime(0));
//        sleep(1);

    }
    return NULL ;
}
void *readFrFifo(void *arg){
    QMediaFifo  *fifo = (QMediaFifo *)arg ;
    sleep(3);
    printf("read start -> %d \n",fifo->GetQueueSize());
//    int size  = 0;
    struct fr_buf_info fr ;
    int counter  = 0;
    int ret   = 0;
    int interval = getRandom(100,1000);
    while(1){
        counter++;
        ret = fifo->GetItem(&fr) ;
        if(ret == 0 ){
            //printf("no item in fifo \n");
        }
        else if(ret > 0 ){
            //printf("get buf ok ->size:%8d priv:%5d  queue:%3d\n",fr.size,fr.priv,fifo->GetQueueSize());
            fifo->PutItem(&fr);
            if(( counter %40  )==  0){
                printf("pause and wait for full\n");
            }
        }
        else {
            printf("get buf error \n");
            break;
        }
        if(counter % interval  == 0){
            interval = getRandom(20,300);
            counter = 1;
            printf("update io block interval ->%d\n", interval);
            fifo->ShowSummary();
            sleep(4);
            //printf("fifo status ->%d\n", fifo->GetQueueSize());
        }
        usleep(GetUsleepTime(1));
    }
    return NULL;
}

int main(int argc, char **argv){
    printf("begin test\n");
    pthread_t frInThread;
    char name[] = "test";
    QMediaFifo *fifo =  new QMediaFifo(name);
    fifo->SetMaxCacheTime(100);
    sleep(1);
    fifo->SetHashState(true);

    pthread_create(&frInThread ,NULL,writeFrFifo ,fifo);
//    sleep(5);
#if 1
    pthread_t frOutThread  ;
    pthread_create(&frOutThread,NULL,readFrFifo ,fifo);
    pthread_join(frOutThread ,NULL);
#endif
    pthread_join(frInThread ,NULL);
    fifo->Flush();
    return 0;
}
