#ifndef RV_timer_H
#define RV_timer_H

#include <Arduino.h>

class Rv_Timer {
public:
    // Construtor que inicializa as variáveis Min, Max e Pin
    Rv_Timer(uint16_t _OutMin, uint16_t _OutMax, uint8_t _Pin);
 

    // Função para fazer a leitura
    uint16_t ReadTimer();

private:
    uint16_t MaxADC;
    uint16_t Pin;    
    uint16_t OutMin, OutMax;    
    uint16_t Leitura;
};

#endif