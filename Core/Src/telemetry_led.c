#include "telemetry_led.h"
#include "main.h"

static uint32_t canFlashStart = 0;
static bool canFlashActive = false;

void TelemetryLEDs_SetError(bool hasError)
{
    HAL_GPIO_WritePin(
        GPIOB,
        ERR_LED_Pin,
        hasError ? GPIO_PIN_RESET : GPIO_PIN_SET
    );
}

void TelemetryLEDs_Init(void)
{
	canFlashActive = false;
	canFlashStart = HAL_GetTick();

    // Turn DDBG off (PB4)
	HAL_GPIO_WritePin(GPIOB, DBG_LED_Pin, GPIO_PIN_SET);

    // Turn DERR off (PB5)
	HAL_GPIO_WritePin(GPIOB, ERR_LED_Pin, GPIO_PIN_SET);

    // Turn DCAN off (PB11)
	HAL_GPIO_WritePin(GPIOB, CAN_LED_Pin, GPIO_PIN_SET);
}

void TelemetryLEDs_Update(void)
{
	// DCAN update
	static uint32_t lastHeartbeat = 0;
	static bool debugOn = false;

    uint32_t currentTime = HAL_GetTick();

    if (canFlashActive &&
        (uint32_t)(currentTime - canFlashStart) >= 50U)
    {
        HAL_GPIO_WritePin(GPIOB, CAN_LED_Pin, GPIO_PIN_SET);
        canFlashActive = false;
    }

    //DDBG Update
    if(canFlashActive && (uint32_t)(currentTime - lastHeartbeat) >= 500U){
    	debugOn = !debugOn;
    	TelemetryLEDs_SetDebug(debugOn);
    	lastHeartbeat = currentTime;
    }
}

void TelemetryLEDs_SetDebug(bool on)
{
    HAL_GPIO_WritePin(
        GPIOB,
        DBG_LED_Pin,
        on ? GPIO_PIN_RESET : GPIO_PIN_SET
    );
}

void TelemetryLEDs_OnCANReceive(void)
{
    // Turn the CAN LED ON (active-low)
    HAL_GPIO_WritePin(GPIOB, CAN_LED_Pin, GPIO_PIN_RESET);

    // Record when the CAN packet was received
    canFlashStart = HAL_GetTick();

    // Indicate that a CAN LED flash is active
    canFlashActive = true;
}
