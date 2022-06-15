#pragma once
#include <stdint.h>

void TrmInit(void);
void TrmSendData(const void* buffer, uint16_t bytes_count);
void TrmSendCommand(uint8_t command);
