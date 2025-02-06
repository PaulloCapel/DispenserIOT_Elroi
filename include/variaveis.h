#ifndef Variaveis_h
#define Variaveis_h
#include <main.h>

// task handles

TaskHandle_t
    xTask_StatusLedHandle,
    xTask_ControlDispenserHandle,
    xTask_DataControl,
    xTask_SelectComunicationModeHandle,
    xTask_ComunicationModeMasterHandle,
    xTask_CommunicationModeSlaveHandle;

EventGroupHandle_t
    xEventGroupStatusHandle;

SemaphoreHandle_t
    xMutexMyDataHandle;
//
QueueHandle_t
    xQueueAppToMydata,
    xQueueMyDataToServer;

struct CardRFID
{
    uint8_t succes;
    uint8_t uidlength;
    uint8_t uid_uint8_t[7];
    uint8_t uid_datablock4[16];
    uint32_t uid_uint32_t;
};

// event group bits

#define xEvG_ClockLedVM1Hz 0x01    // 0000 0001 // rename when fuction is defined
#define xEvG_ClockLedVD1Hz 0x02    // 0000 0010 // rename when fuction is defined
#define xEvG_ClockLedVM_VD1Hz 0x04 // 0000 0100 // rename when fuction is defined
#define xEvG_OnLedVM 0x08          // 0000 1000 // rename when fuction is defined
#define xEvG_OnLedVM_VD 0x10       // 0001 0000 // rename when fuction is defined
#define xEvG_OffLedVM_VD 0x20      // 0010 0000 // rename when fuction is defined
#define xEvG_OnLedEsp32 0x40       // 0100 0000 // rename when fuction is defined
#define xEvG_OffLedEsp32 0x80      // 1000 0000 // rename when fuction is defined

#endif