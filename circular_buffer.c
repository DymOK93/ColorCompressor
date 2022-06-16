#include "circular_buffer.h"

#include <string.h>
#include <assert.h>

#include "stm32f0xx.h"

void CbInit(CircularBuffer* target, unsigned char* buffer, uint16_t bytes_count)
{
	target->storage = buffer;
	target->capacity = bytes_count;
	target->head = 0;
	target->tail = 0;
	target->size = 0;
}

static void CbpConsume(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count)
{
	const uint16_t head = cb->head, tail = cb->tail;
	uint16_t delta;

	if (head < tail || CB_CAPACITY(cb) - head > bytes_count) {
		memcpy(dst, cb->storage + head, bytes_count);
		cb->head += bytes_count;
	}
	else
	{
		delta = CB_CAPACITY(cb) - head;
		memcpy(dst, cb->storage + head, delta);
		bytes_count -= delta;
		memcpy(dst + delta, cb->storage, bytes_count);
		cb->head = bytes_count;
	}
}

static void CbpProduce(CircularBuffer* cb, const unsigned char* src, uint16_t bytes_count)
{
	const uint16_t head = cb->head, tail = cb->tail;
	uint16_t delta;

	if (tail < head || CB_CAPACITY(cb) - tail > bytes_count)
	{
		memcpy(cb->storage + tail, src, bytes_count);
		cb->tail += bytes_count;
	}
	else
	{
		delta = CB_CAPACITY(cb) - tail;
		memcpy(cb->storage + tail, src, delta);
		bytes_count -= delta;
		memcpy(cb->storage, src + delta, bytes_count);
		cb->tail = bytes_count;
	}
}

uint16_t CbProduce(CircularBuffer* cb, const unsigned char* src, uint16_t bytes_count)
{
	uint16_t bytes_written;

	if (bytes_count == 0 || CB_FULL(cb))
	{
		return 0;
	}
	bytes_written = MIN(bytes_count, CB_REMAINING(cb));
	cb->size += bytes_written;
	CbpProduce(cb, src, bytes_written);
	return bytes_written;
}

uint16_t CbProduceOne(CircularBuffer* cb, unsigned char value)
{
	return CbProduce(cb, &value, 1);
}

uint16_t CbConsume(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count)
{
	uint16_t bytes_read;

	if (bytes_count == 0 || CB_EMPTY(cb))
	{
		return 0;
	}
	bytes_read = MIN(bytes_count, CB_SIZE(cb));
	cb->size -= bytes_read;
	CbpConsume(cb, dst, bytes_read);
	return bytes_read;
}

uint16_t CbConsumeOne(CircularBuffer* cb, unsigned char* value)
{
	return CbConsume(cb, value, 1);
}

void CbConsumeBlocking(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count)
{
	while (CB_SIZE(cb) < bytes_count) {}
	CbConsume(cb, dst, bytes_count);
}
