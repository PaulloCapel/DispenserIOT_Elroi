#include <main.h>
#include <pin_InOut.h>
#include <variaveis.h>

// prototipos de funções do rtos

void xTask_StatusLed(void *pvParameters);
void xTask_ControlDispenser(void *pvParameters);

void xTask_SelectComunicationMode(void *pvParameters);
void xTask_ComunicationModeMaster(void *pvParameters);
void xTask_CommunicationModeSlave(void *pvParameters);
void xTask_ControlDispenser(void *pvParameters);

// prototipos de funções comuns

void Pin_InOutConfig();
bool _WifiConnect();
CardRFID NFC_Check();

// intancia de libs

Adafruit_PN532 nfc(PN532_SDA, PN532_SLC);

myDebug debug(true); // cria instancia para lib de debug serial

DispenserData MyDataDispenser(debug); // cria instancia dados SPIFFS, e repassa  instancia do debug compartilhada

WifiPortal MyPortalConfig(debug, MyDataDispenser); // cria instancia do portal, e repassa  instancia do debug compartilhada

A041SK DetectorDeMaos(30, 100, S_DetectorPin);
// RV1_Timer PotenciometroTemporizador(1000, 5000, S_TemporizadorPin);

// structs do processo

DispenserData::Registros _RegistrosTemp;
DispenserData::ConfigReg _ConfigRegTemp;
DispenserData::WifiDataDisp _WifiDataTemp;

void setup()
{

  debug.begin(3, 115200); // Configura o nível de debug: 3 (INFO, WARN, ERROR)

  debug.Println("SETUP", "=============Dispenser IOT Elroi Medicial=============", "WARN");

  Pin_InOutConfig(); // chama função de inicializa pinos

  debug.Println("SETUP", "Inicializando tarefas RTOS criticas do sistema", "WARN");

  //------------------------------------INICIALIZAÇÃO CRITICA RTOS -----------------------------//
  xEventGroupStatusHandle = xEventGroupCreate(); // inicia eventgroup do status do led
  xTaskCreatePinnedToCore(xTask_StatusLed, "TASK10", configMINIMAL_STACK_SIZE, NULL, 2, &xTask_StatusLedHandle, PRO_CPU_NUM);
  //------------------------------------INICIALIZAÇÃO CRITICA RTOS -----------------------------//

  debug.Println("SETUP", "Inicializando Sistemas de arquivos do sistema", "WARN");
  MyDataDispenser.begin();
  vTaskDelay(pdMS_TO_TICKS(100));

  /*

  debug.Println("SETUP", "Requisicao restauração memoria flash via DipSwitch_Bit1 ? ", "WARN");
  debug.Println("SETUP", digitalRead(!DipSwitch_Bit1) ? "SIM" : "NAO", "WARN");

  if (!DipSwitch_Bit1)
  {
    MyDataDispenser.Clear_Registros();
  }

  */

  debug.Println("SETUP", "Verificando se portal já foi configurado", "WARN");
  _WifiDataTemp = MyDataDispenser.Read_WifiDataDisp();

  debug.Print("SETUP", "Modo de operação do dispenser : ", "WARN");
  debug.Println("SETUP", String(_WifiDataTemp.Mode), "WARN");
  debug.Println("SETUP", " 0 = não configurado || 1 = modo master || 2 = modo slave ", "WARN");
  debug.Println("SETUP", "SSID : " + String(_WifiDataTemp.Ssid) + " PASSWORD :" + String(_WifiDataTemp.Pass), "WARN");

  debug.Print("SETUP", "Requisicao de configuracao via DipSwitch_Bit0 ? ", "WARN");
  bool DipSwitch_Bit0_value = digitalRead(DipSwitch_Bit0);
  delay(250);
  debug.Println("SETUP", DipSwitch_Bit0_value ? "SIM" : "NAO", "WARN");

  // se o portal não foi configurado, ou houve uma requisição via pinos
  if (_WifiDataTemp.Mode == 0 || DipSwitch_Bit0_value)
  {

    // se entrou aqui, é porque não existe configuração ou foi forçada pelos dipswitch
    debug.Println("SETUP", "Inicializando Portal de configuracao ", "WARN");
    // faz demonstração visual que o portal nao foi configurado
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM1Hz);
    vTaskDelay(pdMS_TO_TICKS(500));
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM_VD1Hz);
    vTaskDelay(pdMS_TO_TICKS(500));
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVD1Hz);

    xTaskCreatePinnedToCore(xTask_SelectComunicationMode, "TASK0", 4096, NULL, 1, &xTask_SelectComunicationModeHandle, APP_CPU_NUM);
    vTaskDelay(pdMS_TO_TICKS(250));
  }
  // se o portal já foi configurado
  else
  {

    // verifica se o dispenser foi configurado em modo master == 1
    if (_WifiDataTemp.Mode == 1)
    {
      debug.Println("SETUP", "Inicializando Dispenser em modo Master", "WARN");
      bool StatusWifiConnect = _WifiConnect();
      if (StatusWifiConnect)
      {
        debug.Println("SETUP", "Criando Task Metodo ComunicationMaster", "WARN");
        xTaskCreatePinnedToCore(xTask_ComunicationModeMaster, "TASK2", 8192, NULL, 1, &xTask_ComunicationModeMasterHandle, tskNO_AFFINITY);
      }
      else
      {
        debug.Println("SETUP", "Não foi possivel se conectar no wifi", "ERROR");
        delay(3000);
        ESP.restart();
        // TODO :  se não conectar wifi o que fazer ?????
      }
    }
    // se não é  == 1 então slave
    else
    {

      debug.Println("SETUP", "Inicializando Dispenser em modo Slave", "WARN");
      xTaskCreatePinnedToCore(xTask_CommunicationModeSlave, "TASK1", 4096, NULL, 1, &xTask_CommunicationModeSlaveHandle, tskNO_AFFINITY);
    }
  }

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1)
      ; // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  xTaskCreatePinnedToCore(xTask_ControlDispenser, "TASK20", 2048, NULL, 1, &xTask_ControlDispenserHandle, tskNO_AFFINITY);
};

void loop()
{
  vTaskDelete(NULL);
};

/*----------------------------------TAREFAS RTOS----------------------------------------- */

// TAREFA DE MODO DE CONGIGURAÇÃO EM MODO AP
void xTask_SelectComunicationMode(void *pvParameters)
{
  // Executa apenas uma vez antes de entrar no loop
  MyPortalConfig.ApMode(); // Inicializa o portal AP

  unsigned long lastTime = 0;
  const unsigned long interval = 2000; // tempo de demostração de portal ativo led

  while (pdTRUE)
  {
    bool ConfigDone = MyPortalConfig.HandleClient();

    // verifica que se a configuração do portal já foi feita.
    if (ConfigDone)
    {
      xEventGroupSetBits(xEventGroupStatusHandle, xEvG_OffLedVM_VD);
      delay(3000);
      ESP.restart();
    }
    // enquanto portal nao foi configurado sinaliza o led
    else
    {
      // demostra que o portal esta ativo
      unsigned long currentTime = millis();
      if (currentTime - lastTime >= interval)
      {
        lastTime = currentTime;
        xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM_VD1Hz);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(25));
  }
};

void xTask_ComunicationModeMaster(void *pvParameters)
{
  debug.Println("xTask_ComunicationModeMaster", "Inicializando dados da tarefa", "WARN");
  unsigned long lastTime = 0;
  const unsigned long interval_get = 10000;
  UBaseType_t highWaterMark;

  while (pdTRUE)
  {

    unsigned long currentTime = millis();
    if (currentTime - lastTime >= interval_get)
    {
      highWaterMark = uxTaskGetStackHighWaterMark(NULL);
      char buffer[50];                                                          // Buffer para formatar a mensagem
      snprintf(buffer, sizeof(buffer), "Pilha livre: %u bytes", highWaterMark); // Formata a string
      debug.Println("xTask_ComunicationModeMaster", buffer, "INFO");

      debug.Println("xTask_ComunicationModeMaster", "Tentativa de conexão com API", "WARN");

      HTTPClient http;
      String url = "https://bacpro.com.br/api/status";
      http.begin(url);                   // Inicia a conexão com a URL
      int httpResponseCode = http.GET(); // Faz a requisição GET
      debug.Println("xTask_ComunicationModeMaster", "Codigo de resposta HTTP : " + httpResponseCode, "WARN");

      if (httpResponseCode > 0)
      {

        String payload = http.getString(); // Obtém a resposta como string
        Serial.println("Resposta da API: " + payload);

        // Processando o JSON
        DynamicJsonDocument doc(200);
        DeserializationError error = deserializeJson(doc, payload);

        if (!error)
        {
          bool status = doc["status"];
          uint32_t timestamp = doc["timestamp"];

          Serial.print("Status: ");
          Serial.println(status);
          Serial.print("Timestamp: ");
          Serial.println(timestamp);
        }
        else
        {
          Serial.println("Erro ao parsear JSON!");
        }
      }
      else
      {
        Serial.print("Erro na requisição HTTP, código: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Fecha a conexão
    }

    vTaskDelay(10); // Pequeno atraso para evitar consumir CPU desnecessariamente
  }
};

void xTask_CommunicationModeSlave(void *pvParameters)
{
  while (pdTRUE)
  {
    vTaskDelay(10);
  }
};
// TAREFA DE CONTROLE DO DISPENSER
void xTask_ControlDispenser(void *pvParameters)
{
  unsigned long lastTime = 0;
  const unsigned long interval_rfid = 1000;
  const unsigned long interval_timeout = 10000;
  unsigned long interval_Bomba = 0;

  bool _Value = false, _LastValue, LigaBomba;
  uint16_t _ValueTemporizador = 0;

  while (pdTRUE)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= interval_rfid)
    {
      CardRFID checkCard = NFC_Check();
      if (checkCard.succes)
      {
        vTaskDelay(pdMS_TO_TICKS(250));
        xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVD1Hz);
        // apos a detecção do cartão ele deve esperar pela inteação das mão do usuario até o timeout
        bool EsperaPelasMaos = true;
        unsigned long InitTime_EsperaPelasMaos = millis();
        while (EsperaPelasMaos)
        {

          // enquanto o tempo for menor que o timeout espera pelas mãos
          if (millis() <= (InitTime_EsperaPelasMaos + interval_timeout))
          {
            _Value = DetectorDeMaos.Read();
            if (!LigaBomba)
            {
              if (_Value)
              {
                Serial.println("Mãos Detectadas");
                LigaBomba = true;

                interval_Bomba = millis() + 2000;
              }
            }

            while (LigaBomba)
            {
              if (millis() > interval_Bomba)
              {
                LigaBomba = false;
                digitalWrite(BombaPin, LOW);
                xEventGroupSetBits(xEventGroupStatusHandle, xEvG_OffLedVM_VD);
              }
              else
              {
                digitalWrite(BombaPin, HIGH);
                xEventGroupSetBits(xEventGroupStatusHandle, xEvG_OnLedVM_VD);
              }
            }
          }
          else
          {
            xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM1Hz);
            EsperaPelasMaos = false;
          }
        }

        lastTime = millis();
      }

      vTaskDelay(10); // Pequeno atraso para evitar consumir CPU desnecessariamente
    }
  }
}; // TAFERA DE STATUS DO LED DA PLACA
void xTask_StatusLed(void *pvParameters)
{

  EventBits_t xEventBits;
  while (pdTRUE)
  {

    xEventBits = xEventGroupWaitBits(
        xEventGroupStatusHandle, // O Event Group onde estamos aguardando
        0xFF,                    // Espera pelos bits de 0 a 7
        pdTRUE,                  // limpa bits ao sair
        pdFALSE,                 // espera por qualquer bit
        portMAX_DELAY            // Aguarda indefinidamente até que todos os bits sejam ativados
    );
    // clock led vm 1hz
    if ((xEventBits & xEvG_ClockLedVM1Hz) != 0)
    { // Ação para o BIT_0
      analogWrite(LedVMPin, 255);
      vTaskDelay(pdMS_TO_TICKS(500));
      analogWrite(LedVMPin, 0);
    }
    // clock led vd 1hz
    if ((xEventBits & xEvG_ClockLedVD1Hz) != 0)
    { // Ação para o BIT_1
      analogWrite(LedVDPin, 255);
      vTaskDelay(pdMS_TO_TICKS(500));
      analogWrite(LedVDPin, 0);
    }
    // clock led vd e led vm 1hz
    if ((xEventBits & xEvG_ClockLedVM_VD1Hz) != 0)
    { // Ação para o BIT_2
      analogWrite(LedVMPin, 31);
      analogWrite(LedVDPin, 255);
      vTaskDelay(pdMS_TO_TICKS(500));
      analogWrite(LedVMPin, 0);
      analogWrite(LedVDPin, 0);
    }
    // liga led vermelho indicação de falha de hardware
    if ((xEventBits & xEvG_OnLedVM) != 0)
    { // Ação para o BIT_3
      analogWrite(LedVMPin, 255);
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    // liga led vermelho e verde indicação de falta de comunicação
    if ((xEventBits & xEvG_OnLedVM_VD) != 0)
    { // Ação para o BIT_4
      analogWrite(LedVMPin, 3);
      analogWrite(LedVDPin, 255);
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    // deliga leds
    if ((xEventBits & xEvG_OffLedVM_VD) != 0)
    { // Ação para o BIT_5
      analogWrite(LedVMPin, 0);
      analogWrite(LedVDPin, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    // liga led led da pcb da placa
    if ((xEventBits & xEvG_OnLedEsp32) != 0)
    { // Ação para o BIT_6
      digitalWrite(LedESP32, HIGH);
    }
    // ledliga led da pcb da placa
    if ((xEventBits & xEvG_OffLedEsp32) != 0)
    { // Ação para o BIT_7
      digitalWrite(LedESP32, LOW);
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
};

/*--------------------------------------funções gerais-------------------------------------*/

// configurações de pinos
void Pin_InOutConfig()
{

  pinMode(DipSwitch_Bit0, INPUT_PULLUP);
  pinMode(DipSwitch_Bit1, INPUT_PULLUP);
  pinMode(LedESP32, OUTPUT);
  pinMode(BombaPin, OUTPUT);
  pinMode(LedVDPin, OUTPUT);
  vTaskDelay(pdMS_TO_TICKS(50));
  digitalWrite(LedVDPin, LOW);
  digitalWrite(BombaPin, LOW);
  vTaskDelay(pdMS_TO_TICKS(100));
}
// configuração e conecção no wifi

bool _WifiConnect()
{

  debug.Println("_WifiConnect()", "Dados do Wifi", "WARN");
  debug.Println("_WifiConnect()", "SSID : " + String(_WifiDataTemp.Ssid) + " PASSWORD :" + String(_WifiDataTemp.Pass), "WARN");
  // aguarda intervalo  caso passou, retorna false de  pois nao conseguiu se conectar no wifi
  if (_WifiDataTemp.Ssid != "" && _WifiDataTemp.Pass != "")
  {

    WiFi.begin(_WifiDataTemp.Ssid, _WifiDataTemp.Pass);
    debug.Println("_WifiConnect()", "Iniciando comunicacao WIFI", "WARN");

    const unsigned long WifiInterval = 15000 + millis(); // Tempo que espera o para fazer conexçao com wifi
    while (WiFi.status() != WL_CONNECTED)
    {
      // aguarda tempo imputado, caso nao se conectou retorna false

      if (millis() >= WifiInterval)
      {
        return false;
      }
      // enquanto passa o tempo printa dados a cada 1000ms
      vTaskDelay(pdMS_TO_TICKS(1000));
      xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM1Hz);
      Serial.print(".");
    }
    debug.Println("_WifiConnect()", "Wi-Fi conectado", "WARN");
    IPAddress localIP = WiFi.localIP();
    String ipString = localIP.toString(); // Converte o IP para uma string
    debug.Println("_WifiConnect()", "Endereco de IP : " + ipString, "WARN");

    return true;
  }
  else
  {
    debug.Println("_WifiConnect()", "SSID ou PASSWORD invalidos, reiniciando dados de congiguração do portal", "ERROR");

    return false;
  }
}

// função que le os dados do cartão
CardRFID NFC_Check()
{
  CardRFID _card;
  memset(&_card, 0, sizeof(CardRFID)); // Preenche todos os bytes do objeto com 0
  // chama função que le o cartão e aguarda retorno
  _card.succes = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _card.uid_uint8_t, &_card.uidlength);
  // verifica se cartão foi aproximado
  if (_card.succes)
  {
    // verifica se o modelo do cartão é de 4 bytes
    if (_card.uidlength == 4)
    {
      debug.Println("NFC_Check()", "Cartao detectado ", "INFO");
      debug.Println("NFC_Check()", "UID Length : " + String(_card.uidlength) + " bytes", "INFO");
      // faz o shift rollate para transformar os bytes em uma variavel inteiro 32bits
      _card.uid_uint32_t = (_card.uid_uint8_t[0] << 24) |
                           (_card.uid_uint8_t[1] << 16) |
                           (_card.uid_uint8_t[2] << 8) |
                           _card.uid_uint8_t[3];
      debug.Println("NFC_Check()", "UID Value: " + String(_card.uid_uint32_t), "INFO");
    }
    else
    {
      debug.Println("NFC_Check()", "Modelo de cartao nao suportado", "INFO");
      memset(&_card, 0, sizeof(CardRFID)); // Preenche todos os bytes do objeto com 0
    }
  }

  return _card;
}

/*---------------------------------------------------------------------------------------- */

// programa antigo
/*







void loop() {

  bool _Value = false, _LastValue;
  uint16_t _ValueTemporizador = 0;


}



*/

//--------------------------------------------------------------------------------------//
// função de leitura de habilitação da bomba
/*
  //Serial.println("Inicia Leitura de A041SK");
  _Value =  DetectorDeMaos.Read();
   if(!LigaBomba){
      if(_Value){
        Serial.println("Mãos Detectadas");
        LigaBomba = true;
        GetMillis = millis();
        EndMillis = GetMillis + PotenciometroTemporizador.ReadTimer();
         Serial.print("Temporizador Inicial : ");
         Serial.println(GetMillis);
         Serial.print("Temporizador Final : ");
         Serial.println(EndMillis);
      }
    }

  if (LigaBomba){
    vTaskDelay(50);
    if (millis() > EndMillis )
    {
      LigaBomba = false;
      digitalWrite(Bomba_Pin,LOW);
      digitalWrite(LedVD_Pin,LOW);
      Serial.println("Ciclo Bomba Finalizado");
    }else{
      digitalWrite(Bomba_Pin,HIGH);
      digitalWrite(LedVD_Pin,LOW);
    }
  }
*/

//--------------------------------------------------------------------------------------//

// função de leitura do sensor A0221AU
/*
declaração de variaveis e instancia
HardwareSerial A0221AU_Serial(1);
#define A0221AU_RX_Pin  33
#define A0221AU_TX_Pin  32
unsigned char data[4]={};
float distance
*/

/*
// inicia serial de comunicação com sensor
A0221AU_Serial.begin(9600,SERIAL_8N1,A0221AU_RX_Pin,A0221AU_TX_Pin);
*/

/*// Verifica se há dados disponíveis no buffer serial
  if (A0221AU_Serial.available()) {
    // Aguarda até encontrar o cabeçalho 0xFF
    if (A0221AU_Serial.read() == 0xFF) {
      uint8_t data[4];
      data[0] = 0xFF;  // Cabeçalho já lido

      // Lê os próximos 3 bytes
      for (int i = 1; i < 4; i++) {
        while (!A0221AU_Serial.available()); // Aguarda até o dado estar disponível
        data[i] = A0221AU_Serial.read();
      }
      A0221AU_Serial.flush();
      // Calcula a soma
      uint8_t sum = (data[0] + data[1] + data[2]) & 0x00FF;

      // Verifica se a soma está correta
      if (sum == data[3]) {
        int distance = (data[1] << 8) + data[2];
        if (distance > 30) {
          Serial.print("Distance = ");
          Serial.print(distance / 10);
          Serial.println(" cm");
        } else {
          Serial.println("Below the lower limit");
        }
      } else {
        Serial.println("ERROR: Checksum mismatch");
      }
    }
  }
  // Aguarda antes de verificar novamente
  vTaskDelay(100);
*/
