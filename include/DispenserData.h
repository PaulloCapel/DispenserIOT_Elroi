#ifndef DISPENSER_DATA_H
#define DISPENSER_DATA_H

/*
    LIB     : DADOS DO DISPENSER
    DEV     : PAULO CAPEL
    REV     : 1.1
    DATA    : 11/12/2024

    DESCRIÇÃO


    1.0 - STRUCT DE DADOS A SER SALVO NA MEMORIA FLASH ATRAVES DO SPIFFS
    1.1 - STRUCT DE DADOS A SER SALVO NA MEMORIA FLASH ATRAVES DO LittleFS

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
#include <myDebug.h>
#include <LittleFS.h>

// Tamanho do buffer configurável
#define BUFFER_SIZE 512    // 512 equivale a 8.192 bytes de ocupação na memoria
#define BUFFER_API_SIZE 16 // 16 equivale a 256 bytes de ocupação

class DispenserData
{
public:
    //-----------------Definição dos structs envolvidos no processo-------------------//

#pragma pack(push, 1) // ajusta para que os structs definidos não possuam padding
    // struct do registro de dados 16 bytes por linha do array
    struct Registros
    {
        uint8_t hw_id[6];   // 6 bytes for mac address of the esp 32
        uint8_t free;       // 1 byte free for padding
        uint8_t event_id;   // 1 byte event
        uint32_t data;      // 4 bytes user_id or fluid level
        uint32_t timestamp; // 4 bytes (Unix Timestamp)
    };

    // struct de configuração de dados do buffer
    struct ConfigReg
    {
        uint32_t Assinatura;        // assinatura de inicialização do buffer
        int32_t IndiceAtual;       // Índice da próxima posição disponível        
        size_t IndiceMaisAntigo;    // Posição mais antiga no buffer (para sobrescrita)
        size_t ContagemDeDados;     // Quantidade de registros que contem no buffer
    };

    // struct de dados apenas dos registros para api tamanho é por defult 16
    struct DataTo_API
    {
        Registros data[BUFFER_API_SIZE];
    };

    // struct do arquivo /buffer_reg.bin
    struct BufferReg
    {
        ConfigReg cfg;
        Registros data[BUFFER_SIZE];
    };

       

    struct Str_LoraNode

        {
            uint16_t    Address;            // endereço do nó 
            uint8_t     Priority;           // pioridade no buffer ( 1 - 32 em função do preenchimento do buffer)  
            uint32_t    LastSync;           // ultima sincronização em unix

        };


    struct Str_LoraMasterCfg{

        uint8_t         Channel;                // canal de transmissão do bradcast        
        uint16_t        myAddress;              // endereço do nó
        Str_LoraNode SlaveNode[32];          // array de configurações nos dos da rede

    };

    struct StrNetworkCfgSlave{

        uint8_t     Mode;                   // 0 = não configurado || 1 = modo master || 2 = modo slave
        uint8_t     Channel;            // canal de transmissão do bradcast        
        uint16_t    MyAddress;          // endereço do nó 
        uint16_t    BackOff;            // BackOff da rede
        uint8_t     Priority;           // pioridade no buffer ( 1 -  32 em função do preenchimento do buffer )                                     


    };



    // struct do arquivo de configuração da rede do nó master

    struct Str_NetworkCfg
    {
        char Ssid[33];                  // ssid do wifi tamanho maximo 32 caracteres conforme padrão Wi-Fi (IEEE 802.11)
        char Pass[64];                  // senha do wifi maximo 63 caracteres
        uint8_t Mode;                   // 0 = não configurado || 1 = modo master || 2 = modo slave
        uint16_t SyncTime;              // tempo de sincronização server
        uint32_t LastUnixTime;          // ultimo valor recebido
        uint32_t Assinatura;            // assinatura de inicialização do buffer
        Str_LoraMasterCfg LoraMasterCfg;
        StrNetworkCfgSlave LoraSlaveCfg;
        

    };

   



#pragma pack(pop)

    

    //------------------------------------------------------------------------------------------//

    // função que recebe ponteiro da instance da função debug
    explicit DispenserData(myDebug &debugInstance) : debug(debugInstance) {}

    //-----------------funções gerais de inicialização e manipulação LittleFS-------------------//

    /* Função begin();
     * @brief    Função de inicialização do sistema de de arquivos
     *           Caso algum arquivo não exista ele já detecta e cria o arquivo novamente
     *
     * @return   Sucesso ou falha atribuida ao processo
     */

    bool begin();

    /* Função Format();
     * @brief  Formada o LittleFS
     *
     * @return   Sucesso ou falha atribuida ao processo
     */
    bool Format();

    /* Função CopyFlash_To_InternalRAM();
     * @brief  Recupera arquivo do Storage LittleFS para o Buffer interno BufferReg_RAM
     *
     * @return   Sucesso ou falha atribuida ao processo
     */

    bool CopyFlash_To_InternalRAM();

    /* Função       CopyInternalRam_To_Flash();
     * @brief       Função que salva os dados do buffer temporario da ram, na flash em caso de falta de energia
     *              Atribuir pino de interrupção para tratar esta função.
     */

    void CopyInternalRam_To_Flash(); //-------------------ELABORAÇÃO PENDENTE ---------------------//

    //------------------------------------------------------------------------------------------//

    //------------------------funções de manipulação dos dados do wifi--------------------------//

    // função que limpa o arquivo de configuração wifi
    bool Clear_NetworkCfg();

    /* Função Write_WifiDataDisp();
    * @brief    Escreve os dados de entrada da função do arquivo /cfg_wifi.bin
    * @param    _Ssid           Recebe até 32 caracteres
    * @param    _Pass           Recebe até 63 caracteres
    * @param    Mode            0 = não configurado
    *                           1 = modo master
    *                           2 = modo slave
    * @param    SyncTime        Intervalo entre as sincronização com API
    * @param    LastUnixTime    Ultimo valor recebido da API formato UnixTime
    *
    * @return   Sucesso ou falha atribuida ao processo

    */
    bool Write_NetworkCfg_To_Flash(const std::string &_Ssid, const std::string &_Pass, uint8_t _Mode, uint16_t _SyncTime, uint16_t _MasterAddress);

    /* Função Read_Cfg_Wifi();
     * @brief    Função le os dados no arquivo na flash, e escreve os dados lidos na memoria ram.
     *
     * @return   Escreve nas variaveis definidas na RAM Internal_NetworkCfg_RAM
     *
     */
    Str_NetworkCfg Read_NetworkCfg_From_Ram();

    //------------------------------------------------------------------------------------------//

    //------------------------funções de manipulação dos registros----------------------------//
    /* Função Clear_Registros();
     * @brief  Limpa os arquivos \reg.bin e \cfg_reg.bin
     *
     * @return   Sucesso ou falha atribuida ao processo
     */
    bool Clear_Registros_Flash();

    /* Função Read_ConfigBuffer();
     * @brief  Le o arquivo  \cfg_reg.bin e retorna o struct dos dados
     *
     * @return   // struct de configuração de dados do buffer
     *           struct ConfigReg
     *                {
     *                   size_t IndiceAtual;       // Índice da próxima posição disponível
     *                   size_t TamanhAtual;       // Tamanho atual do buffer
     *                   size_t IndiceMaisAntigo;  // Posição mais antiga no buffer (para sobrescrita)
     *                   bool Inicializado;        // que dados foram inicializados
     *                   bool BufferCircularAtivo; //
     *                };
     *
     */
    ConfigReg Read_ConfigBuffer_From_Ram();

    /* Função Put_Registros();
     * @brief  Coloca um registro no buffer da ram

     *
     * @return   Sucesso ou falha atribuida ao processo
     *
     */
    bool Put_Registros(Registros _Registros_To_Buffer);

    /* Função Put_Registros_From_API();
     * 
     * @brief  Função que devolve os dados para o buffer em caso de falha de envio para API
     *
     * @return   Sucesso ou falha atribuida ao processo
     *
     */
    bool Put_Registros_From_API(DataTo_API _Data_To_API);

    /* Função Get_Data_To_API();
     * @brief  Função que coleta os dados para envio da API.
     *         Toda vez que os dados são retirados, o buffer decrementa os dados retirados
     *         ou seja, caso o envio para API falhe, devemos considerar o retorno dos dados
     *         atravez da função Put_Registros_From_API()
     *
     *
     * @return   DataTo_API com array de 16 registros
     *
     */
    //DataTo_API Get_Data_To_API();
    DataTo_API Get_Data_To_API();

    //------------------------------------------------------------------------------------------//
    

private:

    const char *_filenameNetworkCfg = "/network_cfg.bin"; // arquivo de configurações master       
    const char *_filenameBuffer = "/buffer_reg.bin";

    myDebug &debug; // Referência ao objeto myDebug

    // variaveis compartilhadas entre fora e dentro da lib
    static BufferReg Internal_BufferReg_RAM;     // buffer armazenado como static
    static Str_NetworkCfg Internal_NetworkCfg_RAM; // dados do wifi armazenado como static
    static DataTo_API Internal_Data_To_API;

    uint32_t Ass_Storage = 0xAABBCCDD;
};

#endif
