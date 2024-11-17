#ifndef MAIN_H
#define MAIN_H

/*LIB ARDUINO*/
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <hardwareserial.h>
#include <time.h>


/*LIB SALVAR ESCALA DE MEDICAO*/
#include <Preferences.h> // flash memory in esp32 better than EEPROM

/*LIB DE GERENCIADO FREERTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <esp_task_wdt.h>

#include <esp_log.h> // native log on esp 32
/*LIB DE AUXILIARES*/

#include <0A41SK.h>
#include <RV1_Timer.h>

// LIB DO PN532

//Biblioteca -----------------------------------------------------------------------------------------

#include <Adafruit_PN532.h>
#include <Wire.h>




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




#endif