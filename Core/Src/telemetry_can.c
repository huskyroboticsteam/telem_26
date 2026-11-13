#include "telemetry_can.h"
#include "CAN26.h"

static CANPacket_t rxPacket;
static FDCAN_HandleTypeDef *canHandle = NULL;
static CANDevice_t telemetryDevice = {
    .peripheralDomain = 1,
    .powerDomain = 0,
    .motorDomain = 0,
    .deviceUUID = CAN_UUID_TELEMETRY
};

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

	if(CANPollAndRecieve(canHandle, &rxPacket)){

	}
}
