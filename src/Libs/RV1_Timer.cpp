#include <RV1_Timer.h>

// Construtor que inicializa as variáveis
Rv_Timer::Rv_Timer (uint16_t _OutMin, uint16_t _OutMax, uint8_t _Pin)
    :OutMin(_OutMin),OutMax(_OutMax),Pin(_Pin),MaxADC(4095) {
}


uint16_t Rv_Timer::ReadTimer(){
    Leitura = 0;
    Leitura = map(analogRead(Pin),0,MaxADC,OutMin,OutMax); // escalona valor de adc em bit para volt base 100 330 equivale 3,3v





    return Leitura;
};