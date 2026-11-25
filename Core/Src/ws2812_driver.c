#include "ws2812_driver.h"

#include <stddef.h>

enum {
    BITS_PER_PIXEL = 24u,
    MIN_LATCH_SLOTS = 227u,
    DEFAULT_LATCH_SLOTS = 256u,
    BUFFER_SLOTS = 2u * DEFAULT_LATCH_SLOTS + 10u * BITS_PER_PIXEL,
};

static WS2812_Config active_config;
static bool initialized;
/* Watch this in the debugger. HAL_BUSY owns the buffer until a callback. */
static volatile HAL_StatusTypeDef transfer_status = HAL_ERROR;
_Alignas(uint32_t) static uint16_t dma_buffer[BUFFER_SLOTS];

static void set_data_pin_mode(uint32_t mode)
{
    GPIO_InitTypeDef gpio = {
        .Pin = GPIO_PIN_0,
        .Mode = mode,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_FREQ_MEDIUM,
        .Alternate = GPIO_AF2_TIM3,
    };
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_Init(GPIOB, &gpio);
}

bool WS2812_Init(const WS2812_Config *config)
{
    if (config == NULL || config->timer == NULL || transfer_status == HAL_BUSY ||
        config->timer->Instance != TIM3 || config->channel != TIM_CHANNEL_3 ||
        config->led_count == 0u) {
        return false;
    }

    TIM_HandleTypeDef *timer = config->timer;
    if (HAL_RCC_GetPCLK1Freq() != 16000000u ||
        (RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1 ||
        timer->Init.Prescaler != 0u || __HAL_TIM_GET_AUTORELOAD(timer) != 19u ||
        config->latch_slots < MIN_LATCH_SLOTS ||
        config->zero_high_ticks == 0u ||
        config->one_high_ticks <= config->zero_high_ticks ||
        config->one_high_ticks > __HAL_TIM_GET_AUTORELOAD(timer)) {
        return false;
    }

    DMA_HandleTypeDef *dma = timer->hdma[TIM_DMA_ID_CC3];
    if (dma == NULL || dma->Init.Request != DMA_REQUEST_TIM3_CH3 ||
        dma->Init.Mode != DMA_NORMAL ||
        dma->Init.Direction != DMA_MEMORY_TO_PERIPH ||
        dma->Init.MemDataAlignment != DMA_MDATAALIGN_HALFWORD ||
        dma->Init.PeriphDataAlignment != DMA_PDATAALIGN_HALFWORD ||
        dma->Init.MemInc != DMA_MINC_ENABLE || dma->Init.PeriphInc != DMA_PINC_DISABLE ||
        HAL_DMA_GetState(dma) != HAL_DMA_STATE_READY ||
        HAL_TIM_GetChannelState(timer, TIM_CHANNEL_3) != HAL_TIM_CHANNEL_STATE_READY) {
        return false;
    }

    const uint32_t frame_slots = BITS_PER_PIXEL * (uint32_t)config->led_count +
                                 2u * config->latch_slots;
    if (frame_slots > BUFFER_SLOTS || frame_slots > UINT16_MAX) {
        return false;
    }

    active_config = *config;
    set_data_pin_mode(GPIO_MODE_OUTPUT_PP);
    initialized = true;
    transfer_status = HAL_OK;
    return true;
}

static uint16_t build_buffer(WS2812_Colors color)
{
    uint16_t index = 0u;
    for (uint16_t slot = 0u; slot < active_config.latch_slots; ++slot) {
        dma_buffer[index++] = 0u;
    }

    for (uint16_t led = 0u; led < active_config.led_count; ++led) {
        for (int bit = 31; bit >= 8; --bit) {
            const uint32_t mask = 1u << bit;
            dma_buffer[index++] = (color.color & mask) ? active_config.one_high_ticks
                                                      : active_config.zero_high_ticks;
        }
    }

    for (uint16_t slot = 0u; slot < active_config.latch_slots; ++slot) {
        dma_buffer[index++] = 0u;
    }

    return index;
}

static void finish_transfer(HAL_StatusTypeDef result)
{
    HAL_TIM_PWM_Stop_DMA(active_config.timer, active_config.channel);
    set_data_pin_mode(GPIO_MODE_OUTPUT_PP);
    transfer_status = result;
}

static void WS2812_SetColor(WS2812_Colors color)
{
    if (!initialized || transfer_status == HAL_BUSY) {
        return;
    }

    TIM_HandleTypeDef *timer = active_config.timer;
    const uint16_t length = build_buffer(color);

    /* Load zero into both the active and preloaded compare registers. */
    __HAL_TIM_SET_COMPARE(timer, TIM_CHANNEL_3, 0u);
    timer->Instance->EGR = TIM_EGR_UG;
    __HAL_TIM_SET_COUNTER(timer, 0u);
    __HAL_TIM_CLEAR_FLAG(timer, TIM_FLAG_UPDATE | TIM_FLAG_CC3);
    set_data_pin_mode(GPIO_MODE_AF_PP);

    transfer_status = HAL_BUSY;
    __DMB();
    if (HAL_TIM_PWM_Start_DMA(timer, active_config.channel,
                              (const uint32_t *)dma_buffer, length) != HAL_OK) {
        finish_transfer(HAL_ERROR);
    }
}

void WS2812_SetRed(void)
{
    WS2812_SetColor((WS2812_Colors){ .color = RED });
}

void WS2812_SetGreen(void)
{
    WS2812_SetColor((WS2812_Colors){ .color = GREEN });
}

void WS2812_SetBlue(void)
{
    WS2812_SetColor((WS2812_Colors){ .color = BLUE });
}

void WS2812_Reset(void)
{
    WS2812_SetColor((WS2812_Colors){ .color = reset });
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *timer)
{
    if (transfer_status == HAL_BUSY &&
        timer == active_config.timer && timer->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
        /* The final latch slots already left PB0 low long enough to apply the frame. */
        finish_transfer(HAL_OK);
    }
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *timer)
{
    if (transfer_status == HAL_BUSY &&
        timer == active_config.timer && timer->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
        finish_transfer(HAL_ERROR);
    }
}
