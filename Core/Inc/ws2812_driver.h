#ifndef WS2812_DRIVER_H_
#define WS2812_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"

typedef struct {
    TIM_HandleTypeDef *timer;  // Initialized TIM3 handle
    uint32_t channel;         // TIM_CHANNEL_3, routed to PB0
    uint16_t led_count;
    uint16_t zero_high_ticks;
    uint16_t one_high_ticks;
    uint16_t latch_slots;     // Low PWM periods before and after the frame
} WS2812_Config;

typedef struct {
    uint32_t color;           // Packed as 0xGGRRBB00
} WS2812_Colors;

#define RED   0x00FF0000u
#define GREEN 0xFF000000u
#define BLUE  0x0000FF00u
#define reset 0x00000000u

/* Call after CubeMX initializes GPIO, DMA, and TIM3. */
bool WS2812_Init(const WS2812_Config *config);

/* Call from the main loop with interrupts enabled. Sends asynchronously.
 * A call while DMA is busy leaves the active frame intact.
 */
void WS2812_SetRed(void);
void WS2812_SetGreen(void);
void WS2812_SetBlue(void);
void WS2812_Reset(void);

#endif /* WS2812_DRIVER_H_ */

