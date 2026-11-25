#ifndef TELEMETRY_SERVO_H_
#define TELEMETRY_SERVO_H_

#include <stdbool.h>
#include "stm32g4xx_hal.h"

bool TelemetryServo_Init(TIM_HandleTypeDef *timer);
bool TelemetryServo_SetAngle(float angle_degrees);
#endif /* TELEMETRY_SERVO_H_ */
