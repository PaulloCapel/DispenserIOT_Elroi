#include <DispenserData.h>

void DispenserData::begin()
{

    if (!SPIFFS.begin(true))
    {
        debug.Println("DispenserData.Begin()", "Erro: Falha ao inicializar o SPIFFS", "ERROR");
        return;
    }

    File root = SPIFFS.open("/"); // Abre o "diretório" onde estão os arquivos na SPIFFS
    if (!root)                    // Se houver falha ao abrir o "diretório", ...
    {
        // informa ao usuário que houve falhas e sai da função retornando false.
        debug.Println("DispenserData.Begin()", "falha ao abrir o diretório /data", "ERROR");
    }
    File file = root.openNextFile(); // Relata o próximo arquivo do "diretório" e

    int qtdFiles = 0; // variável que armazena a quantidade de arquivos que há no diretório informado.

    while (file)
    { // Enquanto houver arquivos no "diretório" que não foram vistos,
        debug.Print("DispenserData.Begin()", "  FILE : ", "INFO");
        debug.Print("DispenserData.Begin()", String(file.name()), "INFO"); // Imprime o nome do arquivo
        debug.Print("DispenserData.Begin()", "\tSIZE : ", "INFO");
        debug.Println("DispenserData.Begin()", String(file.size()), "INFO"); // Imprime o tamanho do arquivo
        qtdFiles++;                                                          // Incrementa a variável de quantidade de arquivos
        file = root.openNextFile();                                          // Relata o próximo arquivo do diretório e
    }
    bool check_init = false;
    if (qtdFiles == 0) // Se após a visualização de todos os arquivos do diretório
                       //                      não houver algum arquivo, ...
    {
        // Avisa o usuário que não houve nenhum arquivo para ler e retorna false.
        debug.Println("DispenserData.Begin()", "Nenhum arquivo encontrado", "WARN");
        check_init = inicializaArquivos(); // chama função de inicialização de arquivos
        debug.Print("DispenserData.Begin()", "Arquivos inicializados ? ", "INFO");
        debug.Println("DispenserData.Begin()", check_init ? "Sim" : "Nao", "INFO");
    }

    debug.Println("DispenserData.Begin()", "SPIFFS Inicializado", "INFO");
}

bool DispenserData::inicializaArquivos()
{
    // Inicializa ConfigReg
    bool ConfigRegExist = SPIFFS.exists(_filenameConfig);
    debug.Print("DispenserData.Begin()", "Arquivo /RegistroConfig.bin existe ? ", "INFO");
    debug.Println("DispenserData.Begin()", ConfigRegExist ? "true" : "false", "INFO");

    if (!ConfigRegExist)
    {

        ConfigReg config = {0, 0, 0, true, false}; // inicializa arquivo de configuração
        File configFile = SPIFFS.open(_filenameConfig, FILE_WRITE);
        if (configFile)
        {
            configFile.write(reinterpret_cast<const uint8_t *>(&config), sizeof(config));
            configFile.close();
            debug.Println("DispenserData.Begin()", "Arquivo /RegistroConfig.bin, criado com sucesso", "INFO");
        }
        else
        {
            debug.Println("DispenserData.Begin()", "Erro ao abrir /RegistroConfig.bin, ou arquivo corrompido", "WARN");
            return false;
        }

        // Inicializa BufferRegistros
        Registros *BufferRegistros = new Registros[BUFFER_SIZE]; // Usando memória dinâmica
        memset(BufferRegistros, 0, sizeof(BufferRegistros));

        File dataFile = SPIFFS.open(_filenameData, FILE_WRITE);
        if (dataFile)
        {
            dataFile.write(reinterpret_cast<const uint8_t *>(BufferRegistros), sizeof(BufferRegistros));
            dataFile.close();
            debug.Println("DispenserData.Begin()", "Arquivo /Registros.bin, criado com sucesso", "INFO");
            delete[] BufferRegistros; // Liberar memória antes de retornar
        }
        else
        {
            debug.Println("DispenserData.Begin()", "Erro ao criar arquivo /Registros.bin ", "WARN");
            delete[] BufferRegistros; // Liberar memória antes de retornar
            return false;
        }

        debug.Println("DispenserData.Begin()", "=== Configuração Atual ===", "INFO");
        debug.Println("DispenserData.Begin()", "Indice Atual: " + String(config.IndiceAtual), "INFO");
        debug.Println("DispenserData.Begin()", "Indice Mais Antigo: " + String(config.IndiceMaisAntigo), "INFO");
        debug.Print("DispenserData.Begin()", "Buffer Circular Ativo: ", "INFO");
        debug.Println("DispenserData.Begin()", config.BufferCircularAtivo ? "Sim" : "Não", "INFO");
        debug.Print("DispenserData.Begin()", "Dados de configuração inicializado ? ", "INFO");
        debug.Println("DispenserData.Begin()", config.Inicializado ? "Sim" : "Não", "INFO");
        debug.Println("DispenserData.Begin()", "==========================", "INFO");
    }

    // verifica se o arquivo de configurações do wifi existe
    bool WifiConfigExist = SPIFFS.exists(_filenameWifiData);
    debug.Print("DispenserData.Begin()", "Arquivo /WifiDataDisp.bin existe ? ", "INFO");
    debug.Println("DispenserData.Begin()", WifiConfigExist ? "true" : "false", "INFO");
    if (!WifiConfigExist)
    {
        WifiDataDisp _WifiDataDisp;
        memset(&_WifiDataDisp, 0, sizeof(WifiDataDisp));
        _WifiDataDisp.Mode = 0; // inicializa em 0 demonstrando que o portal não foi configurado

        File _WifiDataDispFile = SPIFFS.open(_filenameWifiData, FILE_WRITE);
        if (_WifiDataDispFile)
        {
            _WifiDataDispFile.write(reinterpret_cast<const uint8_t *>(&_WifiDataDisp), sizeof(_WifiDataDisp));
            _WifiDataDispFile.close();
            debug.Println("DispenserData.Begin()", "Arquivo /WifiDataDisp.bin, criado com sucesso", "INFO");
        }
        else
        {
            debug.Println("DispenserData.Begin()", "Erro ao abrir /WifiDataDisp.bin, ou arquivo corrompido", "WARN");
            return false;
        }
    }

    return true;
}

void DispenserData::Put_NovoRegistro(uint8_t _hw_id[6], uint8_t _freepd, uint8_t _event, uint32_t _dataUser, uint32_t _timestamp)
{
    debug.Println("DispenserData.PutNovaPos()", "Adicionando novos dados na fila", "WARN");
    // Lê o arquivo de configuração para obter o índice atual
    ConfigReg config;
    File configFile = SPIFFS.open(_filenameConfig, FILE_READ);
    if (!configFile)
    {
        debug.Println("DispenserData.PutNovaPos()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
        return;
    }
    else
    {
        configFile.read(reinterpret_cast<uint8_t *>(&config), sizeof(ConfigReg));
        configFile.close();
    }

    // Aloca memória dinamicamente para o buffer de registros
    Registros *BufferRegistros = new Registros[BUFFER_SIZE]; // Usando memória dinâmica
    // zera variaveis da alocação dinamica do buffer
    memset(BufferRegistros, 0, sizeof(Registros) * BUFFER_SIZE);
    // abre aquivo de dados
    File dataFile = SPIFFS.open(_filenameData, FILE_READ);
    if (dataFile)
    {
        dataFile.read(reinterpret_cast<uint8_t *>(BufferRegistros), sizeof(Registros) * BUFFER_SIZE);
        dataFile.close();
    }
    else
    {
        debug.Println("DispenserData.PutNovaPos()", "Erro ao abrir /Registros.bin ", "WARN");
        delete[] BufferRegistros; // Liberar memória antes de retornar
        return;
    }
    delay(10);
    // Verifica se o índice atingiu o limite e, caso positivo, ativa o buffer circular
    if (config.IndiceAtual >= BUFFER_SIZE)
    {
        config.IndiceAtual = 0;                                                // Reinicia o índice para sobrescrever o início
        config.BufferCircularAtivo = true;                                     // Define que o buffer está operando de forma circular
        config.IndiceMaisAntigo = (config.IndiceMaisAntigo + 1) % BUFFER_SIZE; // Atualiza o índice mais antigo
    }

    // Atualiza o registro na posição indicada pelo índice atual
    memcpy(BufferRegistros[config.IndiceAtual].hw_id, _hw_id, 6);
    BufferRegistros[config.IndiceAtual].freepd = _freepd;
    BufferRegistros[config.IndiceAtual].data = _dataUser;
    BufferRegistros[config.IndiceAtual].event_id = _event;
    BufferRegistros[config.IndiceAtual].timestamp = _timestamp;

    // Incrementa o índice para a próxima posição, se necessário
    config.IndiceAtual = (config.IndiceAtual + 1) % BUFFER_SIZE;

    // Salva o buffer atualizado de volta no arquivo de dados
    dataFile = SPIFFS.open(_filenameData, FILE_WRITE);
    if (dataFile)
    {
        dataFile.write(reinterpret_cast<const uint8_t *>(BufferRegistros), sizeof(Registros) * BUFFER_SIZE);
        dataFile.close();
    }
    else
    {
        debug.Println("DispenserData.PutNovaPos()", "Erro ao abrir /Registros.bin ", "WARN");
        delete[] BufferRegistros; // Liberar memória antes de retornar
        return;
    }

    // Salva a configuração atualizada no arquivo de configuração
    configFile = SPIFFS.open(_filenameConfig, FILE_WRITE);
    if (configFile)
    {
        configFile.write(reinterpret_cast<const uint8_t *>(&config), sizeof(ConfigReg));
        configFile.close();
    }
    else
    {
        debug.Println("DispenserData.PutNovaPos()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
    }

    debug.Println("DispenserData.PutNovaPos()", "=== Configuração Atual ===", "INFO");
    debug.Println("DispenserData.PutNovaPos()", "Indice Atual: " + String(config.IndiceAtual), "INFO");
    debug.Println("DispenserData.PutNovaPos()", "Indice Mais Antigo: " + String(config.IndiceMaisAntigo), "INFO");
    debug.Print("DispenserData.PutNovaPos()", "Buffer Circular Ativo: ", "INFO");
    debug.Println("DispenserData.PutNovaPos()", config.BufferCircularAtivo ? "Sim" : "Não", "INFO");
    debug.Print("DispenserData.PutNovaPos()", "Dados de configuração inicializado ? ", "INFO");
    debug.Println("DispenserData.PutNovaPos()", config.Inicializado ? "Sim" : "Não", "INFO");
    debug.Println("DispenserData.PutNovaPos()", "==========================", "INFO");
    //
    int indiceParaImprimir = (config.IndiceAtual == 0) ? (BUFFER_SIZE - 1) : (config.IndiceAtual - 1);
    String macAddress = "";
    // Concatena os bytes em formato "XX:XX:XX:XX:XX:XX"
    for (int i = 0; i < 6; i++)
    {
        if (i > 0)
        {
            macAddress += ":"; // Adiciona ":" entre os bytes
        }
        macAddress += String(BufferRegistros[indiceParaImprimir].hw_id[i], HEX); // Converte o byte para HEX
    }
    // Converte para maiúsculas
    macAddress.toUpperCase();
    debug.Println("DispenserData.PutNovaPos()", "=== Dados do Buffer ===", "INFO");

    debug.Print("DispenserData.PutNovaPos()", " || Hardware Id : " + macAddress, "INFO");
    debug.Print("DispenserData.PutNovaPos()", " || Event : " + String(BufferRegistros[indiceParaImprimir].event_id), "INFO");
    debug.Print("DispenserData.PutNovaPos()", " || Data : " + String(BufferRegistros[indiceParaImprimir].data), "INFO");
    debug.Println("DispenserData.PutNovaPos()", " || TimeStamp : " + String(BufferRegistros[indiceParaImprimir].timestamp), "INFO");
    debug.Println("DispenserData.PutNovaPos()", "=======================", "INFO");

    // Libera a memória alocada após o uso
    delete[] BufferRegistros; // Liberar memória antes de retornar
    debug.Print("DispenserData.PutNovaPos()", "Dados Adicionados com sucesso", "WARN");
}

DispenserData::Registros DispenserData::Get_RegistroMaisAntigo()
{
    debug.Println("DispenserData.GetPosAntiga()", "Retirando dados da fila", "WARN");
    // Variáveis temporárias
    ConfigReg config;

    // Ler a configuração do arquivo _filenameConfig
    File configFile = SPIFFS.open(_filenameConfig, FILE_READ);
    if (!configFile)
    {
        debug.Println("DispenserData.GetPosAntiga()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
        return {}; // Retorna um registro vazio em caso de erro
    }
    configFile.read(reinterpret_cast<uint8_t *>(&config), sizeof(ConfigReg));
    configFile.close();

    if (config.IndiceAtual > 0)
    {
        Registros *BufferRegistros = new Registros[BUFFER_SIZE]; // Usando memória dinâmica
        // zera variaveis da alocação dinamica do buffer
        memset(BufferRegistros, 0, sizeof(Registros) * BUFFER_SIZE);
        Registros posicaoMaisAntiga; // Registro mais antigo para retornar
        // Ler o buffer do arquivo _filenameData
        File dataFile = SPIFFS.open(_filenameData, FILE_READ);
        if (!dataFile)
        {
            debug.Println("DispenserData.GetPosAntiga()", "Erro ao abrir /Registros.bin ", "WARN");
            delete[] BufferRegistros; // Liberar memória antes de retornar
            return {};                // Retorna um registro vazio em caso de erro
        }
        dataFile.read(reinterpret_cast<uint8_t *>(BufferRegistros), sizeof(Registros) * BUFFER_SIZE);
        dataFile.close();

        // Armazenar a posição mais antiga
        posicaoMaisAntiga = BufferRegistros[config.IndiceMaisAntigo];

        // Verifica se o buffer está operando de forma circular
        if (config.BufferCircularAtivo)
        {
            // Limpar a posição de dados removida no buffer circular
            memset(&BufferRegistros[config.IndiceMaisAntigo], 0, sizeof(Registros));

            // Avançar o índice de leitura (retirar o dado mais antigo)
            config.IndiceMaisAntigo = (config.IndiceMaisAntigo + 1) % BUFFER_SIZE;
            if (config.IndiceMaisAntigo = 0)
            {
                config.BufferCircularAtivo = false;
            }
        }
        else
        {
            // Modo não circular: deslocar o buffer
            for (size_t i = config.IndiceMaisAntigo; i < config.IndiceAtual - 1; ++i)
            {
                BufferRegistros[i] = BufferRegistros[i + 1];
            }

            // Limpar a última posição do buffer
            memset(&BufferRegistros[config.IndiceAtual - 1], 0, sizeof(Registros));

            // Atualizar os índices
            config.IndiceAtual--; // Reduz a contagem do buffer
        }

        // Escrever o buffer atualizado no arquivo _filenameData
        dataFile = SPIFFS.open(_filenameData, FILE_WRITE);
        if (!dataFile)
        {
            debug.Println("DispenserData.GetPosAntiga()", "Erro ao abrir /Registros.bin ", "WARN");
        }
        else
        {
            dataFile.write(reinterpret_cast<const uint8_t *>(BufferRegistros), sizeof(Registros) * BUFFER_SIZE);
            dataFile.close();
        }

        // Atualizar a configuração no arquivo _filenameConfig
        configFile = SPIFFS.open(_filenameConfig, FILE_WRITE);
        if (!configFile)
        {
            debug.Println("DispenserData.GetPosAntiga()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
        }
        else
        {
            configFile.write(reinterpret_cast<const uint8_t *>(&config), sizeof(ConfigReg));
            configFile.close();
        }

        // Liberar memória antes de retornar
        delete[] BufferRegistros;

        debug.Println("DispenserData.PutNovaPos()", "=== Configuração Atual ===", "INFO");
        debug.Println("DispenserData.PutNovaPos()", "Indice Atual: " + String(config.IndiceAtual), "INFO");
        debug.Println("DispenserData.PutNovaPos()", "Indice Mais Antigo: " + String(config.IndiceMaisAntigo), "INFO");
        debug.Print("DispenserData.PutNovaPos()", "Buffer Circular Ativo: ", "INFO");
        debug.Println("DispenserData.PutNovaPos()", config.BufferCircularAtivo ? "Sim" : "Não", "INFO");
        debug.Print("DispenserData.PutNovaPos()", "Dados de configuração inicializado ? ", "INFO");
        debug.Println("DispenserData.PutNovaPos()", config.Inicializado ? "Sim" : "Não", "INFO");
        debug.Println("DispenserData.PutNovaPos()", "==========================", "INFO");
        //
        int indiceParaImprimir = (config.IndiceAtual == 0) ? (BUFFER_SIZE - 1) : (config.IndiceAtual - 1);
        String macAddress = "";
        // Concatena os bytes em formato "XX:XX:XX:XX:XX:XX"
        for (int i = 0; i < 6; i++)
        {
            if (i > 0)
            {
                macAddress += ":"; // Adiciona ":" entre os bytes
            }
            macAddress += String(BufferRegistros[indiceParaImprimir].hw_id[i], HEX); // Converte o byte para HEX
        }
        // Converte para maiúsculas
        macAddress.toUpperCase();
        debug.Println("DispenserData.PutNovaPos()", "=== Dados do Buffer ===", "INFO");

        debug.Print("DispenserData.PutNovaPos()", " || Hardware Id : " + macAddress, "INFO");
        debug.Print("DispenserData.PutNovaPos()", " || Event : " + String(BufferRegistros[indiceParaImprimir].event_id), "INFO");
        debug.Print("DispenserData.PutNovaPos()", " || Data : " + String(BufferRegistros[indiceParaImprimir].data), "INFO");
        debug.Println("DispenserData.PutNovaPos()", " || TimeStamp : " + String(BufferRegistros[indiceParaImprimir].timestamp), "INFO");
        debug.Println("DispenserData.PutNovaPos()", "=======================", "INFO");

        debug.Println("DispenserData.GetPosAntiga()", "Dados da fila retirados", "WARN");
        // Retornar o registro mais antigo
        return posicaoMaisAntiga;
    }
    else
    {
        debug.Println("DispenserData.GetPosAntiga()", "Buffer vazio, nenhum dado a ser retornado", "WARN");
        return {};
    }
}

DispenserData::ConfigReg DispenserData::Read_ConfigBuffer()
{

    ConfigReg config;

    debug.Println("DispenserData.VerificaBuffer()", "Coletando dados de configuraçoes do buffer", "WARN");
    // Lê o arquivo de configuração para obter o índice atual
    File configFile = SPIFFS.open(_filenameConfig, FILE_READ);
    if (!configFile)
    {
        debug.Println("DispenserData.VerificaBuffer()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
        return {};
    }
    else
    {
        configFile.read(reinterpret_cast<uint8_t *>(&config), sizeof(ConfigReg));
        configFile.close();
        return config;
        debug.Println("DispenserData.VerificaBuffer()", "Dados coletados", "WARN");
    }
}

bool DispenserData::Clear_Registros()
{
    debug.Println("DispenserData.Clear_Registros()", "Limpando /RegistroConfig.bin | /Registros.bin", "WARN");
    // Cria buffers zerados diretamente
    Registros *bufferZerado = new Registros[BUFFER_SIZE]; // Usando memória dinâmica
    // zera variaveis da alocação dinamica do buffer
    memset(bufferZerado, 0, sizeof(Registros) * BUFFER_SIZE);

    ConfigReg config = {0, 0, 0, true, false}; // Índice e posição zerados, dados OK

    // Abre o arquivo de dados e limpa o conteúdo
    File dataFile = SPIFFS.open(_filenameData, FILE_WRITE);
    if (!dataFile)
    {
        debug.Println("DispenserData.Clear_Registros()", "Erro ao abrir /Registros.bin ", "WARN");
        delete[] bufferZerado;
        return false;
    }
    else
    {
        dataFile.write(reinterpret_cast<const uint8_t *>(bufferZerado), sizeof(bufferZerado));
        dataFile.close();
    }

    // Atualiza o arquivo de configuração
    File configFile = SPIFFS.open(_filenameConfig, FILE_WRITE);
    if (!configFile)
    {
        debug.Println("DispenserData.Clear_Registros()", "Erro ao abrir arquivo /RegistroConfig.bin ", "WARN");
        delete[] bufferZerado;
        return false;
    }
    else
    {
        configFile.write(reinterpret_cast<const uint8_t *>(&config), sizeof(ConfigReg));
        configFile.close();
    }

    debug.Println("DispenserData.Clear_Registros()", "Arquivos /RegistroConfig.bin | /Registros.bin  limpos", "WARN");
    delete[] bufferZerado;
    return true;
}

//---------------------------funções relacionadas a manipulação dos dados do wifi --------------------------//

bool DispenserData::Clear_WifiConfig()
{
    // Registra um log indicando que o arquivo /WifiDataDisp.bin está sendo limpo
    debug.Println("DispenserData.Clear_WifiConfig()", "Limpando arquivo /WifiDataDisp.bin ", "WARN");

    // Cria uma estrutura para armazenar os dados lidos do arquivo, inicializada com zero
    WifiDataDisp _WifiDataDisp = {};

    // Tenta abrir o arquivo /WifiDataDisp.bin para escrita no SPIFFS
    File _WifiDataDispFile = SPIFFS.open(_filenameWifiData, FILE_WRITE);

    // Verifica se o arquivo foi aberto com sucesso
    if (_WifiDataDispFile)
    {
        // Escreve a estrutura _WifiDataDisp no arquivo
        _WifiDataDispFile.write(reinterpret_cast<const uint8_t *>(&_WifiDataDisp), sizeof(_WifiDataDisp));

        // Fecha o arquivo
        _WifiDataDispFile.close();

        // Registra um log indicando que o arquivo foi criado com sucesso
        debug.Println("DispenserData.Clear_WifiConfig()", "Arquivo /WifiDataDisp.bin, criado com sucesso", "INFO");

        // Retorna true indicando que a operação foi bem-sucedida
        return true;
    }
    else
    {
        // Caso o arquivo não possa ser aberto, registra um log de erro
        debug.Println("DispenserData.Clear_WifiConfig()", "Erro ao abrir /WifiDataDisp.bin, ou arquivo corrompido", "WARN");

        // Retorna false indicando que a operação falhou
        return false;
    }
}

DispenserData::WifiDataDisp DispenserData::Read_WifiDataDisp()
{
    // Registra um log indicando que o arquivo /WifiDataDisp.bin está sendo limpo
    debug.Println("DispenserData.Read_WifiDataDisp()", "Lendo em /WifiDataDisp.bin ", "INFO");
    // Cria uma estrutura para armazenar os dados lidos do arquivo, inicializada com zero
    WifiDataDisp _WifiDataDisp = {};

    // Tenta abrir o arquivo /WifiDataDisp.bin para leitura no SPIFFS
    File _WifiDataDispFile = SPIFFS.open(_filenameWifiData, FILE_READ);

    // Verifica se o arquivo foi aberto corretamente
    if (!_WifiDataDispFile)
    {
        // Caso o arquivo não possa ser aberto, registra um log de erro
        debug.Println("DispenserData.Read_WifiDataDisp()", "Erro ao abrir arquivo /WifiDataDisp.bin ", "ERROR");

        // Retorna uma estrutura vazia (com todos os campos zero) indicando que houve erro na leitura
        return {};
    }
    else
    {
        // Lê os dados do arquivo e os armazena na estrutura _WifiDataDisp
        _WifiDataDispFile.read(reinterpret_cast<uint8_t *>(&_WifiDataDisp), sizeof(_WifiDataDisp));

        // Fecha o arquivo após a leitura
        _WifiDataDispFile.close();

        // Registra um log indicando que os dados foram coletados com sucesso
        debug.Println("DispenserData.Read_WifiDataDisp()", "Dados coletados /WifiDataDisp.bin ", "INFO");

        // Retorna a estrutura com os dados lidos do arquivo
        return _WifiDataDisp;
    }
}

void DispenserData::Write_WifiDataDisp(const std::string &_Ssid, const std::string &_Pass, uint8_t _Mode, uint16_t _SyncTime)
{
    WifiDataDisp _WifiDataDisp = {};

    // Copia o SSID para a estrutura, garantindo que não ultrapasse o tamanho do buffer
    strncpy(_WifiDataDisp.Ssid, _Ssid.c_str(), sizeof(_WifiDataDisp.Ssid) - 1);
    _WifiDataDisp.Ssid[sizeof(_WifiDataDisp.Ssid) - 1] = '\0'; // Garante terminação correta

    // Copia a senha para a estrutura, garantindo que não ultrapasse o tamanho do buffer
    strncpy(_WifiDataDisp.Pass, _Pass.c_str(), sizeof(_WifiDataDisp.Pass) - 1);
    _WifiDataDisp.Pass[sizeof(_WifiDataDisp.Pass) - 1] = '\0';

    _WifiDataDisp.Mode = _Mode;
    _WifiDataDisp.SyncTime = _SyncTime;

    File _WifiDataDispFile = SPIFFS.open(_filenameWifiData, FILE_WRITE);
    if (_WifiDataDispFile)
    {
        _WifiDataDispFile.write(reinterpret_cast<const uint8_t *>(&_WifiDataDisp), sizeof(_WifiDataDisp));
        _WifiDataDispFile.close();
        debug.Println("DispenserData.Write_WifiDataDisp()", "Escrita /WifiDataDisp.bin realizada com sucesso", "INFO");
    }
    else
    {
        debug.Println("DispenserData.Write_WifiDataDisp()", "Erro ao abrir /WifiDataDisp.bin ou arquivo corrompido", "ERROR");
    }
}

