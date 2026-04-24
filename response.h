#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct {
        const char *status;
        const char *content_type;
        char *body;
        long body_len;
        int need_free;
} Response;

Response make_text_response(const char *status, const char *content_type,
                            const char *text);

Response make_file_response(const char *filename, const char *content_type);

void free_response(Response *r);

#endif
