#ifndef MAIN_H
#define MAIN_H

/*LIB ARDUINO*/
#include <Arduino.h>

#include <hardwareserial.h>
#include <time.h>

#include<WifiPortal.h> // lib dedicada ao portal de configurações.


/*LIB SALVAR ESCALA DE MEDICAO*/
#include <Preferences.h> // flash memory in esp32 better than EEPROM

/*LIB DE GERENCIADO FREERTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <esp_task_wdt.h>


/*LIB DE AUXILIARES*/

#include <0A41SK.h>
#include <RV1_Timer.h>

// LIB DO PN532

//Biblioteca -----------------------------------------------------------------------------------------

#include <Adafruit_PN532.h>
#include <Wire.h>

// lib de dados do dispenser

#include <DispenserData.h>

#include <myDebug.h>



/*

  Autor           : Paulo Capel
  Data            : 19/10/2024
  Descrição       : Dispenser IOT 
  Versão da placa : 1.1
  uC              : ESP32 WROOM-32
  SO              : FreeRTOS

  CORE 0  PRO_CPU_NUM - Faz funções inerentes ao controle do processo
  CORE 1  APP_CPU_NUM - Faz funções inerentes a comunicação e status da placa



*/

/*  
   arquitetura dos dados a ser transmitidos entre o master e a api

  struct Registro {
    uint8_t hw_id[6];        // 6 bytes para o MAC Address
    uint32_t user_id;        // 4 bytes
    uint8_t type_event;      // 1 byte 
    DEC(10)    ping teste // 
    DEC(11)    sincronicação npt //
    DEC(100)   envio de dados registro de higinização
    DEC(101)   envio de nivel de fliudo
    DEC(200)   envio de dados alarmes / avisos 
    uint8_t data_fluid;      // 1 byte 0 - 100 % da quantidade de fluido
    uint32_t timestamp;      // 4 bytes (Unix Timestamp)
  };

  arquitetura de troca de dados rede lora 

struct Registro {
    uint8_t head_init[4]     // 4 bytes inicio do cabeçalho 0xab - 0xcd 
    uint8_t hw_id[6];        // 6 bytes para o MAC Address
    uint32_t user_id;        // 4 bytes
    uint8_t type_event;      // 1 byte      
    uint8_t data_fluid;      // 1 byte 0 - 100 % da quantidade de fluido
    uint32_t timestamp;      // 4 bytes (Unix Timestamp)
    uint32_t crc32;          // 4 bytes 
    uint8_t head_end[4]      // 4 bytes final do cabeçalho
  };



*/




#endif