#ifndef DISPENSER_DATA_H
#define DISPENSER_DATA_H

/*
    LIB     : DADOS DO DISPENSER
    DEV     : PAULO CAPEL
    REV     : 1
    DATA    : 11/12/2024

    DESCRIÇÃO 

    STRUCT DE DADOS A SER SALVO NA MEMORIA FLASH ATRAVES DO SPIFFS

    // struct do registro de dados 16 bytes por linha do array
    struct Registros 
    {
        uint8_t hw_id[6];       // 6 bytes for mac address of the esp 32
        uint8_t freepd;           // 1 byte free for padding  
        uint8_t event_id;       // 1 byte event
        uint32_t data;          // 4 bytes user_id or fluid level      
        uint32_t timestamp;     // 4 bytes (Unix Timestamp)
    };




*/

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <myDebug.h>

// Tamanho do buffer configurável
#define BUFFER_SIZE 512 // equivale a 6144 bytes de ocupação na memoria dinamica

class DispenserData
{
public:

    // struct do registro de dados 16 bytes por linha do array
    struct Registros 
    {
        uint8_t hw_id[6];       // 6 bytes for mac address of the esp 32
        uint8_t freepd;         // 1 byte free for padding  
        uint8_t event_id;       // 1 byte event
        uint32_t data;          // 4 bytes user_id or fluid level      
        uint32_t timestamp;     // 4 bytes (Unix Timestamp)
    };
    // struct de configuração de dados do buffer
    struct ConfigReg 
    {
        size_t IndiceAtual;       // Índice da próxima posição disponível
        size_t TamanhAtual;       // Tamanho atual do buffer
        size_t IndiceMaisAntigo;  // Posição mais antiga no buffer (para sobrescrita)
        bool Inicializado;        // que dados foram inicializados
        bool BufferCircularAtivo; //
    };

    // 
    struct WifiDataDisp 
    {
        char Ssid[64];          // ssid do wifi tamanho maximo 64 caracteres
        char Pass[32];          // senha do wifi maximo 32 caracteres      
        uint8_t Mode;           // 0 = não configurado || 1 = modo master || 2 = modo slave 
        uint16_t SyncTime;      // tempo de sincronização server      
        
        
    };
    //função que recebe ponteiro da instance da função debug
    explicit DispenserData(myDebug& debugInstance) : debug(debugInstance) {} 
    // inicialização da spiffs / inicialização de arquivos
    void begin();
    // função que coloca um novo dado no buffer
    void AddNovosDados(uint8_t _hw_id[6], uint8_t _freepd, uint8_t _event, uint32_t _dataUser,  uint32_t _timestamp);
    // função que coleta a informação mais antiga do buffer
    Registros RetNovosDados(); 
    // limpa dados do buffer / arquivos de configurações
    bool ClearAll();
    // coleta dados do arquivo de configuração do buffer.
    ConfigReg VerificaBuffer();

    // Adic
    void Put_WifiDataDisp(char Ssid[64],char Pass[32],uint8_t Mode,uint16_t SyncTime);
    // função que le os dados do wifi na memoria flash
    WifiDataDisp Get_WifiDataDisp();

private:

    const char *_filenameData = "/Registros.bin";        // Arquivo de dados
    const char *_filenameConfig = "/RegistroConfig.bin"; // Arquivo de configurações
    const char *_filenameWifiData = "/WifiDataDisp.bin"; // Arquivo de configurações
    myDebug& debug; // Referência ao objeto myDebug
    // função de inicialização de arquivos caso ele nao exista.
    bool inicializaArquivos();
};

#endif
