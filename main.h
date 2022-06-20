#pragma once
#include "circular_buffer.h"
#include "packet.h"

#include <stdint.h>

#define TMP_BUFFER_SIZE PACKET_MAX_SIZE

typedef uint16_t (*handler_t)(const unsigned char* data, uint16_t count);

typedef struct {
  unsigned char buffer[TMP_BUFFER_SIZE];
  uint16_t bytes_unprocessed;
  handler_t handler;
} Storage;

uint16_t ProcessNextPacket(CircularBuffer* cb,
                           Storage* storage,
                           uint16_t packet_size);

uint16_t ProcessCommands(const unsigned char* data, uint16_t count);
uint16_t ProcessData(const unsigned char* data, uint16_t count);
