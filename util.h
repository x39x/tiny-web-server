#ifndef UTIL_H
#define UTIL_H

int send_all(int fd, const char *buf, long len);
const char *guess_type(const char *path);

#endif
