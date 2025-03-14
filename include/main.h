#ifndef MAIN_H
#define MAIN_H

/*

  Autor           : Paulo Capel
  Data            : 19/10/2024
  Descrição       : Dispenser IOT
  Versão da placa : 1.1
  uC              : ESP32 WROOM-32
  SO              : FreeRTOS

  CORE 0  PRO_CPU_NUM - Faz funções inerentes comunicação e status da placa
  CORE 1  APP_CPU_NUM - Faz funções inerentes controle / dados


*/

/*LIB ARDUINO*/
#include <Arduino.h>
#include <WiFi.h>
#include <hardwareserial.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Ping.h>

#include <time.h>
#include <sys/time.h>

/*LIB DE GERENCIADO FREERTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <esp_task_wdt.h>

// LIB DO PN532

#include <Adafruit_PN532.h>
#include <Wire.h>

// BIBLIOTECAS ESPECIFICAS

#include <DispenserData.h>
#include <myDebug.h>
#include <WifiPortal.h> // lib dedicada ao portal de configurações.

/*LIB DE AUXILIARES*/

#include <0A41SK.h>
#include <RV1_Timer.h>

// PROTOTIPOS DE FUNÇÃO RTOS

void xTask_StatusLed(void *pvParameters);
void xTask_ControlDispenser(void *pvParameters);

void xTask_SelectComunicationMode(void *pvParameters);
void xTask_ModeMaster(void *pvParameters);
void xTask_ModeSlave(void *pvParameters);

/*
   arquitetura dos dados a ser transmitidos entre o master e a api

    // struct do registro de dados 16 bytes por linha do array
    struct Registros
    {
        uint8_t hw_id[6];       // 6 bytes for mac address of the esp 32
        uint8_t freepd;         // 1 byte free for padding
        uint8_t event_id;       // 1 byte event
        uint32_t data;          // 4 bytes user_id or fluid level
        uint32_t timestamp;     // 4 bytes (Unix Timestamp)
    };

  arquitetura de troca de dados rede lora

    struct Registro 
    {
        uint8_t head_init[2]     // 2 bytes inicio do cabeçalho 0xab 
        uint8_t hw_id[6];        // 6 bytes para o MAC Address
        uint32_t user_id;        // 4 bytes
        uint8_t type_event;      // 1 byte      
        uint32_t timestamp;      // 4 bytes (Unix Timestamp)
        uint32_t crc32;          // 4 bytes
        uint8_t head_end[2]      // 4 bytes final do cabeçalho - 0xcd
    };



*/

#endif