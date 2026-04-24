#include "util.h"
#include <string.h>
#include <sys/socket.h>

int send_all(int fd, const char *buf, long len) {
        long sent = 0;
        while (sent < len) {
                long n = send(fd, buf + sent, len - sent, 0);
                if (n <= 0)
                        return -1;
                sent += n;
        }
        return 0;
}

const char *guess_type(const char *path) {
        const char *ext = strrchr(path, '.');

        if (!ext)
                return "application/octet-stream";
        if (strcmp(ext, ".html") == 0)
                return "text/html";
        if (strcmp(ext, ".txt") == 0)
                return "text/plain";
        if (strcmp(ext, ".css") == 0)
                return "text/css";
        if (strcmp(ext, ".js") == 0)
                return "application/javascript";

        return "application/octet-stream";
}
