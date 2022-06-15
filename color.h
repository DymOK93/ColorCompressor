#pragma once
#include <stdint.h>

typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} Bgr888;

typedef struct {
	uint16_t first;
	uint16_t second;
} Rgb666;

Rgb666 ColorCompress(Bgr888 bgr);
