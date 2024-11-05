#ifndef MAIN_H
#define MAIN_H

/*LIB ARDUINO*/
#include <Arduino.h>
#include <WiFi.h>
#include <hardwareserial.h>
/*LIB SALVAR ESCALA DE MEDICAO*/
#include <EEPROM.h>


/*LIB DE GERENCIADO FREERTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <esp_task_wdt.h>
#include <esp_log.h>
/*LIB DE AUXILIARES*/

#include <0A41SK.h>
#include <RV1_Timer.h>



#include <DNSServer.h>


/*

  Autor           : Paulo Capel
  Data            : 19/10/2024
  Descrição       : Dispenser IOT 
  Versão da placa : 1.1
  uC              : ESP32 WROOM-32
  SO              : FreeRTOS

  



*/

#endif