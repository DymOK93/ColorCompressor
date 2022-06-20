#pragma once
#include <stdint.h>

#define TRM_USART_SPEED 115200

void TrmInit(void);
void TrmSendData(const void* buffer, uint16_t bytes_count);
void TrmSendCommand(uint8_t command);
