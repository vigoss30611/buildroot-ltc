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

#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include "gralloc_priv.h"

/*****************************************************************************/

static int gralloc_map(__maybe_unused gralloc_module_t const* module,
        private_handle_t *handle, void** vaddr)
{
    if (!(handle->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER)) {
        size_t size = handle->attrs.size;
        void* mappedAddress = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                handle->fd, 0);
        if (mappedAddress == MAP_FAILED) {
            ALOGE("Could not mmap %s", strerror(errno));
            return -errno;
        }

        handle->base = intptr_t(mappedAddress) + handle->offset;
//        ALOGD("gralloc_map() succeeded fd=%d, off=%d, size=%d, vaddr=%p",
//                handle->fd, handle->offset, handle->attrs.size, mappedAddress);
    }
    *vaddr = (void*)handle->base;
    ALOGD("%s: vaddr=%p", __FUNCTION__, *vaddr);
    return 0;
}

static int gralloc_unmap(__maybe_unused gralloc_module_t const* module,
        private_handle_t *handle)
{
    if (!(handle->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER)) {
        void* base = (void *)handle->base;
        size_t size = handle->attrs.size;
//        ALOGD("unmapping from %p, size=%d", base, size);
        if (munmap(base, size) < 0) {
            ALOGE("Could not unmap %s", strerror(errno));
        }
    }
    handle->base = 0;
    return 0;
}

/*****************************************************************************/

static pthread_mutex_t sMapLock = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************/

int gralloc_register_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    const private_handle_t *hnd =
        reinterpret_cast<const private_handle_t *>(handle);
    ALOGD_IF(hnd->pid == getpid(),
            "Registering a buffer in the process that created it. "
            "This may cause memory ordering problems.");

    ALOGD("%s: fd=%d", __FUNCTION__, hnd->fd);
    void *vaddr;
    return gralloc_map(module, const_cast<private_handle_t *>(hnd), &vaddr);
}

int gralloc_unregister_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    const private_handle_t *hnd =
        reinterpret_cast<const private_handle_t *>(handle);

    ALOGD("%s: fd=%d", __FUNCTION__, hnd->fd);

    if (hnd->base) {
        gralloc_unmap(module, const_cast<private_handle_t *>(hnd));
    }

    return 0;
}

int mapBuffer(gralloc_module_t const* module, private_handle_t* hnd)
{
    void* vaddr;
    return gralloc_map(module, hnd, &vaddr);
}

int terminateBuffer(gralloc_module_t const* module, private_handle_t *hnd)
{
    if (hnd->base) {
        // this buffer was mapped, unmap it now
        gralloc_unmap(module, hnd);
    }

    return 0;
}

int gralloc_lock(__maybe_unused gralloc_module_t const* module,
        buffer_handle_t handle,
        int usage, int l, int t, int w, int h, void** vaddr)
{
    // this is called when a buffer is being locked for software
    // access. in thin implementation we have nothing to do since
    // not synchronization with the h/w is needed.
    // typically this is used to wait for the h/w to finish with
    // this buffer if relevant. the data cache may need to be
    // flushed or invalidated depending on the usage bits and the
    // hardware.

    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    const private_handle_t *hnd =
        reinterpret_cast<const private_handle_t *>(handle);

    if (hnd->attrs.format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        if ((usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) ||
            ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) ==
                      GRALLOC_USAGE_HW_CAMERA_ZSL)) {
            ALOGD("%s: IMPLEMENTATION_DEFINED buffer but usage for encoder",
                __FUNCTION__);
            return -EINVAL;
        }
    }

    *vaddr = (void*)hnd->base;
    return 0;
}

int gralloc_unlock(gralloc_module_t const* module, buffer_handle_t handle) {
    // we're done with a software buffer. nothing to do in this
    // implementation. typically this is used to flush the data cache.

    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;
    return 0;
}

int gralloc_lock_ycbcr(gralloc_module_t const* module, buffer_handle_t handle,
        int usage, int l, int t, int w, int h, android_ycbcr *ycbcr)
{
 //   ALOGD("%s: usage=%#x l=%d t=%d w=%d h=%d", __FUNCTION__, usage, l, t, w, h);

    if (!ycbcr) {
        ALOGE("gralloc_lock_ycbcr got NULL ycbcr struct");
        return -EINVAL;
    }

    if (private_handle_t::validate(handle) < 0) return -EINVAL;

    const private_handle_t *hnd =
        reinterpret_cast<const private_handle_t *>(handle);

    int format = hnd->attrs.format;
    if (format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        ALOGD("%s: IMPLEMENTATION_DEFINED buffer requested.", __FUNCTION__);

        if ((usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) ||
            ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) ==
                      GRALLOC_USAGE_HW_CAMERA_ZSL)) {
            // YUV buffers used by Camera output - Encoder input (NV21)
            format = HAL_PIXEL_FORMAT_YCbCr_420_888;
        }
    }
    if (format != HAL_PIXEL_FORMAT_YCbCr_420_888 &&
        format != HAL_PIXEL_FORMAT_YCrCb_420_SP &&
        format != HAL_PIXEL_FORMAT_YCbCr_422_SP) {
        ALOGE("%s: gralloc_lock_ycbcr can only be used with "
                "HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED or "\
                "HAL_PIXEL_FORMAT_YCbCr_* buffer format. "
                "Got %#x buffer format instead.", __FUNCTION__,
                hnd->attrs.format);
        return -EINVAL;
    }

    // Calculate offsets to underlying YUV data
    size_t yStride;
    size_t cStride;
    size_t yOffset;
    size_t uOffset;
    size_t vOffset;
    size_t cStep;
    switch (hnd->attrs.format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        case HAL_PIXEL_FORMAT_YCbCr_422_SP:
            yStride = hnd->attrs.stride;
            cStride = hnd->attrs.stride;
            yOffset = 0;
            uOffset = yStride * hnd->attrs.height;
            vOffset = uOffset + 1;
            cStep = 2;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
            yStride = hnd->attrs.stride;
            cStride = hnd->attrs.stride;
            yOffset = 0;
            vOffset = yStride * hnd->attrs.height;
            uOffset = vOffset + 1;
            cStep = 2;
            break;
        default:
            // so far no planar formats supported
            ALOGE("gralloc_lock_ycbcr unexpected internal format %d",
                    hnd->attrs.format);
            return -EINVAL;
    }

    ycbcr->y = (void *)(hnd->base + yOffset);
    ycbcr->cb = (void *)(hnd->base + uOffset);
    ycbcr->cr = (void *)(hnd->base + vOffset);
    ycbcr->ystride = yStride;
    ycbcr->cstride = cStride;
    ycbcr->chroma_step = cStep;

    // Zero out reserved fields
    memset(ycbcr->reserved, 0, sizeof(ycbcr->reserved));

    ALOGD("gralloc_lock_ycbcr success. usage: %x, ycbcr.y: %p, .cb: %p, "
            ".cr: %p, .ystride: %d , .cstride: %d, .chroma_step: %d", usage,
            ycbcr->y, ycbcr->cb, ycbcr->cr, ycbcr->ystride, ycbcr->cstride,
            ycbcr->chroma_step);

    return 0;
}
