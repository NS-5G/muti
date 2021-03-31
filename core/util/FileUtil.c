/*
 * FileUtil.c
 *
 *  Created on: Mar 31, 2021
 *      Author: root
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <util/FileUtil.h>

static void sysRead(int fd, char *buf, ssize_t len)
{
        ssize_t n;
        while (len > 0) {
                n = read(fd, buf, len);
                if (n < 0) {
                        if (errno == EINTR || errno == EAGAIN) continue;
                        printf("read error n=%lu, len=%lu.\n", n, len);
                        assert(0);
                } else if (n == 0) {
                        printf("read 0 error n=%lu, len=%lu.\n", n, len);
                        assert(0);
                }
                len -= n;
                buf += n;
        }
}

bool fileUtilReadAFile(char *path, char **buffer, ssize_t *buf_len) {
        int fd;
        struct stat st;
        
        fd = open(path, O_RDONLY, S_IRWXU|S_IRGRP|S_IROTH);
        if (fd < 0) {
                return false;
        }
        assert(fstat(fd, &st) == 0);
        *buf_len = st.st_size;
        *buffer = malloc(*buf_len);
        sysRead(fd, *buffer, *buf_len);
        close(fd);
        return true;
}

static void sysWrite(int fd, char *buf, ssize_t len)
{
        ssize_t n;
        while (len) {
                n = write(fd, buf, len);
                if (n < 0) {
                        if (errno == EINTR || errno == EAGAIN) continue;
                        printf("write error n=%lu, len=%lu.\n", n, len);
                        assert(0);
                }
                len -= n;
                buf += n;
        }
}

bool fileUtilWriteAFile(char *path, char *buffer, size_t buf_len) {
        int fd;
        fd = open(path, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH);
        if (fd < 0) {
                return false;
        }
        sysWrite(fd, buffer, buf_len);
        close(fd);
        return true;
}


