#ifndef Variaveis_h
#define Variaveis_h
#include <main.h>

// task handles

TaskHandle_t
    xTask_StatusLedHandle,
    xTask_ControlDispenserHandle,
    xTask_DataControl,
    xTask_SelectComunicationModeHandle,
    xTask_ModeMasterHandle,
    xTask_ModeSlaveHandle;

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

struct ApiStatus
{
    bool req;
    uint32_t timestamp;
};

bool AtualizaTime = false;

// structs do processo

DispenserData::Str_NetworkCfg App_NetworkConfig;
DispenserData::ConfigReg App_Cfg_Registros;
DispenserData::DataTo_API App_Registro_To_API;
DispenserData::Registros App_Registro_To_Buffer;

// eventos de processo 1 até 100

uint8_t EventID_NIVELDISPENSER = 10;            // medição do fluido do reservatorio
uint8_t EventID_IDOK_MAOS_OK = 11;              // detectou o cartão e detectou as maos
uint8_t EventID_IDOK_MAOS_NOK = 12;             // detectou o cartão mas houve timeout de aguardo das maos

// eventos de erro hardware 195 - 225

uint8_t EventID_HW_PN532_ERROR = 200;           // erro no modulo PN532 RFID
uint8_t EventID_HW_A0221AU_ERROR = 201;         // erro no modulo A0221AU MEDIDOR DE DISTANCIA
uint8_t EventID_HW_RADIOE32EBYTE_ERROR = 202;   // erro no modulo E32 91520D EBYTE

// eventos de erro software 226 - 255

uint8_t EventID_SW_SPIFFS_ERROR = 226; 









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