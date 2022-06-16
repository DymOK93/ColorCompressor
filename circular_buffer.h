#pragma once
#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
	unsigned char* storage;
	uint16_t capacity;
	volatile uint16_t head;
	volatile uint16_t tail;
	volatile uint16_t size;
} CircularBuffer;

#define CB_SIZE(cb) ((cb)->size)
#define CB_CAPACITY(cb) ((cb)->capacity)
#define CB_FULL(cb) (CB_SIZE(cb) == CB_CAPACITY(cb))
#define CB_EMPTY(cb) (CB_SIZE(cb) == 0)
#define CB_REMAINING(cb) (CB_CAPACITY(cb) - CB_SIZE(cb))

void CbInit(CircularBuffer* target, unsigned char* buffer, uint16_t bytes_count);

uint16_t CbProduce(CircularBuffer* cb, const unsigned char* src, uint16_t bytes_count);
uint16_t CbProduceOne(CircularBuffer* cb, unsigned char value);

uint16_t CbConsume(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count);
uint16_t CbConsumeOne(CircularBuffer* cb, unsigned char* value);
void CbConsumeBlocking(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count);

