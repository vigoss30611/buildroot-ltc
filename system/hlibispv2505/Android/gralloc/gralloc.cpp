/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "gralloc"

#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <cutils/ashmem.h>
#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include <felixcommon/ci_alloc_info.h>
#include <img_errors.h>

#include <ion/ion.h>

#include "gralloc_priv.h"
#include "gr.h"

/*****************************************************************************/

// libion in Android Kitkat uses struct ion_handle*
// Lollipop has ion_user_handle_t
#ifdef HAS_STRUCT_ION_HANDLE
typedef struct ion_handle* ion_user_handle_t;
#endif

struct gralloc_context_t {
    alloc_device_t device;
    /* our private data here */
};
static int ion_client_fd = -1;
//static int gralloc_alloc_buffer(alloc_device_t* dev,
//        struct buffer_attrs &attrs, buffer_handle_t* pHandle);
static int gralloc_alloc_ion_buffer(alloc_device_t* dev,
        struct buffer_attrs &attrs, buffer_handle_t* pHandle, int stride);
/*****************************************************************************/

int fb_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device);

static int gralloc_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device);

extern int gralloc_lock(gralloc_module_t const* module,
        buffer_handle_t handle, int usage,
        int l, int t, int w, int h,
        void** vaddr);

extern int gralloc_unlock(gralloc_module_t const* module,
        buffer_handle_t handle);

extern int gralloc_register_buffer(gralloc_module_t const* module,
        buffer_handle_t handle);

extern int gralloc_unregister_buffer(gralloc_module_t const* module,
        buffer_handle_t handle);

extern int gralloc_lock_ycbcr(gralloc_module_t const* module,
        buffer_handle_t handle, int usage, int l, int t, int w, int h,
        android_ycbcr *ycbcr);
/*****************************************************************************/

static struct hw_module_methods_t gralloc_module_methods = {
        /*open:*/ gralloc_device_open
};

struct private_module_t HAL_MODULE_INFO_SYM = {
    /*base:*/ {
        /*common:*/ {
            /*tag:*/ HARDWARE_MODULE_TAG,
            /*version_major:*/ 1,
            /*version_minor:*/ 0,
            /*id:*/ GRALLOC_HARDWARE_MODULE_ID,
            /*name:*/ "Graphics Memory Allocator Module",
            /*author:*/ "The Android Open Source Project",
            /*methods:*/ &gralloc_module_methods,
            /*dso:*/ NULL,
            /*reserved:*/ { 0, }
        },
        /*registerBuffer:*/ gralloc_register_buffer,
        /*unregisterBuffer:*/ gralloc_unregister_buffer,
        /*lock:*/ gralloc_lock,
        /*unlock:*/ gralloc_unlock,
        /*perform:*/ NULL,
        /*lock_ycbcr:*/ gralloc_lock_ycbcr,
#ifdef GRALLOC_MODULE_API_VERSION_0_3
        NULL,
        NULL,
        NULL,
#endif
        /*reserved_proc:*/ { 0, }
    },
    /*framebuffer: */NULL,
    /*flags:*/ 0,
    /*numBuffers:*/ 0,
    /*bufferMask:*/ 0,
    /*lock:*/ PTHREAD_MUTEX_INITIALIZER,
    /*currentBuffer:*/ NULL,
    /*pmem_master:*/ 0,
    /*pmem_master_base:*/ NULL,
    /*info:*/ { 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0 ,0 },
        { 0, 0, 0 }, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { } },
    /*finfo:*/ { { }, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { } },
    /*xdpi:*/ 0.0,
    /*ydpi:*/ 0.0,
    /*fps:*/ 0.0
};

/*****************************************************************************/

static int gralloc_alloc_framebuffer_locked(alloc_device_t* dev,
        struct buffer_attrs &attrs, buffer_handle_t* pHandle)
{
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);

    // allocate the framebuffer
    if (m->framebuffer == NULL) {
        // initialize the framebuffer, the framebuffer is mapped once
        // and forever.
        int err = mapFrameBufferLocked(m);
        if (err < 0) {
            return err;
        }
    }

    const uint32_t bufferMask = m->bufferMask;
    const uint32_t numBuffers = m->numBuffers;
    const size_t bufferSize = m->finfo.line_length * m->info.yres;

    if (numBuffers == 1) {
        // If we have only one buffer, we never use page-flipping. Instead,
        // we return a regular buffer which will be memcpy'ed to the main
        // screen when post is called.
        int newUsage = (attrs.usage & ~GRALLOC_USAGE_HW_FB) |
            GRALLOC_USAGE_HW_2D;
        struct buffer_attrs newAttrs(bufferSize, newUsage, 0, 0, 0);
        return gralloc_alloc_ion_buffer(dev, newAttrs, pHandle, m->finfo.line_length);
    }

    if (bufferMask >= ((1LU<<numBuffers)-1)) {
        // We ran out of buffers.
        return -ENOMEM;
    }

    // create a "fake" handles for it
    intptr_t vaddr = intptr_t(m->framebuffer->base);
    private_handle_t* hnd = new private_handle_t(dup(m->framebuffer->fd),
            attrs, private_handle_t::PRIV_FLAGS_FRAMEBUFFER);

    // find a free slot
    for (uint32_t i=0 ; i<numBuffers ; i++) {
        if ((bufferMask & (1LU<<i)) == 0) {
            m->bufferMask |= (1LU<<i);
            break;
        }
        vaddr += bufferSize;
    }

    hnd->base = vaddr;
    hnd->offset = vaddr - intptr_t(m->framebuffer->base);
    *pHandle = hnd;

    return 0;
}

static int gralloc_alloc_ion_framebuffer(alloc_device_t* dev,
        struct buffer_attrs &attrs, buffer_handle_t* pHandle)
{
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);
    pthread_mutex_lock(&m->lock);
    int err = gralloc_alloc_framebuffer_locked(dev, attrs, pHandle);
    pthread_mutex_unlock(&m->lock);
    return err;
}

/*****************************************************************************/

static int gralloc_alloc_ion_buffer(alloc_device_t* dev,
        struct buffer_attrs &attrs, buffer_handle_t* pHandle,
        __maybe_unused int stride)
 {
    int err = 0;
    int fd = -1;
    int tmp;
    ion_user_handle_t handle;

    tmp = roundUpToPageSize(attrs.size);

    gralloc_context_t* ctx = reinterpret_cast<gralloc_context_t*>(dev);

#ifdef GRALLOC_ION_USES_CARVEOUT_HEAP
    err = ion_alloc(ion_client_fd, tmp, PAGE_SIZE, ION_HEAP_CARVEOUT_MASK, 0,
            &handle);
#else
    err = ion_alloc(ion_client_fd, tmp, PAGE_SIZE, ION_HEAP_SYSTEM_MASK, 0,
            &handle);
#endif

    if (err < 0) {
        ALOGE("couldn't allocate ION buffer (%s)", strerror(-errno));
        return err;
    }
    err = ion_share(ion_client_fd, handle, &fd);
    if (err < 0) {
        ALOGE("couldn't share ION buffer (%s)", strerror(-errno));
        err = -errno;
    }
    // It is enough to have a file descriptor: we can free the handle
    ion_free(ion_client_fd, handle);

    if (err == 0) {
        private_handle_t* hnd = new private_handle_t(fd, attrs,
               private_handle_t::PRIV_FLAGS_USES_ION);
        gralloc_module_t* module = reinterpret_cast<gralloc_module_t*>(
                dev->common.module);
        err = mapBuffer(module, hnd);

        if (err == 0) {
            *pHandle = hnd;
            ALOGD("%s: ION buffer allocation succeeded fd=%d",
                    __FUNCTION__, fd);
        } else {
            ALOGE("%s: failed to map buffer!", __FUNCTION__);
            return err;
        }
    }

    ALOGE_IF(err, "gralloc failed err=%s", strerror(-err));

    return err;
}

#define ALIGN_SIZE(size, alignment) \
    (((size) + (alignment) - 1) & ~((alignment) - 1))

static int calculateYuvBufferForHw(enum pxlFormat format,
        int w, int h,
        size_t* size, size_t* stride, int* bpp) {

    ALOG_ASSERT(size, "%s: size param is NULL", __FUNCTION__);
    ALOG_ASSERT(stride, "%s: stride param is NULL", __FUNCTION__);
    ALOG_ASSERT(bpp, "%s: param is NULL", __FUNCTION__);

    PIXELTYPE pixelType;
    struct CI_SIZEINFO sizeInfo = CI_SIZEINFO();

    if (PixelTransformYUV(&pixelType, format) != IMG_SUCCESS) {
        ALOGE("%s: PixelTransformYUV function failed!", __FUNCTION__);
        return -EINVAL;
    }
    if (CI_ALLOC_YUVSizeInfo(&pixelType, w, h, NULL /* no tiling */,
                &sizeInfo) != IMG_SUCCESS) {
        ALOGE("%s: Cannot calculate YUV buffer size!", __FUNCTION__);
        return -EINVAL;
    }
    // Important: both strides (luma and chroma) should be the same!!!
    // This is the only configuration supported by SW at the moment.
    if (sizeInfo.ui32Stride != sizeInfo.ui32CStride) {
        ALOGE("%s: Not supported YUV configuration: Y stride (%d) "
               "and C stride (%d) are different!", __FUNCTION__,
               sizeInfo.ui32Stride, sizeInfo.ui32CStride);
        return -EINVAL;
    }
    *stride = sizeInfo.ui32Stride;
    *size = sizeInfo.ui32Stride * sizeInfo.ui32Height +
            sizeInfo.ui32CStride * sizeInfo.ui32CHeight;
    // This is workaround! Extra page should be allocated for HW_1.2
    *size = ALIGN_SIZE(*size, PAGE_SIZE);
    *bpp = pixelType.ui8PackedStride;
    //---------------------------------------------------------------------
    ALOGV("%s: Calculated buffer size (f=%d): ui32Stride = %d ui32Height = %d"
            " ui32CStride = %d ui32CHeight = %d size = %d", __FUNCTION__,
            format, sizeInfo.ui32Stride, sizeInfo.ui32Height,
            sizeInfo.ui32CStride, sizeInfo.ui32CHeight, *size);

    return 0;
}

static int calculateRgbBufferForHw(enum pxlFormat format, int w, int h,
        size_t* size, size_t* stride, int* bpp ) {

    ALOG_ASSERT(size, "%s: size param is NULL", __FUNCTION__);
    ALOG_ASSERT(stride, "%s: stride param is NULL", __FUNCTION__);
    ALOG_ASSERT(bpp, "%s: param is NULL", __FUNCTION__);

    PIXELTYPE pixelType;
    struct CI_SIZEINFO sizeInfo = CI_SIZEINFO();

    if (PixelTransformRGB(&pixelType, format) != IMG_SUCCESS) {
        ALOGE("%s: PixelTransformRGB function failed!", __FUNCTION__);
        return -EINVAL;
    }
    if (CI_ALLOC_RGBSizeInfo(&pixelType, w, h, NULL /* no tiling */,
                &sizeInfo) != IMG_SUCCESS) {
        ALOGE("%s: Cannot calculate RGB buffer size!", __FUNCTION__);
        return -EINVAL;
    }

    *stride = sizeInfo.ui32Stride;
    // In case of RGB chroma parameters should be 0.
    *size = sizeInfo.ui32Stride * sizeInfo.ui32Height +
            sizeInfo.ui32CStride * sizeInfo.ui32CHeight;
    // This is workaround! Extra page should be allocated for HW_1.2
    *size = ALIGN_SIZE(*size, PAGE_SIZE);
    *bpp = pixelType.ui8PackedStride;
    //---------------------------------------------------------------------
    ALOGD("%s: Calculated buffer size (f=%d): ui32Stride = %d ui32Height = %d"
            " ui32CStride = %d ui32CHeight = %d size = %d", __FUNCTION__,
            format, sizeInfo.ui32Stride, sizeInfo.ui32Height,
            sizeInfo.ui32CStride, sizeInfo.ui32CHeight, *size);

    return 0;
}

static int calculateRawBufferForHw(enum pxlFormat format, int w, int h,
        enum MOSAICType mosaic,
        size_t* size, size_t* stride, int* bpp) {

    ALOG_ASSERT(size, "%s: size param is NULL", __FUNCTION__);
    ALOG_ASSERT(stride, "%s: stride param is NULL", __FUNCTION__);
    ALOG_ASSERT(bpp, "%s: param is NULL", __FUNCTION__);

    PIXELTYPE pixelType;
    struct CI_SIZEINFO sizeInfo = CI_SIZEINFO();

    if (PixelTransformBayer(&pixelType, format, mosaic) != IMG_SUCCESS) {
        ALOGE("%s: PixelTransformRGB function failed!", __FUNCTION__);
        return -EINVAL;
    }
    if (CI_ALLOC_Raw2DSizeInfo(&pixelType, w, h, NULL /* no tiling */,
                &sizeInfo) != IMG_SUCCESS) {
        ALOGE("%s: Cannot calculate RAW buffer size!", __FUNCTION__);
        return -EINVAL;
    }

    *stride = sizeInfo.ui32Stride;
    // In case of RAW chroma parameters should be 0.
    *size = sizeInfo.ui32Stride * sizeInfo.ui32Height +
            sizeInfo.ui32CStride * sizeInfo.ui32CHeight;
    // This is workaround! Extra page should be allocated for HW_1.2
    *size = ALIGN_SIZE(*size, PAGE_SIZE);
    *bpp = pixelType.ui8PackedStride;
    //---------------------------------------------------------------------
    ALOGD("%s: Calculated buffer size (f=%d): ui32Stride = %d ui32Height = %d"
            " ui32CStride = %d ui32CHeight = %d size = %d", __FUNCTION__,
            format, sizeInfo.ui32Stride, sizeInfo.ui32Height,
            sizeInfo.ui32CStride, sizeInfo.ui32CHeight, *size);

    return 0;
}

static int gralloc_alloc(alloc_device_t* dev, int w, int h, int format,
        int usage, buffer_handle_t* pHandle, int* pStride)
{
    int err;
    h = ALIGN_SIZE(h, 16);
    int align = 64;
    size_t size = 0;
    size_t stride = 0;

    int bpp = 0;

    ALOGD("%s: w=%d h=%d usage=0x%x format=0x%x", __FUNCTION__, w, h, usage,
            format);

    // Implementation defined formats
    if (format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        ALOGD("%s: IMPLEMENTATION_DEFINED buffer requested.", __FUNCTION__);

        if ((usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) ||
            ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) ==
                      GRALLOC_USAGE_HW_CAMERA_ZSL)) {
            // YUV buffers used by Camera output - Encoder input (NV21)
            format = HAL_PIXEL_FORMAT_YCbCr_420_888;
        } else if (usage & (GRALLOC_USAGE_HW_TEXTURE |
                    GRALLOC_USAGE_HW_COMPOSER |
                    GRALLOC_USAGE_HW_FB)) {
            // RGB buffers used by Camera / Display output.
            format = HAL_PIXEL_FORMAT_RGBA_8888;
        } else {
            ALOGE("%s: Not expected buffer usage (%#x)!", __FUNCTION__, usage);
            return -EINVAL;
        }
    }

        //---------------------------------------------------------------------
    if (format == HAL_PIXEL_FORMAT_YCbCr_420_888) {
        //
        // YUV buffers supported by Camera HW.
        // Default format is: YVU_420_PL12_8.
        //
        if (calculateYuvBufferForHw(YUV_420_PL12_8,
                w, h, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    } else if (format == HAL_PIXEL_FORMAT_YCrCb_420_SP) {
        //
        // YUV buffers supported by Camera HW.
        //
        if (calculateYuvBufferForHw(YVU_420_PL12_8,
                w, h, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    } else if (format == HAL_PIXEL_FORMAT_YCbCr_422_SP) {
        //
        // YUV buffers supported by Camera HW.
        //
        if (calculateYuvBufferForHw(YUV_422_PL12_8,
                w, h, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    }
    else if (format == HAL_PIXEL_FORMAT_RGBA_8888 ||
            format == HAL_PIXEL_FORMAT_RGBX_8888 ||
            format == HAL_PIXEL_FORMAT_BGRA_8888) {
        //
        // generic RGB formats, supported by Camera HW
        //
        if (calculateRgbBufferForHw(RGB_888_32,
                w, h, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    }
    else if (format == HAL_PIXEL_FORMAT_RGB_888) {
        //
        // generic RGB formats, supported by Camera HW
        //
        if (calculateRgbBufferForHw(RGB_888_24,
                w, h, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    }
    else if (format == HAL_PIXEL_FORMAT_RAW_SENSOR) {
        //
        // RAW format, supported by Camera HW
        //
        if (calculateRawBufferForHw(BAYER_RGGB_10,
                w, h, MOSAIC_RGGB, &size, &stride, &bpp) != 0) {
            return -EINVAL;
        }
    } else {
        switch (format) {
            case HAL_PIXEL_FORMAT_RGB_565:
                bpp = 2;
                break;
            case HAL_PIXEL_FORMAT_BLOB:
                // force buffer params required by BLOB
                bpp = 1;
                h = 1;
                break;
            default:
                ALOGE("%s: Allocation failed, Invalid buffer format!",
                        __FUNCTION__);
                return -EINVAL;
        }

        stride = (w * bpp + (align - 1)) & ~(align - 1);
        size = stride * h;

        if (!size || !stride) {
            ALOGE("%s: buffer size and stride cannot be 0", __FUNCTION__);
            return -EINVAL;
        }
    }

    struct buffer_attrs attrs(size, usage, w, h, format, stride, bpp);
    if (usage & GRALLOC_USAGE_HW_FB) {
        ALOGD("%s: allocating framebuffer", __FUNCTION__);
        err = gralloc_alloc_ion_framebuffer(dev, attrs, pHandle);
    } else {
        ALOGD("%s: allocating ION buffer", __FUNCTION__);
        err = gralloc_alloc_ion_buffer(dev, attrs, pHandle, stride);
    }

    if (err < 0)
        return err;

    ALOGD("handle=%p", *pHandle);

    *pStride = stride/bpp;

    return 0;
}

static int gralloc_free(alloc_device_t* dev, buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    const private_handle_t *hnd =
        reinterpret_cast<const private_handle_t *>(handle);
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);

    if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) {
        // free this buffer
        const size_t bufferSize = m->finfo.line_length * m->info.yres;
        int index = (hnd->base - m->framebuffer->base) / bufferSize;
        m->bufferMask &= ~(1<<index);
    } else {
        terminateBuffer(&m->base, const_cast<private_handle_t *>(hnd));
    }

    ALOGD("%s: handle=%p fd=%d", __FUNCTION__, handle, hnd->fd);

    close(hnd->fd);
    delete hnd;
    return 0;
}

/*****************************************************************************/

static int gralloc_close(struct hw_device_t *dev)
{
    gralloc_context_t* ctx = reinterpret_cast<gralloc_context_t*>(dev);
    if (ctx) {
        /* : keep a list of all buffer_handle_t created, and free them
         * all here.
         */
        if (ion_client_fd >= 0) {
            int res = ion_close(ion_client_fd);
            if (res == -1) {
                ALOGE("Error %d (%s) closing ION device",
                errno, strerror(errno));
            }
        }

        delete ctx;
    }
    return 0;
}

int gralloc_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device)
{
    int status = -EINVAL;
    if (!strcmp(name, GRALLOC_HARDWARE_GPU0)) {
        gralloc_context_t *dev = new gralloc_context_t();

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = gralloc_close;

        dev->device.alloc = gralloc_alloc;
        dev->device.free = gralloc_free;

        *device = &dev->device.common;
        status = 0;
    } else status = fb_device_open(module, name, device);

    if (ion_client_fd >= 0)
        return status;

    ion_client_fd = ion_open();
    if (ion_client_fd < 0) {
        ALOGE("Error %d (%s) opening ION device", errno, strerror(errno));
        status = -EINVAL;
    } else ALOGI("ION device opened fd %d", ion_client_fd);

    return status;
}

