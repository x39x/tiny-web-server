#include "router.h"
#include "response.h"
#include "util.h"

#include <string.h>

Response route(const char *path) {

        if (strcmp(path, "/") == 0) {
                return make_file_response("index.html", "text/html");
        }

        // 错误处理，防止访问父目录文件
        if (strlen(path) == 1 || strstr(path, "..")) {
                return make_text_response("404 Not Found", "text/plain",
                                          "not found\n");
        }

        // /file .ext -> file.ext
        const char *file = path + 1;
        return make_file_response(file, guess_type(file));
}
