#pragma once
#include "circular_buffer.h"

#define RCV_FLAG_RETRY_IF_OVERWRITE 0x1

void RcvInit(void);
void RcvListen(CircularBuffer* cb, int flags);
