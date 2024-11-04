#ifndef A041SK_H
#define A041SK_H

#include <Arduino.h>

class A041SK {
public:
    // Construtor que inicializa as variáveis Min, Max e Pin
    A041SK(uint16_t _Min, uint16_t _Max, uint8_t _Pin);

    // Função para configurar os valores de Min, Max e o pino (se necessário alterar depois)
    void Config(uint16_t _Min, uint16_t _Max, uint8_t _Pin);

    // Função para fazer a leitura
    bool Read();

private:
    uint16_t MaxADC;
    uint16_t Pin;
    uint16_t MaxScale;
    uint16_t Min;
    uint16_t Max;
    uint16_t Leitura;
};

#endif