/*
 * Copyright (C) 2013 The Android Open Source Project
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

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "debug.h"
#include "protocol.h"
#include "transport.h"
#include "errno.h"
#include <stdlib.h>
#include <fcntl.h>

#define COMMAND_BUF_SIZE 24
//ivan test
//#define COMMAND_BUF_SIZE 8

ssize_t transport_handle_write(struct transport_handle *thandle, char *buffer, size_t len)
{
    return thandle->transport->write(thandle, buffer, len);
}

void transport_handle_close(struct transport_handle *thandle)
{
    thandle->transport->close(thandle);
}

int transport_handle_download(struct transport_handle *thandle, size_t len)
{
    ssize_t ret;
    size_t n = 0;
    int fd;
    // TODO: move out of /dev
    char tempname[] = "/dev/fastboot_download";
    char *buffer;

	fd = open(tempname, O_RDWR|O_CREAT);
    if (fd < 0)
        return -1;

    ftruncate(fd, len);

    buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == NULL) {
        D(ERR, "mmap(%u) failed: %d %s", len, errno, strerror(errno));
        goto err;
    }

    while (n < len) {
        ret = thandle->transport->read(thandle, buffer + n, len - n);
        if (ret <= 0) {
            D(WARN, "transport read failed, ret=%d %s", ret, strerror(-ret));
            break;
        }
        n += ret;
    }

    munmap(buffer, len);

    if (n != len)
        goto err;

    close(fd);
    return fd;

err:
    close(fd);
    transport_handle_close(thandle);
    return -1;
}

int transport_handle_download_stub(struct transport_handle *thandle, size_t len)
{
    ssize_t ret;
    size_t n = 0;
    int fd;
    int fd_ius;
    // TODO: move out of /dev
    char tempname[] = "/dev/fastboot_download";
	char ius_path[] = "/burn.ius";
    char *buffer;

	fd_ius = open(ius_path, O_RDONLY);
    	

	if (fd_ius < 0) {
        D(WARN, "transport open ius(%s)failed, ret=%d", ius_path, fd_ius);
		close(fd_ius);
		return -1;
	}
	len = lseek(fd_ius, 0, SEEK_END);
    D(WARN, "transport seek end ius(%s) len=%d", ius_path, len);

	ret = lseek(fd_ius, 0, SEEK_SET);	
	if (ret < 0) {
        D(WARN, "transport seek 0 ius(%s)failed, ret=%d", ius_path, fd_ius);
		close(fd_ius);
		return -1;
	}

    //fd = mkstemp(tempname);
	fd = open(tempname, O_RDWR|O_CREAT);
    if (fd < 0)	{
        D(ERR, "mkstemp(%u) failed: %s", fd, tempname);
        return -1;
	}

    //unlink(tempname);

    ftruncate(fd, len);

    buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == NULL) {
        D(ERR, "mmap(%u) failed: %d %s", len, errno, strerror(errno));
        goto err;
    }


    while (n < len) {
        ret = read(fd_ius, buffer + n, len - n);
        if (ret <= 0) {
            D(WARN, "transport read failed, n=%d, ret=%d %s", n, ret, strerror(-ret));
            break;
        }
        n += ret;
    }

    munmap(buffer, len);
		
	close(fd_ius);
    if (n != len)
        goto err;

    close(fd);

    D(WARN, "transport download success");
    return fd;

err:
    close(fd);
    return -1;
}

static void *transport_data_thread(void *arg)
{
    struct transport_handle *thandle = arg;
    struct protocol_handle *phandle = create_protocol_handle(thandle);

    while (!thandle->stopped) {
        int ret;
        char buffer[COMMAND_BUF_SIZE + 1];
        D(VERBOSE, "transport_data_thread\n");

        ret = thandle->transport->read(thandle, buffer, COMMAND_BUF_SIZE);
        if (ret <= 0) {
            D(DEBUG, "ret = %d\n", ret);
            break;
        }
        if (ret > 0) {
            buffer[ret] = 0;
            protocol_handle_command(phandle, buffer);
        }
    }

    transport_handle_close(thandle);
    free(thandle);

    return NULL;
}

static void *transport_connect_thread(void *arg)
{
    struct transport *transport = arg;
    while (!transport->stopped) {
        struct transport_handle *thandle;
        pthread_t thread;
        pthread_attr_t attr;

        D(VERBOSE, "transport_connect_thread\n");
        thandle = transport->connect(transport);
        if (thandle == NULL) {
            D(ERR, "transport connect failed\n");
            sleep(1);
            continue;
        }
        thandle->transport = transport;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        pthread_create(&thread, &attr, transport_data_thread, thandle);

        sleep(1);
    }

    return NULL;
}

void transport_register(struct transport *transport)
{
    pthread_t thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&thread, &attr, transport_connect_thread, transport);
}
