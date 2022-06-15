#pragma once
#include <stdint.h>

#define PACKET_TYPE_DATA 0x1
#define PACKET_TYPE_COMMAND 0x2
#define PACKET_MIN_SIZE 1
#define PACKET_MAX_SIZE 64

#define PACKET_HEADER(type, size) ((type) << 6 | ((size) - 1))
#define PACKET_DATA(header) ((header) >> 6 == PACKET_TYPE_DATA)
#define PACKET_COMMAND(header)  ((header) >> 6 == PACKET_TYPE_DATA)
#define PACKET_SIZE(header) (((header) & 0x3F) + 1)
