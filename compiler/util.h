#pragma once

#include <cstdint>

namespace Util {
    int ReadFile(char const*, uint8_t **);
    char* CopyCString(const char *in);
}
