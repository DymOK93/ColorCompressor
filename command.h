#pragma once
#include <stdint.h>

#define CMD_GREEN_LED_ON 0x1
#define CMD_GREEN_LED_OFF 0x2
#define CMD_GREEN_LED_TOGGLE (CMD_GREEN_LED_ON | CMD_GREEN_LED_OFF)
#define CMD_BLUE_LED_ON 0x4
#define CMD_BLUE_LED_OFF 0x8
#define CMD_BLUE_LED_TOGGLE (CMD_BLUE_LED_ON | CMD_BLUE_LED_OFF)

#define CMD_NEXT_PICTURE 0x80

void CmdInit(void);
void CmdExecute(uint8_t command);
void CmdListen(void);
