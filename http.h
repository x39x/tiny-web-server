#ifndef HTTP_H
#define HTTP_H

#include "response.h"

char *read_http_header(int fd, long *out_len);
void send_response(int client_fd, const Response *r, int keep_alive);

#endif
