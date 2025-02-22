#include <DispenserData.h>

DispenserData::BufferReg DispenserData::Internal_BufferReg_RAM;
DispenserData::Str_NetworkCfg DispenserData::Internal_NetworkCfg_RAM;
DispenserData::DataTo_API DispenserData::Internal_Data_To_API;

//------------------------------------funções padrões dos arquivos---------------------------------------//

bool DispenserData::begin()
{

    // inicializa variavel internas da memoria ram

    memset(&Internal_BufferReg_RAM, 0, sizeof(BufferReg));
    memset(&Internal_NetworkCfg_RAM, 0, sizeof(Str_NetworkCfg));
    memset(&Internal_Data_To_API, 0, sizeof(DataTo_API));

    // inicializa LittleFS
    if (!LittleFS.begin(true))
    {
        debug.Println("DispenserData.Begin()", "Falha ao montar LittleFS", "ERROR");
        return false;
    }

    // Obter o espaço total e usado em bytes
    size_t total = LittleFS.totalBytes();
    size_t used = LittleFS.usedBytes();
    size_t free = total - used;

    debug.Println("DispenserData.Begin()","Espaço Total LittleFS: " + String(total) + " bytes", "INFO");
    debug.Println("DispenserData.Begin()","Espaço Usado LittleFS: " + String(used) + " bytes", "INFO");
    debug.Println("DispenserData.Begin()","Espaço Livre LittleFS: " + String(free) + " bytes", "INFO");

    // Verifica aquivos existentes
    File root = LittleFS.open("/"); // Abre o "diretório" onde estão os arquivos
    if (!root)                      // Se houver falha ao abrir o "diretório", ...
    {
        // informa ao usuário que houve falhas e sai da função retornando false.
        debug.Println("DispenserData.Begin()", "falha ao abrir o diretório LittleFS", "ERROR");
        return false;
    }

    File file = root.openNextFile(); // Relata o próximo arquivo do "diretório" e
    int qtdFiles = 0;                // variável que armazena a quantidade de arquivos que há no diretório informado.

    while (file)
    { // Enquanto houver arquivos no "diretório" que não foram vistos,
        debug.Print("DispenserData.Begin()", " FILE : ", "INFO");
        debug.Print("DispenserData.Begin()", String(file.name()), "INFO"); // Imprime o nome do arquivo
        debug.Print("DispenserData.Begin()", "\tSIZE : ", "INFO");
        debug.Println("DispenserData.Begin()", String(file.size()), "INFO"); // Imprime o tamanho do arquivo
        qtdFiles++;                                                          // Incrementa a variável de quantidade de arquivos
        file = root.openNextFile();                                          // Relata o próximo arquivo do diretório e
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    
    // faz a verificação se o arqquivo /buffer_reg.bin existe, ou se esta corrompido
    bool File_BufferRegExist = LittleFS.exists(_filenameBuffer);
    debug.Print("DispenserData.Begin()", "Arquivo /buffer_reg.bin existe ? ", "INFO");
    debug.Println("DispenserData.Begin()", File_BufferRegExist ? "sim" : "nao", "INFO");
    bool BackupSucess = false;
    // verifica se o arquivo se existe faz backup para ram
    if (File_BufferRegExist)
    {
        BackupSucess = CopyFlash_To_InternalRAM();
        debug.Print("DispenserData.Begin()", "Backup /buffer_reg.bin realizado ? ", "INFO");
        debug.Println("DispenserData.Begin()", BackupSucess ? "sim" : "nao", "INFO");
    }

    // caso o arquivo não exista ou esteja corrompido ele cria um novo
    if (!File_BufferRegExist || !BackupSucess)
    {
        debug.Println("DispenserData.Begin()", "Criando arquivo /buffer_reg.bin", "WARN");
        memset(&Internal_BufferReg_RAM, 0, sizeof(BufferReg));

        Internal_BufferReg_RAM.cfg.Assinatura = Ass_Storage; // assina o arquivo
        Internal_BufferReg_RAM.cfg.IndiceAtual = 0;       // inicializa buffer vazio (-1 proxima casa é 0 ao ser incrementado)
        Internal_BufferReg_RAM.cfg.ContagemDeDados = 0;
        Internal_BufferReg_RAM.cfg.IndiceMaisAntigo = 0;

        // 3. Cria e escreve arquivo de dados
        File BufferRegFile = LittleFS.open(_filenameBuffer, FILE_WRITE);
        if (!BufferRegFile)
        {
            debug.Println("DispenserData.Begin()", "Erro ao criar /buffer_reg.bin", "ERROR");
            BufferRegFile.close();
            return false;
        }

        size_t written_ = BufferRegFile.write(reinterpret_cast<const uint8_t *>(&Internal_BufferReg_RAM), sizeof(BufferReg));
        BufferRegFile.close();

        if (written_ != sizeof(BufferReg))
        {
            debug.Println("DispenserData.Begin()", "Escrita incompleta em /buffer_reg.bin", "ERROR");
            BufferRegFile.close();
            LittleFS.remove(_filenameBuffer);
            return false;
        }

        // le o arquivo escrito e coleta assinatura
        Internal_BufferReg_RAM.cfg.Assinatura = 0;
        File BufferRegFile_ = LittleFS.open(_filenameBuffer, FILE_READ);
        if (!BufferRegFile_)
        {
            debug.Println("DispenserData.Begin()", "Erro ao ler /buffer_reg.bin", "ERROR");
            BufferRegFile_.close();
            LittleFS.remove(_filenameBuffer);
            return false;
        }

        // Lê os dados do arquivo e os armazena na estrutura _WifiDataDisp
        BufferRegFile_.read(reinterpret_cast<uint8_t *>(&Internal_BufferReg_RAM), sizeof(BufferReg));
        // Fecha o arquivo após a leitura
        BufferRegFile_.close();
        String assinaturaHex = String(Internal_BufferReg_RAM.cfg.Assinatura, HEX);
        assinaturaHex.toUpperCase(); // Opcional: "FFABCDFF" em vez de "ffabcdff"
        debug.Println("DispenserData.Begin()", "Assinatura do arquivo /buffer_reg.bin : 0x" + assinaturaHex, "INFO");

        if (Internal_BufferReg_RAM.cfg.Assinatura != Ass_Storage)
        {
            debug.Println("DispenserData.Begin()", "A assinatura do arquivo lido, é diferente da assinatura interna", "ERROR");
            LittleFS.remove(_filenameBuffer);
            return false;
        }
    }

    // verifica se existe o arquivo de configuração wifi, se nao cria ele.
    bool File_WifiConfigExist = LittleFS.exists(_filenameNetworkCfg);
    debug.Print("DispenserData.Begin()", "Arquivo /network_cfg.bin existe ? ", "INFO");
    debug.Println("DispenserData.Begin()", File_WifiConfigExist ? "sim" : "nao", "INFO");
    bool WifiConfigFileCorrupt = false;

    // caso exista verifica se não está corrompido
    if (File_WifiConfigExist)
    {
        memset(&Internal_NetworkCfg_RAM, 0, sizeof(Str_NetworkCfg));

        File _WifiDataDispFile = LittleFS.open(_filenameNetworkCfg, FILE_READ);
        if (!_WifiDataDispFile)
        {
            debug.Println("DispenserData.Begin()", "Erro ao abrir arquivo /network_cfg.bin ", "ERROR");
            LittleFS.remove(_filenameNetworkCfg);
            WifiConfigFileCorrupt = true;
        }
        else
        {
            
            _WifiDataDispFile.read(reinterpret_cast<uint8_t *>(&Internal_NetworkCfg_RAM), sizeof(Str_NetworkCfg));
            _WifiDataDispFile.close();

            if (Internal_NetworkCfg_RAM.Assinatura != Ass_Storage)
            {
                debug.Println("DispenserData.Begin()", "Arquivo /network_cfg.bin corrompido, impossível fazer backup", "ERROR");
                LittleFS.remove(_filenameNetworkCfg);
                WifiConfigFileCorrupt = true;
            }
            debug.Print("DispenserData.Begin()", "Backup /network_cfg.bin realizado ? ", "INFO");
        debug.Println("DispenserData.Begin()", !WifiConfigFileCorrupt ? "sim" : "nao", "INFO");
        }
    }

    // cria o arquivo se necessário
    if (!File_WifiConfigExist || WifiConfigFileCorrupt)
    {
        debug.Println("DispenserData.Begin()", "Criando novo /network_cfg.bin", "WARN");

        
        Internal_NetworkCfg_RAM.Assinatura = Ass_Storage;

        File wifiFile = LittleFS.open(_filenameNetworkCfg, FILE_WRITE); // Alterado para modo "w"
        if (!wifiFile)
        {
            debug.Println("DispenserData.Begin()", "Erro ao criar /network_cfg.bin", "ERROR");
            return false;
        }

        
        size_t _written = wifiFile.write(reinterpret_cast<const uint8_t *>(&Internal_NetworkCfg_RAM), sizeof(Str_NetworkCfg));
        wifiFile.flush();
        wifiFile.close();

        debug.Println("DispenserData.Begin()", "Dados escritos: " + String(_written), "INFO");

        if (_written != sizeof(Str_NetworkCfg))
        {
            debug.Println("DispenserData.Begin()", "Escrita incompleta em /network_cfg.bin", "ERROR");
            LittleFS.remove(_filenameNetworkCfg);
            return false;
        }

        File _WifiDataDispFile = LittleFS.open(_filenameNetworkCfg, FILE_READ);
        if (!_WifiDataDispFile)
        {
            debug.Println("DispenserData.Begin()", "Erro ao abrir arquivo /network_cfg.bin ", "ERROR");
            LittleFS.remove(_filenameNetworkCfg);
            WifiConfigFileCorrupt = true;
        }
        else
        {
            
            _WifiDataDispFile.read(reinterpret_cast<uint8_t *>(&Internal_NetworkCfg_RAM), sizeof(Str_NetworkCfg));
            _WifiDataDispFile.close();

            debug.Println("DispenserData.Begin()", "Assinatura escrita WifiConfig : " + String(Internal_NetworkCfg_RAM.Assinatura), "ERROR");

            if (Internal_NetworkCfg_RAM.Assinatura != Ass_Storage)
            {
                debug.Println("DispenserData.Begin()", "Arquivo /network_cfg.bin corrompido, impossível fazer backup", "ERROR");
                Format();
                return false;
            }
        }

        debug.Println("DispenserData.Begin()", "Criado com sucesso /network_cfg.bin", "INFO");
    }
        /**/

    vTaskDelay(100);

    return true;
}

bool DispenserData::Format()
{

    debug.Println("FormatMyData()", "Formatando LittleFS", "INFO");
    LittleFS.format();
    return true;
}

bool DispenserData::CopyFlash_To_InternalRAM()
{

    File File_ = LittleFS.open(_filenameBuffer, FILE_READ);
    if (!File_)
    {
        debug.Println("CopyFlash_To_InternalRAM()", "Erro ao ler /buffer_reg.bin", "ERROR");
        return false;
    }

    // Lê os dados do arquivo e os armazena na estrutura _WifiDataDisp
    File_.read(reinterpret_cast<uint8_t *>(&Internal_BufferReg_RAM), sizeof(BufferReg));
    // Fecha o arquivo após a leitura
    File_.close();

    if (Internal_BufferReg_RAM.cfg.Assinatura != Ass_Storage)
    {
        debug.Println("CopyFlash_To_InternalRAM()", "Assinatura incorreta /buffer_reg.bin", "ERROR");
        memset(&Internal_BufferReg_RAM, 0, sizeof(BufferReg));
        return false;
    }

    return true;
}

bool DispenserData::Clear_Registros_Flash()
{
    debug.Println("Clear_Registros_Flash", "Limpando /buffer_reg.bin", "WARN");

    BufferReg *BufferReg_Empty = new BufferReg;
    // zera variaveis da alocação dinamica do buffer
    memset(BufferReg_Empty, 0, sizeof(BufferReg));
    memset(&Internal_BufferReg_RAM, 0, sizeof(BufferReg));

    // 3. Cria e escreve arquivo de dados
    File BufferRegFile = LittleFS.open(_filenameBuffer, FILE_WRITE);
    if (!BufferRegFile)
    {
        debug.Println("Clear_Registros_Flash", "Erro ao criar /buffer_reg.bin", "ERROR");
        LittleFS.remove(_filenameBuffer);
        delete BufferReg_Empty;
        return false;
    }

    size_t bufferSize = sizeof(BufferReg_Empty);
    size_t written_ = BufferRegFile.write(reinterpret_cast<const uint8_t *>(BufferReg_Empty), bufferSize);
    BufferRegFile.close();

    if (written_ != bufferSize)
    {
        debug.Println("Clear_Registros_Flash", "Escrita incompleta em /buffer_reg.bin", "ERROR");
        LittleFS.remove(_filenameBuffer);
        delete BufferReg_Empty;
        return false;
    }

    delete BufferReg_Empty;
    return true;
}

void DispenserData::CopyInternalRam_To_Flash(){
     // 3. Cria e escreve arquivo de dados
     File BufferRegFile = LittleFS.open(_filenameBuffer, FILE_WRITE);
     if (!BufferRegFile)
     {
         debug.Println("CopyInternalRam_To_Flash", "Erro ao criar /buffer_reg.bin", "ERROR");
         BufferRegFile.close();
         return;
         
     }

     size_t written_ = BufferRegFile.write(reinterpret_cast<const uint8_t *>(&Internal_BufferReg_RAM), sizeof(BufferReg));
     BufferRegFile.close();

     if (written_ != sizeof(BufferReg))
     {
         debug.Println("CopyInternalRam_To_Flash", "Escrita incompleta em /buffer_reg.bin", "ERROR");
         BufferRegFile.close();
         LittleFS.remove(_filenameBuffer);
         return;
        
     }
     debug.Println("CopyInternalRam_To_Flash", "Dadso salvo na flash com sucesso", "ERROR");
}


//-------------------------------------------------------------------------------------------------------//

//------------------------funções relacionadas a manipulação dos dados do wifi --------------------------//

bool DispenserData::Clear_NetworkCfg()
{
    // Registra um log indicando que o arquivo /network_cfg.bin está sendo limpo
    debug.Println("Clear_WifiConfig()", "Limpando arquivo /network_cfg.bin ", "WARN");

    memset(&Internal_NetworkCfg_RAM, 0, sizeof(Str_NetworkCfg));
    Internal_NetworkCfg_RAM.Mode = 0;
    Internal_NetworkCfg_RAM.Assinatura = Ass_Storage;

    // Tenta abrir o arquivo /network_cfg.bin para escrita no littlefs
    File _WifiDataDispFile = LittleFS.open(_filenameNetworkCfg, FILE_WRITE);

    // Verifica se o arquivo foi aberto com sucesso
    if (_WifiDataDispFile)
    {
        // Escreve a estrutura _WifiDataDisp no arquivo
        _WifiDataDispFile.write(reinterpret_cast<const uint8_t *>(&Internal_NetworkCfg_RAM), sizeof(Str_NetworkCfg));

        // Fecha o arquivo
        _WifiDataDispFile.close();

        // Registra um log indicando que o arquivo foi criado com sucesso
        debug.Println("Clear_WifiConfig()", "Arquivo /network_cfg.bin, criado com sucesso", "INFO");

        // Retorna true indicando que a operação foi bem-sucedida
        return true;
    }
    else
    {
        // Caso o arquivo não possa ser aberto, registra um log de erro
        debug.Println("Clear_WifiConfig()", "Erro ao abrir /network_cfg.bin, ou arquivo corrompido", "WARN");

        // Retorna false indicando que a operação falhou
        return false;
    }
}

bool DispenserData::Write_NetworkCfg_To_Flash(const std::string &_Ssid, const std::string &_Pass, uint8_t _Mode, uint16_t _SyncTime, uint16_t _MasterAddress)
{
    
    // Copia o SSID para a estrutura, garantindo que não ultrapasse o tamanho do buffer
    strncpy(Internal_NetworkCfg_RAM.Ssid, _Ssid.c_str(), sizeof(Internal_NetworkCfg_RAM.Ssid) - 1);
    Internal_NetworkCfg_RAM.Ssid[sizeof(Internal_NetworkCfg_RAM.Ssid) - 1] = '\0'; // Garante terminação correta

    // Copia a senha para a estrutura, garantindo que não ultrapasse o tamanho do buffer
    strncpy(Internal_NetworkCfg_RAM.Pass, _Pass.c_str(), sizeof(Internal_NetworkCfg_RAM.Pass) - 1);
    Internal_NetworkCfg_RAM.Pass[sizeof(Internal_NetworkCfg_RAM.Pass) - 1] = '\0';

    Internal_NetworkCfg_RAM.Mode = _Mode;
    Internal_NetworkCfg_RAM.SyncTime = _SyncTime;
    Internal_NetworkCfg_RAM.Assinatura = Ass_Storage;

        File wifiFile = LittleFS.open(_filenameNetworkCfg, FILE_WRITE); // Alterado para modo "w"
        if (!wifiFile)
        {
            debug.Println("Write_NetworkCfg_To_Flash", "Erro ao criar /cfg_wifi.bin", "ERROR");
            return false;
        }

        
        size_t _written = wifiFile.write(reinterpret_cast<const uint8_t *>(&Internal_NetworkCfg_RAM), sizeof(Str_NetworkCfg));
        wifiFile.flush();
        wifiFile.close();

        debug.Println("Write_NetworkCfg_To_Flash", "Dados escritos: " + String(_written), "INFO");

        if (_written != sizeof(Str_NetworkCfg))
        {
            debug.Println("Write_NetworkCfg_To_Flash", "Escrita incompleta em /cfg_wifi.bin", "ERROR");
            LittleFS.remove(_filenameNetworkCfg);
            return false;
        }
 return true;
}

DispenserData::Str_NetworkCfg DispenserData::Read_NetworkCfg_From_Ram()
{
    return Internal_NetworkCfg_RAM;
}

//-------------------------------------------------------------------------------------------------------//

//----------funções relacionadas a manipulação dos dados dos registros na memoria ram--------------------//

DispenserData::ConfigReg DispenserData::Read_ConfigBuffer_From_Ram()
{

    return Internal_BufferReg_RAM.cfg;
}

// Função para adicionar registros ao buffer
bool DispenserData::Put_Registros(Registros _Registros_To_Buffer)
{
    // Se o buffer está cheio (modo circular)
    if (Internal_BufferReg_RAM.cfg.ContagemDeDados == BUFFER_SIZE)
    {
        // Sobrescreve o registro mais antigo
        Internal_BufferReg_RAM.cfg.IndiceMaisAntigo =
            (Internal_BufferReg_RAM.cfg.IndiceMaisAntigo + 1) % BUFFER_SIZE;
    }
    else
    {
        // Atualiza contagem se não está cheio
        Internal_BufferReg_RAM.cfg.ContagemDeDados++;
    }

    // Insere o novo registro // ISSO AQUI ABAIXO PASSA PARA TEPOIS DO SOMATORIO, ABAIXO.
    Internal_BufferReg_RAM.data[Internal_BufferReg_RAM.cfg.IndiceAtual] = _Registros_To_Buffer;

    // Atualiza índice próximo com wrapping
    Internal_BufferReg_RAM.cfg.IndiceAtual =(Internal_BufferReg_RAM.cfg.IndiceAtual + 1) % BUFFER_SIZE;

    return true;
}

DispenserData::DataTo_API DispenserData::Get_Data_To_API()
{

    if (Internal_BufferReg_RAM.cfg.ContagemDeDados > BUFFER_API_SIZE)
    {
        memset(&Internal_Data_To_API, 0, sizeof(DispenserData::DataTo_API));
        size_t elementos_para_coletar = 0;

        // Determina quantos elementos podemos coletar (máximo BUFFER_API_SIZE)
        elementos_para_coletar = (Internal_BufferReg_RAM.cfg.ContagemDeDados > BUFFER_API_SIZE)
                                     ? BUFFER_API_SIZE
                                     : Internal_BufferReg_RAM.cfg.ContagemDeDados;

        // Copia os registros mais antigos para o buffer da API
        for (size_t i = 0; i < elementos_para_coletar; i++)
        {
            size_t current_idx = (Internal_BufferReg_RAM.cfg.IndiceMaisAntigo + i) % BUFFER_SIZE;
            Internal_Data_To_API.data[i] = Internal_BufferReg_RAM.data[current_idx];
        }

        // Atualiza buffer após coleta
        if (elementos_para_coletar > 0)
        {
            // Atualiza índice mais antigo
            Internal_BufferReg_RAM.cfg.IndiceMaisAntigo =
                (Internal_BufferReg_RAM.cfg.IndiceMaisAntigo + elementos_para_coletar) % BUFFER_SIZE;

            // Atualiza contagem
            Internal_BufferReg_RAM.cfg.ContagemDeDados -= elementos_para_coletar;
        }

        return Internal_Data_To_API;
    }
    else
    {
        return {};
    }
}

bool DispenserData::Put_Registros_From_API(DataTo_API _Data_To_API)
{
    // Copia os dados para a variável estática (evita destruição após a função)
    Internal_Data_To_API = _Data_To_API;

    // Verifica se há espaço suficiente OU se o buffer está em modo circular
    if (Internal_BufferReg_RAM.cfg.ContagemDeDados + BUFFER_API_SIZE > BUFFER_SIZE)
    {
        // Calcula quantos elementos serão sobrescritos
        size_t elementos_a_sobrescrever = Internal_BufferReg_RAM.cfg.ContagemDeDados + BUFFER_API_SIZE - BUFFER_SIZE;

        // Atualiza o índice mais antigo (remove os elementos mais velhos que serão sobrescritos)
        Internal_BufferReg_RAM.cfg.IndiceMaisAntigo =
            (Internal_BufferReg_RAM.cfg.IndiceMaisAntigo + elementos_a_sobrescrever) % BUFFER_SIZE;

        // Atualiza a contagem (buffer permanecerá cheio)
        Internal_BufferReg_RAM.cfg.ContagemDeDados = BUFFER_SIZE;
    }
    else
    {
        // Atualiza a contagem normalmente
        Internal_BufferReg_RAM.cfg.ContagemDeDados += BUFFER_API_SIZE;
    }

    // Insere os 16 registros no buffer
    for (size_t i = 0; i < BUFFER_API_SIZE; i++)
    {
        Internal_BufferReg_RAM.data[Internal_BufferReg_RAM.cfg.IndiceAtual] = Internal_Data_To_API.data[i];
        Internal_BufferReg_RAM.cfg.IndiceAtual =
            (Internal_BufferReg_RAM.cfg.IndiceAtual + 1) % BUFFER_SIZE;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------//
