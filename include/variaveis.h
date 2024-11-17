#ifndef Variaveis_h
#define Variaveis_h
#include <main.h>

static const char *InitTag = "[SETUP]";

// task handles

TaskHandle_t    xTask_WifiConfigHandle,           // ONLY FIRST CONFIG, AND AFTER DELETE TASK
                xTask_PingTestHandle,             // PINGTEST || CALL 5000ms 
                xTask_SendDataTo_ServerHandle,    // BY EVENT  
                xTask_ReciveDataBy_ServerHandle,  // BY EVENT                             
                xTask_TankLevelHandle,            // 1000ms IF EVENT OK, BLOCK CALL 250ms  
                xTask_StatusLedHandle,            // CALL 500ms AND BY EVENT
                xTask_ControlDispenserHandle;     // CALL 250ms AND BY EVENT

// timer handles 

TimerHandle_t   xTimer_5SegHandle,                
                xTimer_1SegHandle,
                xTimer_500msHandle,
                xTimer_250msHandle;

// event group handle

EventGroupHandle_t xEventGroupStatusHandle;

// event group bits

#define xEvG_ClockLedVM1Hz           0x01  // 0000 0001 // rename when fuction is defined
#define xEvG_ClockLedVD1Hz           0x02  // 0000 0010 // rename when fuction is defined
#define xEvG_ClockLedVM_VD1Hz        0x04  // 0000 0100 // rename when fuction is defined
#define xEvG_OnLedVM                 0x08  // 0000 1000 // rename when fuction is defined
#define xEvG_OnLedVM_VD              0x10  // 0001 0000 // rename when fuction is defined
#define xEvG_OffLedVM_VD             0x20  // 0010 0000 // rename when fuction is defined
#define xEvG_OnLedEsp32              0x40  // 0100 0000 // rename when fuction is defined
#define xEvG_OffLedEsp32             0x80  // 1000 0000 // rename when fuction is defined

// variaveis da configuração wifi






 /*
CRIA TAREFAS
CORE 1
TASK 0 - ESP_APMODE || CALL 250ms
 
TASK1 -  PINGTEST || CALL 5000ms

TASK2 - INFO_STATUS_TO_SERVER || 
TASK3 - UPTADE_DATA_TO_ESP || BY EVENT
CORE 0
TASK 1 - NFC_CHECK || CALL 250ms AND BY EVENT
TASK 2 - TANK_LEVEL || CALL 1000ms IF EVENT OK, BLOCK CALL 250ms
TASK 3 - WAIT_HANDS || BY EVENTS
TASK 4 -PUMP_CONTROL || BY EVENTS
TASK 5 - STATUS_LED || CALL 500ms AND BY EVENT

*/



#endif