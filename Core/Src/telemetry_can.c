#include "telemetry_can.h"
#include "telemetry_led.h"
#include "CAN26.h"

static CANPacket_t rxPacket;
static FDCAN_HandleTypeDef *canHandle = NULL;
static CANDevice_t telemetryDevice = {
    .peripheralDomain = 1,
    .powerDomain = 0,
    .motorDomain = 0,
    .deviceUUID = CAN_UUID_TELEMETRY
};

// Device representing the Jetson computer
static CANDevice_t jetson = {
    .peripheralDomain = 0,
    .powerDomain = 0,
    .motorDomain = 0,
    .deviceUUID = CAN_UUID_JETSON
};

// Packet used for transmitting CAN messages
static CANPacket_t txPacket;

// Firmware information
static const char *fwName = "tele";
static uint16_t revision = 1;

void TelemetryCAN_Init(FDCAN_HandleTypeDef *hfdcan)
{
	canHandle = hfdcan;
	CANInit(canHandle, &telemetryDevice);

}

void TelemetryCAN_Process(void)
{
	if(canHandle == NULL){
		return;
	}

	if(CANPollAndReceive(canHandle, &rxPacket)){

		TelemetryLEDs_OnCANReceive();

		uint8_t commandID = rxPacket.command & 0x7F;
		uint8_t ackReq = rxPacket.command & 0x80;

		bool failure = true;

		switch(commandID)
		{
			case CAN_COMMAND_ID__VERSION_GET:

				// sends if the acknowledge is received.
				if(ackReq){
					txPacket = CANUniversalPacket_Acknowledge(
					        telemetryDevice,
					        rxPacket.device,
					        failure,
					        rxPacket.command
					    );
					CANSend(canHandle, &txPacket);
				}

				break;

			default:
				break;

		}




	}
}

