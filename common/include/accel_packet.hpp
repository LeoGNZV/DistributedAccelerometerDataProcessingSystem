#pragma once

#include <cstdint>

struct AccelPacket {
    int version = 1;

    int64_t timestamp;

    double x;
    double y;
    double z;
};
