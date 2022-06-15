#include "circular_buffer.h"

#include <string.h>

void CbInit(CircularBuffer* target, unsigned char* buffer, uint16_t bytes_count)
{
	target->storage = buffer;
	target->capacity = bytes_count;
	target->head = 0;
	target->tail = 0;
	target->size = 0;
}

static uint16_t CbpConsume(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count)
{
	const uint16_t head = cb->head, tail = cb->tail;
	uint16_t bytes_read, delta, shift;

	if (tail > head) {
		delta = tail - head;
		bytes_read = delta < bytes_count ? delta : bytes_count;
		memcpy(dst, cb->storage + head, bytes_read);
		cb->head += bytes_read;
	}
	else
	{
		delta = cb->capacity - head;
		if (bytes_count < delta)
		{
			memcpy(dst, cb->storage + head, bytes_count);
			cb->head += bytes_count;
			bytes_read = bytes_count;
		}
		else {
			memcpy(dst, cb->storage + head, delta);
			bytes_count -= delta;
			shift = bytes_count > tail ? tail : bytes_count;
			memcpy(dst + delta, cb->storage, shift);
			cb->head = shift;
			bytes_read = delta + shift;
		}
	}
	cb->size -= bytes_read;
	return bytes_read;
}

static uint16_t CbpProduce(CircularBuffer* cb, const unsigned char* src, uint16_t bytes_count)
{
	const uint16_t head = cb->head, tail = cb->tail;
	uint16_t bytes_written, delta, shift;

	if (head > tail)
	{
		delta = head - tail;
		bytes_written = delta < bytes_count ? delta : bytes_count;
		memcpy(cb->storage + tail, src, bytes_written);
		cb->tail += bytes_written;
	} else
	{
		delta = cb->capacity - tail;
		if (bytes_count < delta)
		{
			memcpy(cb->storage + tail, src, bytes_count);
			cb->tail += bytes_count;
			bytes_written = bytes_count;
		} else
		{
			memcpy(cb->storage + tail, src, delta);
			bytes_count -= delta;
			shift = bytes_count > head ? head : bytes_count;
			memcpy(cb->storage, src + delta, shift);
			cb->tail = shift;
			bytes_written = delta + shift;
		}
	}
	cb->size += bytes_written;
	return bytes_written;
}

uint16_t CbProduce(CircularBuffer* cb, const unsigned char* src, uint16_t bytes_count)
{
	if (bytes_count == 0 || CB_FULL(cb))
	{
		return 0;
	}
	return CbpProduce(cb, src, bytes_count);
}

uint16_t CbProduceOne(CircularBuffer* cb, unsigned char value)
{
	return CbProduce(cb, &value, 1);
}

uint16_t CbConsume(CircularBuffer* cb, unsigned char* dst, uint16_t bytes_count)
{
	if (bytes_count == 0 || CB_EMPTY(cb))
	{
		return 0;
	}
	return CbpConsume(cb, dst, bytes_count);
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
