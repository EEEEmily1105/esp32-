#pragma once

#include <stdint.h>

#define VOICE_BUSY 7
#define VOICE_DATA 9

void voice_Init(void);
void  voice_Transmit_Data(uint8_t Data);