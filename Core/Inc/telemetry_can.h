#ifndef TELEMETRY_CAN_H
#define TELEMETRY_CAN_H

#include "stm32g4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

// CAN communication
void TelemetryCAN_Init(FDCAN_HandleTypeDef *hfdcan);
void TelemetryCAN_Process(void);

// Limit-switch functions
bool TelemetryLimits_IsSwitch1Active(void);
bool TelemetryLimits_IsSwitch2Active(void);

void TelemetryLimits_Init(void);
void TelemetryLimits_Update(void);

// Limit-switch CAN reporting
bool TelemetryCAN_ReportLimitSwitch(uint8_t switchID, bool active);

#endif
