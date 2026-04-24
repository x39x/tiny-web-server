#include "server.h"
#include "http.h"
#include "response.h"
#include "router.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int handle_one_request(int client_fd) {
        long req_len = 0;
        char *req = read_http_header(client_fd, &req_len);
        if (!req) {
                return 0;
        }

        req = realloc(req, req_len + 1);
        req[req_len] = '\0';

        char method[16] = {0};
        char path[1024] = {0};
        char version[16] = {0};

        if (sscanf(req, "%15s %1023s %15s", method, path, version) != 3) {
                Response r = make_text_response("400 Bad Request", "text/plain",
                                                "bad request\n");
                send_response(client_fd, &r, 0);
                free(req);
                return 0;
        }

        // 默认 keep-alive（HTTP/1.1）
        int keep_alive = 1;

        if (strstr(req, "Connection: close") != NULL) {
                keep_alive = 0;
        }

        Response r = route(path);
        send_response(client_fd, &r, keep_alive);
        free_response(&r);

        free(req);
        return keep_alive;
}

void start_server(int port) {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);

        // TCP 连接关闭时，会经历“四次挥手”，其中主动关闭的一方会进入TIME_WAIT
        // 状态（通常 1~4 分钟）
        // SO_REUSEADDR : 即使端口还有 TIME_WAIT 的连接，也允许我重新 bind
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("bind");
                close(server_fd);
        }

        listen(server_fd, 10);

        printf("listening on http://127.0.0.1:%d\n", port);

        while (1) {
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) {
                        perror("accept");
                        continue;
                }

                // 超时防止挂死
                struct timeval tv;
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

                int keep_alive = 1;
                int max_requests = 10;

                while (keep_alive && max_requests-- > 0) {
                        keep_alive = handle_one_request(client_fd);
                }

                close(client_fd);
        }
}
