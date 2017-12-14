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

#ifndef GRALLOC_PRIV_H_
#define GRALLOC_PRIV_H_

#include <stdint.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <hardware/gralloc.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include <cutils/native_handle.h>

#include <linux/fb.h>

/*****************************************************************************/

struct private_module_t;
struct private_handle_t;

struct private_module_t {
    gralloc_module_t base;

    private_handle_t* framebuffer;
    uint32_t flags;
    uint32_t numBuffers;
    uint32_t bufferMask;
    pthread_mutex_t lock;
    buffer_handle_t currentBuffer;
    int pmem_master;
    void* pmem_master_base;

    struct fb_var_screeninfo info;
    struct fb_fix_screeninfo finfo;
    float xdpi;
    float ydpi;
    float fps;
};

/*****************************************************************************/

// Buffer attributes
struct buffer_attrs {
#ifdef __cplusplus
    buffer_attrs(int size, int usage, int width, int height,
            int format, int stride=0, int bpp=1,
            bool tiled=false) :
        size(size),
        usage(usage),
        width(width),
        height(height),
        format(format),
        stride(stride),
        bpp(bpp),
        tiled(tiled){}
#endif
    int size;
    int usage;   // usage bits the buffer was created with
    int width;   // buffer width in pixels
    int height;  // buffer height in pixels
    int format;  // real internal pixel format format
    int stride;  // buffer stride in bytes
    int bpp;     // bytes per pixel
    bool tiled;  // true if tiled
};

#ifdef __cplusplus
struct private_handle_t : public native_handle {
#else
    struct private_handle_t {
        struct native_handle nativeHandle;
#endif

    enum {
        PRIV_FLAGS_FRAMEBUFFER = 0x00000001,
        PRIV_FLAGS_USES_ION = 0x00000002
    };

    // file-descriptors
    int fd;
    // ints
    int magic;
    int flags;

    // FIXME: the attributes below should be out-of-line
    int offset;
    int base;
    int pid;
    struct buffer_attrs attrs;
    /***
     * @brief the id of the buffer when registered in HW mmu
     * @note Modified and read in Felix ISP Camera HAL 3.2
     */
    unsigned int importedId;

#ifdef __cplusplus
    static const int sNumInts = 6 + sizeof(struct buffer_attrs) / sizeof(int);
    static const int sNumFds = 1;
    static const int sMagic = 0x3141592;

    private_handle_t(int fd, struct buffer_attrs &attrs, int flags) :
        fd(fd), magic(sMagic), flags(flags), offset(0), base(0), pid(getpid()),
        attrs(attrs)
    {
        version = sizeof(native_handle);
        numInts = sNumInts;
        numFds = sNumFds;
    }
    ~private_handle_t() {
        version = 0;
        magic = 0;
    }

    static int validate(const native_handle* h) {
        const private_handle_t* hnd = (const private_handle_t*)h;
        if (!h || h->version != sizeof(native_handle) ||
                h->numInts != sNumInts || h->numFds != sNumFds ||
                hnd->magic != sMagic)
        {
            ALOGE("invalid gralloc handle (at %p)", h);
            return -EINVAL;
        }
        return 0;
    }
public:
    int getIonFd(void) const { return fd; }
    int getStride(void) const { return attrs.stride; } // bufer stride in bytes
    int getBpp(void) const { return attrs.bpp; }
    int getSize(void) const { return attrs.size; } // buffer size in bytes
    bool getTiled(void) const { return attrs.tiled; }
#endif
};

#endif /* GRALLOC_PRIV_H_ */

