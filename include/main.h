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



/*

  Autor           : Paulo Capel
  Data            : 19/10/2024
  Descrição       : Dispenser IOT 
  Versão da placa : 1.1
  uC              : ESP32 WROOM-32
  SO              : FreeRTOS

  Descrição de pinos
  ----------------------------------------------------------------------------------------
  Led Vermelho  GPIO27
  ----------------------------------------------------------------------------------------
  Led Verde     GPIO25
  ----------------------------------------------------------------------------------------
  Sensor detector de maos 
  Modelo          : Sharp 0A41SK
  Entrada         : Analogica : 0 - 3,3V || 4 a 30cm || Não linear
  Interligação    : GPIO34
  ----------------------------------------------------------------------------------------
  Sensor de ajuste de tempo bomba
  Modelo          : Potenciometro
  Entrada         : Analogica : 0 - 2,47V || Ajuste de tempo há definir
  Interligação    : GPIO35
  ----------------------------------------------------------------------------------------
  Sensor de medição de liquido
  Modelo          : HC-SR04 
  Entrada         : Protocolo dedicado 
  Interligação    : GPIO32 ECHO || GPIO33 TRIG
  ---------------------------------------------------------------------------------------- 
  Sensor de medição de liquido
  Modelo          : A0221AU 
  Entrada         : Serial  
  Interligação    : GPIO32 RX SENSOR || GPIO33 TX SENSOR
  ----------------------------------------------------------------------------------------
  Leitor de Tag RFID // NFC
  Modelo          : PN532
  Entrada         : I2C
  Interligação    : SLC GPIO22 || SDA GPIO21

  Modelo          : PN532
  Entrada         : SPI
  Interligação    : SS GPIO5 || SCK GPIO18 || MISO GPIO19 || MOSI GPIO23
  ----------------------------------------------------------------------------------------



*/

#endif