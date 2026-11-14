#ifndef TELEMETRY_LEDS_H
#define TELEMETRY_LEDS_H

#include <stdbool.h>

void TelemetryLEDs_Init(void);
void TelemetryLEDs_Update(void);

void TelemetryLEDs_OnCANReceive(void);
void TelemetryLEDs_SetError(bool hasError);
void TelemetryLEDs_SetDebug(bool on);

#endif
