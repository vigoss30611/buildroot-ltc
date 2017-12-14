#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <jpegexif.h>
#include <jpegutils.h>

static void test_set_exif(char *jpeg_file, char *buf, char *size);
static void test_get_exif(char *jpeg_file);
static void test_jpegbuf_set_exif(char *path);
static int test_jpeg_sw_encode(char *yuv_file, int width, int height, int format);


int main(int argc, char *argv[]) {

    int opt;
    char *seg1,*seg2,*seg3,*seg4;

    while ((opt = getopt(argc, argv, "s:g:hb:e:")) != -1) {
        switch(opt) {
            case 'h':
                printf("exiftest usage:\n"
                       "set exif value:\n"
                       "\texiftest -s jpegfile value length\n"
                       "get exif value:\n"
                       "\texiftest -g jpegfile\n"
                       "exiftest command help:\n"
                       "\texiftest -h\n");
                break;
            case 's':
                seg1=optarg;
                seg2=argv[optind];
                seg3=argv[optind+1];
                printf("seg1:%s seg2:%s seg3:%s\n",seg1,seg2,seg3);
                test_set_exif(seg1, seg2, seg3);
                break;
            case 'g':
                printf("optarg:%s\n",optarg);
                test_get_exif(optarg);
                break;
            case 'b':
                printf("optarg:%s \n",optarg);
                test_jpegbuf_set_exif(optarg);
                break;
            case 'e':
                printf("test software jpeg encode from YUV420P data, yuvFile:%s width:%d height:%d\n", optarg, atoi(argv[optind]), atoi(argv[optind + 1]));
                seg1=optarg;
                seg2=argv[optind];
                seg3=argv[optind+1];
                seg4=argv[optind+2];
                test_jpeg_sw_encode(seg1, atoi(seg2), atoi(seg3), atoi(seg4));
                break;
            default:
                break;
        }

    }
    return 0;
}

static int test_jpeg_sw_encode(char *yuv_file, int width, int height, int format) {

    FILE *file = NULL;
    char *data = NULL;
    int size = 0;
    int lenth = 0;

    if (yuv_file == NULL || width <= 0 || height <= 0) {
        printf("%s(%s),Error:args is wrong!\n",__FILE__, __LINE__);
        return -1;
    }
    printf("%s,yuv_file:%s width:%d height:%d\n",__FILE__, yuv_file, width, height);
    file = fopen(yuv_file, "rb");

    if (file == NULL) {
        printf("%s(%s),Error:Open file %s fail!\n",__FILE__, __LINE__, yuv_file);
        return -1;
    }

    size = width*height*3/2;

    data = (char *)malloc(size);
    if (data == NULL) {
        printf("%s(%s),Error:Malloc mem fail!\n",__FILE__, __LINE__);
        fclose(file);
        file = NULL;
        return -1;
    }
    fread(data, 1, size, file);
    printf("fread data success, size:%d\n", size);

    fseek(file, 0, SEEK_END);
    lenth = ftell(file);

    if (lenth == size) {
        swenc_yuv2jpg(data, width, height, format, "/nfs/sw_en_pic.jpg");//12:yuv420p  25:nv12
    } else {
        printf("%s(%d)Picture file size is not right!lenth:%d size:%d\n",__FILE__, __LINE__, lenth, size);
        free(data);
        data = NULL;
        fclose(file);
        file = NULL;
        return -1;
    }

   /* if(data) {
        printf("1\n");
        free(data);
        data = NULL;
    }*/

    if(file) {
        fclose(file);
        file = NULL;
    }
    return 0;
}

static void test_set_exif(char *jpeg_file, char *buf, char *size) {
       jpeg_set_exif(jpeg_file, buf, atoi(size));
}

static void test_get_exif(char *jpeg_file) {
    char buf[1024];
    int len = sizeof(buf);
    char *ptr = NULL;
    int ret = 0;

    ret = jpeg_get_exif(jpeg_file,buf,len);

    switch (ret) {
        case RT_NOTENOUGH_MEM:
            printf("RT_NOTENOUGH_MEM\n");
            break;
        case RT_NOTEXIF_FORMAT:
            printf("RT_NOTEXIF_FORMAT\n");
            break;
        case RT_NOTSETTAG_VALUE:
            printf("RT_NOTSETTAG_VALUE\n");
            break;
        case RT_ERR_ARGS:
            printf("RT_ERR_ARGS\n");
            break;
        default:
            break;
    }

    if (ret < 0) {
        return;
    }

    printf("\nJpeg Exif %d Bytes Value:\n",ret);
    for (ptr = buf;ptr < (buf + ret);ptr++) {
        printf("%x ",*ptr);
    }
    printf("\n\n");
}

static void test_jpegbuf_set_exif(char *path) {
    printf("start test_jpegbuf_set_exif arg:%s\n",path);
    FILE *fd = fopen(path, "rb");
    int file_len = 0;
    char *mem = NULL;

    if (fd == NULL) {
        printf("error:open %s fail\n", path);
        return;
    }

    fseek(fd, 0, SEEK_END);
    file_len = ftell(fd);
    printf("file_len:%d\n",file_len);
    rewind(fd);
    mem = (char *)malloc(file_len);
    fread(mem, 1, file_len, fd);
    fclose(fd);

    printf("start set exif from mem file_len:%d!\n", file_len);
    jpegbuf_set_exif(mem, file_len, "/nfs/zz.log", 0, "/nfs/Infotm.jpg");
    free(mem);
}

