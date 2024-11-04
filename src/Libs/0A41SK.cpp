#include <0A41SK.h>

/*
 função le a entrada comara com valores de 0 a 100
*/


// Construtor que inicializa as variáveis
A041SK::A041SK(uint16_t _Min, uint16_t _Max, uint8_t _Pin) 
    : Min(_Min), Max(_Max), Pin(_Pin), MaxADC(4095), MaxScale(100), Leitura(0) {
    // Configuração adicional, se necessário
}

// Função para configurar os valores (caso queira alterá-los após a criação do objeto)
void A041SK::Config(uint16_t _Min, uint16_t _Max, uint8_t _Pin) {
    Min = _Min;
    Max = _Max;
    Pin = _Pin;
}

// Função para fazer a leitura (exemplo simples, pode adaptar conforme sua necessidade)
bool A041SK::Read() {
    Leitura = map(analogRead(Pin),0,MaxADC,0,MaxScale); // Leitura do pino analógico    
    bool Value = false;
    
    // Aqui, você pode mapear a leitura de acordo com Min, Max e MaxScale
    // Por exemplo, para mapear a leitura de 0 a MaxScale:
     if ( Leitura >= Min && Leitura <= Max ){
           Value = true;       
        }
         
    return Value;
}


