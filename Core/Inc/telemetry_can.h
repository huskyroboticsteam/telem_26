#ifndef TELEMETRY_CAN_H
#define TELEMETRY_CAN_H

#include "stm32g4xx_hal.h"

void TelemetryCAN_Init(FDCAN_HandleTypeDef *hfdcan);
void TelemetryCAN_Process(void);

#endif
