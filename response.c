#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Response make_text_response(const char *status, const char *content_type,
                            const char *text) {
        Response r;
        r.status = status;
        r.content_type = content_type;
        r.body = (char *)text;
        r.body_len = strlen(text);
        r.need_free = 0;
        return r;
}

Response make_file_response(const char *filename, const char *content_type) {
        FILE *f = fopen(filename, "rb");
        if (!f) {
                return make_text_response("404 Not Found", "text/plain",
                                          "not found\n");
        }

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buf = malloc(size);
        if (!buf) {
                fclose(f);
                return make_text_response("500 Internal Server Error",
                                          "text/plain", "malloc failed\n");
        }

        size_t n = fread(buf, 1, size, f);
        fclose(f);

        Response r;
        r.status = "200 OK";
        r.content_type = content_type;
        r.body = buf;
        r.body_len = n;
        r.need_free = 1;
        return r;
}

void free_response(Response *r) {
        if (r->need_free) {
                free(r->body);
        }
}
