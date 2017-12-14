#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "jpegexif.h"
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>


#define FILE_BYTE_ORDER EXIF_BYTE_ORDER_INTEL
#define ASCII_COMMENT "ASCII\0\0\0"
#define FILE_ERROR_CHECK(f) if (ferror(f)) { \
                                goto err; \
                            }

typedef struct _ExifConfig {
     int tag;
     char *value;
     int format;
     unsigned int len;
     unsigned int components;
     int flag;//0:config from memory 1:config from file
}ExifConfig;

static const unsigned char exif_header[] = {
    0xff, 0xd8, 0xff, 0xe1
};

void jpeg_set_exif(char *jpeg_file, char *buf, int size);
int jpeg_get_exif(char *jpeg_file,char *buf, int size);

static const unsigned int exif_header_len = sizeof(exif_header);
static void api_set_exif_raw(char *file_name, char *exif_data, int len);



static ExifEntry *create_entry(ExifData *exif, ExifIfd ifd, ExifTag tag, ExifFormat format, unsigned long components, size_t len) {
    void *buf;
    ExifEntry *exifEntry;
    ExifMem *exifMem;

    exifMem = exif_mem_new_default();
    assert(exifMem != NULL);

    exifEntry = exif_entry_new_mem(exifMem);
    assert(exifEntry != NULL);

    buf = exif_mem_alloc(exifMem, len);
    assert(buf != NULL);

    exifEntry->data      = buf;
    exifEntry->size      = len;
    exifEntry->tag       = tag;
    exifEntry->components = components;
    exifEntry->format    = format;

    //attach the ExifEntry to an ExifContent (IFD)
    exif_content_add_entry(exif->ifd[ifd], exifEntry);

    exif_mem_unref(exifMem);

    return exifEntry;
}


static ExifEntry *remove_entry_bytag(ExifData *exif, ExifIfd ifd, ExifTag tag) {
    ExifEntry *entry = exif_content_get_entry(exif->ifd[ifd], tag);
    if (entry != NULL) {
        exif_content_remove_entry(exif->ifd[ifd], entry);
    }
    return entry;
}

static void set_exif_value_bytag(ExifData *exif, ExifIfd ifd, ExifTag tag, signed char *value, ExifFormat format, unsigned int len, unsigned long components, int flag) {
    ExifEntry *entry = NULL;
    unsigned long size = 0;
    int format_size = 0;
    int comment_len = 0;
    FILE *fd = NULL;
    char buffer[1024];
    char *pentry = NULL;

#if 1
    entry = exif_content_get_entry(exif->ifd[ifd], tag);
    format_size = exif_format_get_size(format);
    size = len*components*format_size;

    if (entry == NULL) {
        entry = create_entry(exif, ifd, tag, format, components, size);
    } else {
        remove_entry_bytag(exif, ifd, tag);
        entry = create_entry(exif, ifd, tag, format, components, size);
    }

    if (entry == NULL) {
        printf("Error:create entry fail!\n");
        return;
    }

    memset(entry->data, 0, size);

    switch (tag) {
            case EXIF_TAG_USER_COMMENT:
                comment_len = sizeof(ASCII_COMMENT)-1;
                memcpy(entry->data, ASCII_COMMENT, comment_len);
                if (flag) {

                    fd = fopen(value, "rb");
                    if (fd == NULL) {
                        printf("Error:open %s fail!\n",value);
                        break;
                    }

                    pentry = entry->data + comment_len;

                    while(!feof(fd)) {
                        size = fread(buffer, 1, 1024,fd);
                        memcpy(pentry, buffer, size);
                        pentry += size;
                    }

                    fclose(fd);
                } else {
                    memcpy(entry->data + comment_len, value, size - comment_len);
                }
                break;
            default:
                break;
        }

    exif_entry_unref(entry);
#endif
}

static void api_set_exif(char *file_name, ExifConfig *exif_config, int counts) {

    unsigned char *exif_data = NULL;
    unsigned int exif_data_len = 0;
    ExifLoader *loader = NULL;
    ExifData *exif = NULL;
    int i;
    if (file_name == NULL || (exif_config !=NULL && counts < 1)) {
        return;
    }

    loader = exif_loader_new();
    if (loader) {
        exif_loader_write_file(loader, file_name);
        exif = exif_loader_get_data(loader);
        exif_loader_unref(loader);

    }

    if (exif == NULL) {
        exif = exif_data_new();
        if (!exif) {
            printf("Error:Out of memory,alloc exifdata fail!\n");
            assert(1);
        }

        exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
        exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
        exif_data_set_byte_order(exif, FILE_BYTE_ORDER);

        //initial default exifdata
        exif_data_fix(exif);
    }


#if 1
    if (exif_config != NULL) {
        for (i=0;i<counts;i++) {
            set_exif_value_bytag(exif, EXIF_IFD_EXIF, exif_config[i].tag, exif_config[i].value, exif_config[i].format, exif_config[i].len, exif_config[i].components, exif_config[i].flag);
        }
    }

#endif
    exif_data_save_data(exif, &exif_data, &exif_data_len);

    //save exif to image
    api_set_exif_raw(file_name, exif_data, exif_data_len);
    free(exif_data);
    exif_data_unref(exif);
}


//for write exif block data to jpg
static void api_set_exif_raw(char *file_name, char *exif_data, int len) {
    FILE *fd_temp = NULL;
    FILE *fd = NULL;

    int buffer[1024];
    int size = 0;

    //dest temp file
    char *FILE_NAME = "/tmp/exif_pic_raw.jpg";
    int offset = 0;
    int word = 0;

    if (file_name == NULL || exif_data == NULL || len < 0) {
        return;
    }

    fd_temp = fopen(FILE_NAME, "wb+");
    if (fd_temp == NULL) {
        printf("Error:create %s failed!\n", FILE_NAME);
        goto err;
    }
#if 1
    //write exif header
    if (fwrite(exif_header, exif_header_len,1 ,fd_temp) != 1) {
        printf("Error:writing %s failed!\n",FILE_NAME);
        goto err;
    }

    //write EXIF data length in big-endian order,2 bytes for memoring length
    if (fputc((len + 2) >> 8, fd_temp) < 0) {
        printf("Error:writing %s EXIF data failed!\n",FILE_NAME);
        goto err;
    }
    if (fputc((len + 2) & 0xff, fd_temp) < 0) {
        printf("Error:writing %s EXIF data failed!\n",FILE_NAME);
        goto err;
    }

    //write EXIF data block
    if (fwrite(exif_data, len, 1, fd_temp) != 1) {
        printf("Error:writing %s failed!\n",FILE_NAME);
        goto err;
    }
#endif

#if 1
    //write JPEG image data
    fd = fopen(file_name,"r+");

    if (fd == NULL) {
        printf("Error:open %s failed!\n", file_name);
        goto err;
    }

    //skip orignal JPEG's JFIF or EXIF header
    do {
        word = fgetc(fd);
        FILE_ERROR_CHECK(fd);

        if (feof(fd)) break;

        offset++;
        if ((word & 0xff) == 0xff) {
            word = fgetc(fd);
            FILE_ERROR_CHECK(fd);

            if (feof(fd)) break;
            offset++;

            if ((word & 0xff) == 0xe0 || (word & 0xff) == 0xe1) {
               int m , n;
               m = fgetc(fd);
               FILE_ERROR_CHECK(fd);

               n = fgetc(fd);
               FILE_ERROR_CHECK(fd);

               offset = offset + (m >> 8) + n;
               break;
            }

        }
    }while(!feof(fd));

    rewind(fd);
    FILE_ERROR_CHECK(fd);

    fseek(fd, offset, SEEK_SET);
    FILE_ERROR_CHECK(fd);

    while (!feof(fd)) {
        size = fread(buffer, 1, 1024,fd);
        FILE_ERROR_CHECK(fd);

        fwrite(buffer, 1, size, fd_temp);
        FILE_ERROR_CHECK(fd_temp);
    }
#endif

    if (fd) {
        fclose(fd);
        fd = NULL;
        remove(file_name);
        //rename(FILE_NAME,file_name); //two dir will cause rename fail?

        fd = fopen(file_name,"wb+");
        rewind(fd_temp);

        while(!feof(fd_temp)) {
            size = fread(buffer, 1, 1024,fd_temp);
            FILE_ERROR_CHECK(fd_temp);
            fwrite(buffer, 1, size, fd);
            FILE_ERROR_CHECK(fd);
        }

        fclose(fd);
    }


    if (fd_temp) {
        fclose(fd_temp);
        remove(FILE_NAME);
        fd_temp = NULL;
    }

    return;

 err:
    if (fd_temp) {
        fclose(fd_temp);
        remove(FILE_NAME);
        fd_temp = NULL;
    }

    if (fd) {
        fclose(fd);
        fd = NULL;
    }

}

/*
  in: exif_config
  out: exif_data  need free by user
       exif_data_len
*/
static int generate_exif_data(ExifConfig *exif_config, unsigned char **exif_data, unsigned int *exif_data_len) {
    ExifData *exif;

    if (exif_config == NULL) {
        printf("%s(%s): error:params is wrong!\n",__FILE__, __LINE__);
        return 0;
    }
    *exif_data = NULL;
    *exif_data_len = 0;
    exif = exif_data_new();
    if (!exif) {
        printf("Error:Out of memory,alloc exifdata fail!\n");
        assert(1);
    }

    exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exif, FILE_BYTE_ORDER);

    //initial default exifdata
    exif_data_fix(exif);

    set_exif_value_bytag(exif, EXIF_IFD_EXIF, exif_config->tag, exif_config->value, exif_config->format, exif_config->len, exif_config->components, exif_config->flag);
    exif_data_save_data(exif, exif_data, exif_data_len);
    exif_data_unref(exif);
    return 1;
}

static int jpegbuf_picdata_offset(char *buf, int buf_len, int *offset) {
    int m, n;
    int tem_offset;
    int ret = 0;
    if (buf == NULL || buf_len <= 0 || offset == NULL) {
        printf("%s(%s): error:params is wrong!\n",__FILE__, __LINE__);
        return ret;
    }

    m = n = tem_offset = 0;
    for (char *ptr=buf;ptr<(buf + buf_len);ptr++) {
        if (((ptr - buf) <  (buf_len - 2)) && *ptr == 0xff && (*(ptr + 1) == 0xe0 || *(ptr + 1) == 0xe1) ){
            tem_offset += 2;
            m = *(ptr + 2);
            n = *(ptr + 3);
            tem_offset = tem_offset + (m >> 8) + n;
            ret = 1;
            break;
        } else {
            tem_offset++;
        }
    }

    *offset = tem_offset;
    return ret;
}
int jpegbuf_set_exif(char *jpeg_buf, int jpeg_buf_len, char *buf, int size, char *jpeg_file) {
    ExifConfig config;
    unsigned char *exif_data = NULL;
    unsigned int exif_data_len = 0;
    int pic_offset = 0;
    int pic_raw_data_len = 0;
    FILE *fd = NULL;

    if (jpeg_buf == NULL || jpeg_buf_len <= 0 || buf == NULL || size < 0 || jpeg_file == NULL) {
        printf("%s(%s): error:params is wrong!\n",__FILE__, __LINE__);
        return 0;
    }

    if (size == 0) {

        config.flag = 1;
        if (buf == NULL) {
            printf("Error:config file is null!\n");
            return 0;
        }
        fd = fopen(buf, "rb");

        if (fd == NULL) {
            printf("Error:open %s failed!\n",buf);
            goto err;
        }

        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        rewind(fd);
        fclose(fd);
    } else {
        config.flag = 0;
    }

    //generate exif data
    config.tag = EXIF_TAG_USER_COMMENT;
    config.format = EXIF_FORMAT_BYTE;
    config.len = 1;
    config.components = sizeof(ASCII_COMMENT) - 1 + size;
    config.value = buf;

    if (!generate_exif_data(&config, &exif_data, &exif_data_len)) {
        printf("%s(%s): error: generate_exif_data fail!\n",__FILE__, __LINE__);
        goto err;
    }

    //find picture data offset in jpeg_buf
    if (!jpegbuf_picdata_offset(jpeg_buf, jpeg_buf_len, &pic_offset)) {
        printf("%s(%s): error: jpegbuf_picdata_offset fail!\n",__FILE__, __LINE__);
        goto err;
    }

    //generate jpeg_file
    fd = fopen(jpeg_file, "wb+");
    if (fd == NULL) {
        printf("%s(%s): error: create %s fail!\n",__FILE__, __LINE__, jpeg_file);
        goto err;
    }

    fwrite(exif_header, exif_header_len,1 ,fd);
    fputc((exif_data_len + 2) >> 8, fd);
    fputc((exif_data_len + 2) & 0xff, fd);

    fwrite(exif_data, 1, exif_data_len, fd);
    fwrite(jpeg_buf + pic_offset, 1, jpeg_buf_len - pic_offset, fd);

    fclose(fd);
    free(exif_data);
    return 1;
err:
    free(exif_data);
    return 0;
}
void jpeg_set_exif(char *jpeg_file, char *buf, int size) {
    ExifConfig config;
    FILE *fd = NULL;
    if (jpeg_file == NULL || size < 0) {
       printf("Error:ags is wrong!\n");
       return;
    }

    if (size == 0) {

        config.flag = 1;

        if (buf == NULL) {
            printf("Error:config file is null!\n");
            return;
        }

        fd = fopen(buf, "rb");

        if (fd == NULL) {
            printf("Error:open %s failed!\n",buf);
        }

        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        rewind(fd);
        fclose(fd);
        printf("config file size:%d\n",size);
    } else {
        config.flag = 0;
    }

    config.tag = EXIF_TAG_USER_COMMENT;
    config.format = EXIF_FORMAT_BYTE;
    config.len = 1;
    config.components = sizeof(ASCII_COMMENT) - 1 + size;
    config.value = buf;
    api_set_exif(jpeg_file, &config, 1);
}

int jpeg_get_exif(char *jpeg_file, char *buf, int size) {
     ExifLoader *exif_loader = NULL;
     ExifData *exif_data = NULL;
     ExifEntry *exif_entry = NULL;

     if (jpeg_file == NULL || buf == NULL || size < 0) {
         return RT_ERR_ARGS;
     }

     exif_data = exif_data_new_from_file(jpeg_file);
     if (exif_data == NULL) {
         *buf = '\0';
         return RT_NOTEXIF_FORMAT;
     }

     exif_entry = exif_content_get_entry(exif_data->ifd[EXIF_IFD_EXIF], EXIF_TAG_USER_COMMENT);
//additional 1 byte space used for '\0
     if (size <= (exif_entry->components - (sizeof(ASCII_COMMENT) -1))) {
         *buf = '\0';
         return RT_NOTENOUGH_MEM;
     }

     if (exif_entry) {
         exif_entry_get_value(exif_entry, buf, size);
         return exif_entry->components - (sizeof(ASCII_COMMENT) -1);
     } else {
         return RT_NOTSETTAG_VALUE;
     }
}

int jpegbuf_set_exif2mem(char *jpeg_buf, int jpeg_buf_len, char *buf, int size, char *out_mem, int out_size) {
    ExifConfig config;
    unsigned char *exif_data = NULL;
    unsigned int exif_data_len = 0;
    int pic_offset = 0;
    int pic_raw_data_len = 0;
    int len = 0;
    FILE *fd = NULL;

    if (jpeg_buf == NULL || jpeg_buf_len <= 0 || buf == NULL || size < 0 || out_mem == NULL || out_size <= 0) {
        printf("%s(%s): error:params is wrong!\n",__FILE__, __LINE__);
        return 0;
    }

    if (size == 0) {

        config.flag = 1;
        if (buf == NULL) {
            printf("Error:config file is null!\n");
            return 0;
        }
        fd = fopen(buf, "rb");

        if (fd == NULL) {
            printf("Error:open %s failed!\n",buf);
            goto err;
        }

        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        rewind(fd);
        fclose(fd);
    } else {
        config.flag = 0;
    }

    //generate exif data
    config.tag = EXIF_TAG_USER_COMMENT;
    config.format = EXIF_FORMAT_BYTE;
    config.len = 1;
    config.components = sizeof(ASCII_COMMENT) - 1 + size;
    config.value = buf;

    if (!generate_exif_data(&config, &exif_data, &exif_data_len)) {
        printf("%s(%s): error: generate_exif_data fail!\n",__FILE__, __LINE__);
        goto err;
    }

    //find picture data offset in jpeg_buf
    if (!jpegbuf_picdata_offset(jpeg_buf, jpeg_buf_len, &pic_offset)) {
        printf("%s(%s): error: jpegbuf_picdata_offset fail!\n",__FILE__, __LINE__);
        goto err;
    }

    len = exif_header_len + 2 + exif_data_len + jpeg_buf_len - pic_offset;
    if (len > out_size) {
        printf("%s(%s): error: user memory is too small,needSize:%d userSize:%d!\n",__FILE__, __LINE__, len, out_size);
        goto err;
    }
    len = 0;
    //generate jpeg exif file to memeroy
    memcpy(out_mem + len, exif_header, exif_header_len);
    len += exif_header_len;
    *(out_mem + len) = (exif_data_len + 2) >> 8;
    len += 1;
    *(out_mem + len) = (exif_data_len + 2) & 0xff;
    len += 1;
    memcpy(out_mem + len, exif_data, exif_data_len);
    len += exif_data_len;
    memcpy(out_mem + len, jpeg_buf + pic_offset, jpeg_buf_len - pic_offset);
    len += jpeg_buf_len - pic_offset;

    #if 0
    fd = fopen("/jpeg_exif.jpg", "wb+");
    if (fd == NULL) {
        printf("%s(%s): error: create %s fail!\n",__FILE__, __LINE__, "/jpeg_exif.jpg");
        goto err;
    }
    fwrite(out_mem, 1, len, fd);
    fclose(fd);
    #endif
    free(exif_data);
    return len;
err:
    free(exif_data);
    return 0;
}

