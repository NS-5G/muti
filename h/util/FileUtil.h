/*
 * FileUtil.h
 *
 *  Created on: Mar 31, 2021
 *      Author: root
 */

#ifndef UTIL_FILEUTIL_H_
#define UTIL_FILEUTIL_H_
#include <stdbool.h>

extern bool fileUtilReadAFile(char *path, char **buffer, ssize_t *buf_len);
extern bool fileUtilWriteAFile(char *path, char *buffer, ssize_t buf_len);

#endif /* UTIL_FILEUTIL_H_ */
