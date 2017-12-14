#ifndef APIEXIF_H
#define APIEXIF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>


#define RT_NOTENOUGH_MEM     -1
#define RT_NOTEXIF_FORMAT    -2
#define RT_NOTSETTAG_VALUE   -3
#define RT_ERR_ARGS          -4

/*
    func:jpeg_set_exif
    args:
         jpeg_file:point to jpeg location
         buf      :data to be setted.Can be one marker value or an input exif config file name
         size     :data size
    des:
         set data to jpeg file's exif.The exif marker is EXIF_TAG_USER_COMMENT.
         1.if size > 0,then "buf" means marker(EXIF_TAG_USER_COMMENT) value.
         2.if size == 0,then "buf" means a filename which contains exif config data.
           Note:usr config data must write in the marker(EXIF_TAG_USER_COMMENT),so jpeg_get_exif function can get the value.
*/
void jpeg_set_exif(char *jpeg_file, char *buf, int size);


/*
    func:jpeg_get_exif
    args:
         jpeg_file:point to jpeg location
         buf      :user offers buffer to store exif data
         size     :buffer max size
    ret:
        if ret >= 0 the real size exif data
        if ret < 0  get exif data fail: RT_NOTENOUGH_MEM:user offers buf space is not enough to store exif data
                                        RT_NOTEXIF_FORMAT:the jpeg file does not contain exif info
                                        RT_NOTSETTAG_VALUE:the jpeg does not set the marker value
                                        RT_ERR_ARGS:input arguments is wrong
    des:
         get exif data from jpeg file.The exif marker is EXIF_TAG_USER_COMMENT.

*/
int jpeg_get_exif(char *jpeg_file,char *buf, int size);

/*
    func:jpegbuf_set_exif
    args:
         jpeg_buf    : input buffer that holds jpeg data
         jpeg_buf_len: jpeg data length
         buf         : input data that contains exif data.It can be a config file or buffer data.
         size        : exif data size. size == 0, means set exif data from a file.
                                       size > 0, means set exif data from buffer.
         jpeg_file   : output.Saving the configed jpeg to the jpeg_file.
    ret:
         when ret is 0,means setting fails.
              ret is 1,means setting sucesses.
    des:
         set jpeg exif data from jpeg buf, save the last data to a jpeg file.The exif marker is EXIF_TAG_USER_COMMENT.

*/

int jpegbuf_set_exif(char *jpeg_buf, int jpeg_buf_len, char *buf, int size, char *jpeg_file);


/*
    func:jpegbuf_set_exif2mem
    args:
         jpeg_buf    : input buffer that holds jpeg data
         jpeg_buf_len: jpeg data length
         buf         : input data that contains exif data.It can be a config file or buffer data.
         size        : exif data size. size == 0, means set exif data from a file.
                                       size > 0, means set exif data from buffer.
         out_mem     : Saving the configed jpeg data to user's memory.
         out_size    : user memory size,it must be big enough
    ret:
         when ret is 0,means setting fails.
              ret > 0,means setting sucesses, actual data size
    des:
         set jpeg exif data from jpeg buf,save the last data to user memory.The exif marker is EXIF_TAG_USER_COMMENT.

*/

int jpegbuf_set_exif2mem(char *jpeg_buf, int jpeg_buf_len, char *buf, int size, char *out_mem, int out_size);
#ifdef __cplusplus
}
#endif
#endif
