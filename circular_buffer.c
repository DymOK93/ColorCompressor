#include "circular_buffer.h"

#include <string.h>

#include "stm32f0xx.h"

void CbInit(CircularBuffer* target,
            unsigned char* buffer,
            uint16_t bytes_count) {
  target->storage = buffer;
  target->capacity = bytes_count;
  target->head = 0;
  target->tail = 0;
}

static uint16_t CbpConsume(CircularBuffer* cb,
                           unsigned char* dst,
                           uint16_t bytes_count) {
  const uint16_t head = cb->head, tail = cb->tail;
  uint16_t delta, bytes_read;

  if (head == tail) {
    bytes_read = 0;
  } else if (head < tail) {
    bytes_read = MIN(bytes_count, tail - head);
    memcpy(dst, cb->storage + head, bytes_read);
    cb->head += bytes_read;
  } else if (CB_CAPACITY(cb) - head > bytes_count) {
    bytes_read = bytes_count;
    memcpy(dst, cb->storage + head, bytes_read);
    cb->head += bytes_read;
  } else {
    delta = CB_CAPACITY(cb) - head;
    memcpy(dst, cb->storage + head, delta);
    bytes_read = MIN(bytes_count - delta, tail);
    memcpy(dst + delta, cb->storage, bytes_read);
    cb->head = bytes_read;
    bytes_read += delta;
  }
  return bytes_read;
}

static uint16_t CbpProduce(CircularBuffer* cb,
                           const unsigned char* src,
                           uint16_t bytes_count) {
  const uint16_t head = cb->head, tail = cb->tail;
  uint16_t delta, bytes_written;

  if (tail < head) {
    bytes_written = MIN(bytes_count, head - tail);
    memcpy(cb->storage + tail, src, bytes_written);
    cb->tail += bytes_written;
  } else if (CB_CAPACITY(cb) - tail > bytes_count) {
    bytes_written = bytes_count;
    memcpy(cb->storage + tail, src, bytes_written);
    cb->tail += bytes_written;
  } else {
    delta = CB_CAPACITY(cb) - tail;
    memcpy(cb->storage + tail, src, delta);
    bytes_written = MIN(bytes_count - delta, head);
    memcpy(cb->storage, src + delta, bytes_written);
    cb->tail = bytes_written;
    bytes_written += delta;
  }
  return bytes_written;
}

uint16_t CbProduce(CircularBuffer* cb,
                   const unsigned char* src,
                   uint16_t bytes_count) {
  if (bytes_count == 0) {
    return 0;
  }
  return CbpProduce(cb, src, bytes_count);
}

uint16_t CbProduceOne(CircularBuffer* cb, unsigned char value) {
  return CbProduce(cb, &value, 1);
}

uint16_t CbConsume(CircularBuffer* cb,
                   unsigned char* dst,
                   uint16_t bytes_count) {
  if (bytes_count == 0) {
    return 0;
  }
  return CbpConsume(cb, dst, bytes_count);
}

uint16_t CbConsumeOne(CircularBuffer* cb, unsigned char* value) {
  return CbConsume(cb, value, 1);
}

void CbConsumeBlocking(CircularBuffer* cb,
                       unsigned char* dst,
                       uint16_t bytes_count) {
  while (bytes_count > 0) {
    bytes_count -= CbConsume(cb, dst, bytes_count);
  }
}
