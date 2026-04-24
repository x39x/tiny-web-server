#include "http.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static int header_complete(const char *buf, long len) {
        for (long i = 0; i <= len - 4; i++) {
                if (buf[i] == '\r' && buf[i + 1] == '\n' &&
                    buf[i + 2] == '\r' && buf[i + 3] == '\n') {
                        return 1;
                }
        }
        return 0;
}

char *read_http_header(int fd, long *out_len) {
        long cap = 4096, len = 0;
        char *buf = malloc(cap);

        while (1) {
                if (len == cap) {
                        cap *= 2;
                        buf = realloc(buf, cap);
                }

                int n = recv(fd, buf + len, cap - len, 0);
                if (n <= 0) {
                        free(buf);
                        return NULL;
                }

                len += n;

                if (header_complete(buf, len)) {
                        *out_len = len;
                        return buf;
                }
        }
}

void send_response(int client_fd, const Response *r, int keep_alive) {
        char header[1024];

        snprintf(header, sizeof(header),
                 "HTTP/1.1 %s\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %ld\r\n"
                 "Connection: %s\r\n"
                 "\r\n",
                 r->status, r->content_type, r->body_len,
                 keep_alive ? "keep-alive" : "close");

        send_all(client_fd, header, strlen(header));
        send_all(client_fd, r->body, r->body_len);
}
