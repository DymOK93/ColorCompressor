#pragma once
#include "circular_buffer.h"

#include <stdint.h>

uint16_t ProcessNextPacket(
	CircularBuffer* cb,
	unsigned char* buffer,
	uint16_t buffer_size,
	uint16_t bytes_unprocessed,
	uint8_t header);

uint16_t ProcessCommands(const uint8_t* data, uint16_t count);
uint16_t ProcessData(const unsigned char* data, uint16_t count);
