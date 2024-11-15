
#ifndef Pin_inOut_h
#define Pin_inOut_h

#include <main.h>

/*
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


uint8_t S_DetectorPin = 34;
uint8_t S_TemporizadorPin = 35;
uint8_t BombaPin = 26;
uint8_t LedVDPin = 25;
uint8_t LedVMPin = 27;

uint8_t A0221AU_RXPin = 33;
uint8_t A0221AU_TXPin = 32;

#endif