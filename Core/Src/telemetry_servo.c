#include "telemetry_servo.h"

#include <stddef.h>

static TIM_HandleTypeDef *servo_timer = NULL;

bool TelemetryServo_Init(TIM_HandleTypeDef *timer)
{
    if (timer == NULL || timer->Instance != TIM8) {
        return false;
    }

    if (timer->Init.Prescaler != 15u || timer->Init.Period != 19999u) {
        return false;
    }

    servo_timer = timer;
    return true;
}

bool TelemetryServo_SetAngle(float angle_degrees)
{

    static bool pwm_started = false;

    if (servo_timer == NULL ||
        !(angle_degrees >= 0.0f && angle_degrees <= 360.0f)) {
        return false;
    }

    float pulse_us = 1000.0f + (angle_degrees / 360.0f) * 1000.0f;
    uint32_t compare_ticks = (uint32_t)(pulse_us + 0.5f);

    __HAL_TIM_SET_COMPARE(servo_timer, TIM_CHANNEL_2, compare_ticks);

    if (!pwm_started) {
        pwm_started = HAL_TIM_PWM_Start(servo_timer, TIM_CHANNEL_2) == HAL_OK;
    }

    return pwm_started;
}
