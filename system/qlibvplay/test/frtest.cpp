//#include <QCoreApplication>
#include <frfifoloop.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define BUF_NORNMA_SIZE  (0x1000 *5)  //20k  //0x1000 = 4k
#define BUF_MAX_SIZE  (0x1000  *30)   //40k
#define BUF_NUM  30
#define BUF_UNIT_SIZE  1


int getRandom(int min ,int max){
    srand(( unsigned ) time(NULL)) ;
    return rand()%max+ +min;
}

static int totalTime  =  200000;
static int pushTime    = 150000 ;
int GetUsleepTime( int type ){
    static int counter = 0;
    counter++;
    if(counter > 100){
        counter = 0 ;
//        pushTime= totalTime-pushTime ;
//        pushTime = getRandom(0,totalTime);
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
    randomFillBuf((char *)fr->virt_addr,size );
    fr->size = size ;
//    fr->priv = size -1 ;
    fr->timestamp = size +1;

}
void *writeFrFifo(void *arg){
    FrFifoLoop  *fifo = (FrFifoLoop *)arg ;
    printf("write -> %d \n",fifo->GetQueueSize());
    int size  = 0;
    struct fr_buf_info fr ;
    char *temp = (char *)malloc(BUF_MAX_SIZE);
    if(temp == NULL){
        printf("malloc basic error\n");
        return NULL;
    }
    fr.virt_addr = temp ;
    int counter  = 0;
    while(1){
        size = getRandom(BUF_NORNMA_SIZE/2 , BUF_NORNMA_SIZE*2/3);
    //    printf("randon size ->%dk\n",size/1024);
        fillFr(&fr,size);
        fr.size = size ;
        counter++;
        fr.priv  = counter ;
        if(counter% 100  == 0){
            printf("write -> %6d  %d  %d %d\n",counter ,fifo->GetQueueSize(),pushTime,totalTime-pushTime);
        }
        if(fifo->Push(fr)< 0){
            printf("put fifo in error \n");
            continue ;
        }
        usleep(GetUsleepTime(0));
//        sleep(1);

    }
    return NULL ;
}
void *readFrFifo(void *arg){
    FrFifoLoop  *fifo = (FrFifoLoop *)arg ;
    printf("read start -> %d \n",fifo->GetQueueSize());
//    int size  = 0;
    struct fr_buf_info fr ;
    int counter  = 0;
    char *temp = (char *)malloc(BUF_MAX_SIZE);
    if(temp == NULL){
        printf("malloc basic error\n");
        return NULL;
    }
    fr.virt_addr = temp ;
    int ret   = 0;
    while(1){
        counter++;
        ret = fifo->Pop(&fr) ;
        if(ret == 0 ){
            //printf("can not find \n");
        }
        else if(ret > 0 ){
            printf("get buf ok ->size:%6d priv:%5d  queue:%3d\n",fr.size,fr.priv,fifo->GetQueueSize());
            if(( counter %40  )==  0){
                printf("pause and wait for full\n");
                sleep(5);
            }
        }
        else {
            printf("get buf error \n");
            break;
        }
        if(counter % 10  == 0){
            //printf("\t\t read -> %d \n",fifo->GetQueueSize());
        }
        usleep(GetUsleepTime(1));
    }
    return NULL;
}

extern uint32_t calCrc32(  char *buf, uint32_t size);
int main(int argc ,char *argv[]){
    pthread_t frInThread;
                           //FrFifoLoop(int bufNum ,int bufSize ,int maxBufSize,int sizeRate ,int unitKbSize  ,const char *name) {

    char buf[]= "123456";
    buf[5] = 0x0a;
    printf("crc32 ->%X\n",calCrc32(buf,6));
    FrFifoLoop *frLoop = new FrFifoLoop(BUF_NUM ,BUF_NORNMA_SIZE,BUF_MAX_SIZE , 100,BUF_UNIT_SIZE , "frtest" );
    frLoop->SetHashState(true);
    if( frLoop->Prepare() == false ){
        printf("frloop prepare error\n");
        exit(1);
    }
    else {
        printf("fifo prepare ok\n");
    }

    pthread_create(&frInThread ,NULL,writeFrFifo ,frLoop);
//    sleep(5);
#if 1
    pthread_t frOutThread  ;
    pthread_create(&frOutThread,NULL,readFrFifo ,frLoop);
    pthread_join(frOutThread ,NULL);
#endif
    pthread_join(frInThread ,NULL);
    return 0;
}
#if 0
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
#endif
