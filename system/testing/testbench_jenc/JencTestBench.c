#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <getopt.h>
#include "basetype.h"
#include "JpegEncApi.h"
#include "ewl.h"
#include "JpegEncInstance.h"
#include "ewl_jpeg_enc.h"

#define OUT_SIZE 1000000
int main(int argc, char *argv[]) {
    int fd1, fd2, fd3, out_size;
    JpegEncInst encoder;
    JpegEncRet ret;
    int ret2;
    JpegEncIn encIn;
    JpegEncOut encOut;
    JpegEncCfg cfg;

    EWLLinearMem_t inLumMem;
    EWLLinearMem_t inUVMem;
    EWLLinearMem_t outMem;

    int opt;
    int width = 0, height = 0, stride = 0,format = 0, size = 0;
    char lum_file[64] = {0}, cbcr_file[64] = {0}, output[64] = {0};
    int err = 0;
    while ((opt = getopt(argc, argv, "w:h:l:c:o:f:s:m")) != -1) {
        switch(opt) {
            case 'm':
                printf("testbench_jenc usage:testbench_jenc -w width -h height -s stride -l lum_file -c cbcr_file -f format(\"nv12\" or \"nv21\") -o output\n");
                return 0;
            case 'w':
                width = atoi(optarg);
                printf("optarg:%s width:%d\n",optarg, width);
                break;
            case 'h':
                height = atoi(optarg);
                printf("optarg:%s height:%d\n",optarg, height);
                break;
            case 's':
                stride = atoi(optarg);
                printf("optarg:%s stride:%d\n",optarg, stride);
                break;
            case 'l':
                strncpy(lum_file, optarg, 64);
                printf("optarg:%s lum_file:%s\n",optarg, lum_file);
                break;
            case 'c':
                strncpy(cbcr_file, optarg, 64);
                printf("optarg:%s cbcr_file:%s\n",optarg, cbcr_file);
                break;
            case 'f':
                if(strcmp("nv12", optarg) == 0) format = 1;
                else if(strcmp("nv21", optarg) == 0) format = 0;
                else {
                       printf("format is wrong!\n");
                       err = -1;
                     }
                printf("optarg:%s format:%d\n",optarg, format);
                break;
            case 'o':
                strncpy(output, optarg, 64);
                printf("optarg:%s output:%s\n",optarg, output);
                break;
            default:
                return;
        }
    }

    printf("width:%d height:%d stride:%d lum_file:%s cbcr_file:%s format:%s output:%s\n", width, height, stride, lum_file, cbcr_file, (format==0?"nv12":"nv21"),output);
    if (err == -1 || width == 0 || height == 0|| stride == 0 || *lum_file == 0 || *cbcr_file == 0 || *output == 0) {
        printf("args set wrong,please input:testbench_jenc -m\n");
        return -1;
    }
    memset(&inLumMem, 0, sizeof(EWLLinearMem_t));
    memset(&inUVMem, 0, sizeof(EWLLinearMem_t));
    memset(&outMem, 0, sizeof(EWLLinearMem_t));
    cfg.inputWidth = width;
    cfg.inputHeight = height;
    cfg.stride = stride;
    cfg.frameType= format;
    cfg.unitsType = JPEGENC_NO_UNITS;
    ret = JpegEncInit(&cfg, &encoder);
    size = stride*height;

    if(ret != JPEGENC_OK) {
        printf("Jpeg Encoder init fail,ret:%d\n",ret);
        goto err;
    }

    ret2 = EWLMallocLinear(((jpegInstance_s * )encoder)->asic.ewl, size, &inLumMem);
    if (ret2 != EWL_OK)
    {
        fprintf(stderr, "Failed to allocate input picture!\n");
        inLumMem.virtualAddress = NULL;
        goto err;
    }

    printf("0x%x\n",inLumMem.busAddress);
    ret2 = EWLMallocLinear(((jpegInstance_s * )encoder)->asic.ewl, (size<<1), &inUVMem);
    if (ret2 != EWL_OK)
    {
        fprintf(stderr, "Failed to allocate input picture!\n");
        inUVMem.virtualAddress = NULL;
        goto err;
    }

    printf("0x%x\n",inUVMem.busAddress);

    ret2 = EWLMallocLinear(((jpegInstance_s *)encoder)->asic.ewl, OUT_SIZE, &outMem);
    if (ret2 != EWL_OK)
    {
        fprintf(stderr, "Failed to allocate output picture!\n");
        outMem.virtualAddress = NULL;
        goto err;
    }

    printf("0x%x\n", outMem.busAddress);

    fd1 = open(lum_file, O_RDONLY);
    if(fd1 < 0) {
        printf("open file 1.load_mem.bin fail\n");
        goto err;
    }
    if (read(fd1, inLumMem.virtualAddress, size) < 0) {
        printf("Jenc read file1 err\n");
        goto err;
    }
    close(fd1);

    fd2 = open(cbcr_file,O_RDONLY);
    if(fd2 < 0) {
        printf("open file 2.load_mem.bin fail\n");
        goto err;
    }
    if (read(fd2, inUVMem.virtualAddress, size<<1) < 0) {
        printf("Jenc read file2 err\n");
        goto err;
    }
    close(fd2);

    out_size = 0;

    encIn.busLum = inLumMem.busAddress;
    encIn.busCb = inUVMem.busAddress;
    encIn.busOutBuf = outMem.busAddress;
    encIn.pOutBuf = outMem.virtualAddress;
    encIn.outBufSize = OUT_SIZE;
    memset(encIn.pOutBuf, 0, encIn.outBufSize);

    printf("test encIn.busLum:0x%x encIn.busCb:0x%x encIn.busOutBuf:0x%x encIn.busOutBuf_end:0x%x\n", encIn.busLum, encIn.busCb, encIn.busOutBuf, encIn.busOutBuf + encIn.outBufSize);

    ret = JpegEncSetPictureSize(encoder, &cfg);
    if(ret != JPEGENC_OK) {
        printf("JpegEncSetPictureSize:err, ret:%d !\n",ret);
        goto err;
    }

    encOut.jfifSize = 0;
    printf("Start JpegEncEncode\n");
    ret = JpegEncEncode(encoder, &encIn, &encOut);
    if(ret != JPEGENC_FRAME_READY) {
        printf("JpegEncEncode:err, ret:%d !\n",ret);
        goto err;
    }
    printf("End JpegEncEncode\n\n");
    fd3 = open(output, O_CREAT|O_WRONLY, S_IRUSR | S_IROTH);
    if (fd3 < 0) {
        printf("open file err: %d\n", fd3);
        goto err;
    }

    if (write(fd3, outMem.virtualAddress, ((jpegInstance_s * )encoder)->stream.byteCnt) < 0) {//encOut.jfifSize
        printf("Jenc write file err\n");
        goto err;
    }
    close(fd3);
    printf("encOut.jfifSize:%d   program_size:%d\n",encOut.jfifSize, 0x8f49-0x1000);
    printf("exif jenc testbench\n");
    EWLFreeLinear(encoder, &inLumMem);
    EWLFreeLinear(encoder, &inUVMem);
    EWLFreeLinear(encoder, &outMem);
    JpegEncRelease(encoder);
    return 0;
  err:
    if(fd1 > 0) close(fd1);
    if(fd2 > 0) close(fd2);
    if(fd3 > 0) close(fd3);
    if(inLumMem.virtualAddress != NULL) EWLFreeLinear(encoder, &inLumMem);
    if(inUVMem.virtualAddress != NULL) EWLFreeLinear(encoder, &inLumMem);
    if(outMem.virtualAddress != NULL) EWLFreeLinear(encoder, &outMem);
    JpegEncRelease(encoder);
    return -1;
}
