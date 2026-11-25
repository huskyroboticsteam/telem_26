
#include "telemetry_can.h"
#include "telemetry_led.h"
#include "CAN26.h"
#include "main.h"
#include "telemetry_servo.h"
#include "ws2812_driver.h"

#include <stdbool.h>
#include <stdint.h>

#define BRAKE_ID_BASE_ROTATION 0x01
#define BRAKE_ID_SHOULDER      0x02
#define BRAKE_ID_ELBOW         0x03

// Packet received from the CAN bus
static CANPacket_t rxPacket;

// Packet used for transmitting CAN messages
static CANPacket_t txPacket;

// Pointer to the STM32 FDCAN peripheral
static FDCAN_HandleTypeDef *canHandle = NULL;

static CANDevice_t telemetryDevice = {
    .peripheralDomain = 1,
    .powerDomain = 0,
    .motorDomain = 0,
    .deviceUUID = CAN_UUID_TELEMETRY
};

static CANDevice_t jetson = {
    .peripheralDomain = 0,
    .powerDomain = 0,
    .motorDomain = 0,
    .deviceUUID = CAN_UUID_JETSON
};

// firmware info
static const char *fwName = "tele";
static uint16_t revision = 1;

// limit switch initalize state
static bool previousSwitch1 = false;
static bool previousSwitch2 = false;



void TelemetryCAN_Init(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == NULL) {
        return;
    }

    canHandle = hfdcan;

    CANInit(canHandle, &telemetryDevice);
}


// Check for incoming CAN packets and handle commands
void TelemetryCAN_Process(void)
{

    if (canHandle == NULL) {
        return;
    }


    if (CANPollAndReceive(canHandle, &rxPacket)) {

        // flash can
        TelemetryLEDs_OnCANReceive();

        // isolate lower 7
        uint8_t commandID = rxPacket.command & 0x7F;

        // highest shows if ACK was requested
        uint8_t ackReq = rxPacket.command & 0x80;

        bool failure = true;

        switch (commandID)
        {
            case CAN_COMMAND_ID__VERSION_GET:
            {
                // Construct firmware version response
                txPacket = CANUniversalPacket_FirmwareVersion(
                    telemetryDevice,
                    jetson,
                    fwName,
                    revision
                );

                // Send response and record whether queuing failed
                failure = (CANSend(canHandle, &txPacket) != HAL_OK);

                break;
            }

            // Future commands go here:

            // Set servo angle for mast cam
            case CAN_COMMAND_ID__SERVO_ANGLE:
            {
            	if (rxPacket.contentsLength != 4u) {
            		failure = true;
            	    break;
            	}

            	CANPeripheralPacket_SetServoAngleDecoded_t servo =
            	        CANPeripheralPacket_SetServoAngle_Decode(&rxPacket);

            	if (servo.servo_id == 1u) {
            		failure = !TelemetryServo_SetAngle(servo.servo_angle * 360.0f);
            	}

            	break;
            }

            // Set led color for rover
            case CAN_COMMAND_ID__ROVER_LED_COLOR:
            {
                if (rxPacket.contentsLength != 3u) {
                    failure = true;
                    break;
                }

                CANPeripheralPacket_SetRoverLEDColor_Decoded_t rgb =
                    CANPeripheralPacket_SetRoverLEDColor_Decode(&rxPacket);

                if (rgb.red == 255u && rgb.green == 0u && rgb.blue == 0u) {
                    WS2812_SetRed();
                    failure = false;
                } else if (rgb.red == 0u && rgb.green == 255u && rgb.blue == 0u) {
                    WS2812_SetGreen();
                    failure = false;
                } else if (rgb.red == 0u && rgb.green == 0u && rgb.blue == 255u) {
                    WS2812_SetBlue();
                    failure = false;
                } else if (rgb.red == 0u && rgb.green == 0u && rgb.blue == 0u) {
                    WS2812_Reset();
                    failure = false;
                } else {
                    failure = true;
                }

                break;
            }


            // brake control
            case CAN_COMMAND_ID__SET_BRAKE_CONTROL:
            {
                CANPeripheralPacket_SetBrakeControl_Decoded_t brakePacket =
                    CANPeripheralPacket_SetBrakes_Decode(&rxPacket);

                // Accept only the two expected binary states
                if (brakePacket.state != 0 && brakePacket.state != 1) {
                    failure = true;
                    break;
                }

                GPIO_PinState pinState =
                    brakePacket.state ? GPIO_PIN_SET : GPIO_PIN_RESET;

                if (brakePacket.brake_id == BRAKE_ID_BASE_ROTATION) {

                    HAL_GPIO_WritePin(
                        BRAKE_BASE_GPIO_Port,
                        BRAKE_BASE_Pin,
                        pinState
                    );

                    failure = false;

                } else if (brakePacket.brake_id == BRAKE_ID_SHOULDER) {

                    HAL_GPIO_WritePin(
                        BRAKE_SHOULDER_GPIO_Port,
                        BRAKE_SHOULDER_Pin,
                        pinState
                    );

                    failure = false;

                } else if (brakePacket.brake_id == BRAKE_ID_ELBOW) {

                    HAL_GPIO_WritePin(
                        BRAKE_ELBOW_GPIO_Port,
                        BRAKE_ELBOW_Pin,
                        pinState
                    );

                    failure = false;

                } else {

                    failure = true;
                }

                break;
            }

            // CAN_COMMAND_ID__RESET


            default:
                break;
        }

        // Send an acknowledgment if the sender requested one
        if (ackReq) {

            txPacket = CANUniversalPacket_Acknowledge(
                telemetryDevice,
                rxPacket.device,
                failure,
                rxPacket.command
            );

            CANSend(canHandle, &txPacket);
        }
    }
}

bool TelemetryCAN_ReportLimitSwitch(uint8_t switchID, bool active)
{
    if (canHandle == NULL) {
        return false;
    }

    if (switchID != 1 && switchID != 2) {
        return false;
    }

    CANPacket_t packet = {
        .device = jetson,
        .priority = CAN_PRIORITY_HIGH,
        .contentsLength = 2,
        .command = CAN_COMMAND_ID__LIMIT_SWITCH_ALERT,
        .senderUUID = (CANDeviceUUID_t)telemetryDevice.deviceUUID,
        .contents = {
            switchID,
            active ? 1 : 0
        }
    };

    return CANSend(canHandle, &packet) == HAL_OK;
}

bool TelemetryLimits_IsSwitch1Active(void)
{
    return HAL_GPIO_ReadPin(
        LIMIT_SWITCH_1_GPIO_Port,
        LIMIT_SWITCH_1_Pin
    ) == GPIO_PIN_RESET;
}

bool TelemetryLimits_IsSwitch2Active(void)
{
    return HAL_GPIO_ReadPin(
        LIMIT_SWITCH_2_GPIO_Port,
        LIMIT_SWITCH_2_Pin
    ) == GPIO_PIN_RESET;
}

void TelemetryLimits_Init(void)
{
    previousSwitch1 = TelemetryLimits_IsSwitch1Active();
    previousSwitch2 = TelemetryLimits_IsSwitch2Active();
}

void TelemetryLimits_Update(void)
{
    bool switch1 = TelemetryLimits_IsSwitch1Active();
    bool switch2 = TelemetryLimits_IsSwitch2Active();

    if (switch1 != previousSwitch1) {
        if (TelemetryCAN_ReportLimitSwitch(1, switch1)) {
            previousSwitch1 = switch1;
        }
    }

    if (switch2 != previousSwitch2) {
        if (TelemetryCAN_ReportLimitSwitch(2, switch2)) {
            previousSwitch2 = switch2;
        }
    }
}
