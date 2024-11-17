#include <main.h>
#include <pin_InOut.h>
#include <variaveis.h>
#include <web_content.h>

// prototipos de funções do rtos

void xTask_WifiConfig(void *pvParameters);
void xTask_ConnectionTest(void *pvParameters);
void xTask_SendDataTo_Server(void *pvParameters);
void xTask_ReciveDataBy_Server(void *pvParameters);
void xTask_TankLevel(void *pvParameters);
void xTask_StatusLed(void *pvParameters);
void xTask_ControlDispenser(void *pvParameters);

// prototipos de funções comuns

void Pin_InOutConfig();
bool _WifiConnect();
void handleScan();
void handleRoot();
void handleSave();

// instancia memoria flash  esp32

Preferences preferences;

// instancia do server

WebServer server;

Adafruit_PN532 nfc(PN532_SDA, PN532_SLC);





void setup()
{  

  Serial.begin(115200);  
  //esp_log_level_set("*", ESP_LOG_INFO); // inicializa log de informações do esp 32
 //esp_log_set_timestamp_source(ESP_LOG_TIMESTAMP_SOURCE_RTC);
  ESP_LOGI(InitTag, "Iniciando o programa...");
  Serial.println("Iniciando inicialização");
  vTaskDelay(pdMS_TO_TICKS(10));

   
  Serial.println("Configuração de pinos start");  
  Pin_InOutConfig();// chama função de inicializa pinos 

  
  Serial.println("Verificando se os dados do server foi configurado");
  
  preferences.begin("Parameters", false); // false = read/write // abre namespace da preferences
  bool WifiConfigured = preferences.getBool("Configured"); //
  preferences.end();

  Serial.print("Portal foi configurado? : ");
  Serial.println(WifiConfigured ? "true" : "false");
  // Se o portal nao foi configurado ele inicia em modo ap
  //  verifica se dip switch bit 0 esta em false
  Serial.print("DipSwitch Bit0 OFF ? ");
  Serial.println(digitalRead(DipSwitch_Bit0) ? "true" : "false");

  xEventGroupStatusHandle = xEventGroupCreate(); // inicia eventgroup do status do led

  // faz a verificação se o portal ja foi configurado, ou se existe solicitação via pinos
  if (!WifiConfigured || !digitalRead(DipSwitch_Bit0))
  {

    Serial.println("Dispenser nao foi configurado, iniciando AP mode");
    // faz demonstração visual que o portal nao foi configurado
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM1Hz);
    vTaskDelay(pdMS_TO_TICKS(500));
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM_VD1Hz);
    vTaskDelay(pdMS_TO_TICKS(500));
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVD1Hz);

    WiFi.softAP("ConfigPortal");
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/scan", handleScan);
    server.begin();

    xTaskCreate(xTask_WifiConfig, "TASK0", configMINIMAL_STACK_SIZE + 4096, NULL, 1, &xTask_WifiConfigHandle);
  }
  // se o portal já esta configurado ele inicia o processo
  else
  {
    xTaskCreatePinnedToCore(xTask_ConnectionTest, "TASK11", configMINIMAL_STACK_SIZE + 4096, NULL, 1, &xTask_PingTestHandle, APP_CPU_NUM);
  }

  xTaskCreatePinnedToCore(xTask_StatusLed, "TASK10", configMINIMAL_STACK_SIZE, NULL, 1, &xTask_StatusLedHandle, APP_CPU_NUM);

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
};

void loop()
{

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        // If you want to write something to block 4 to test with, uncomment
		// the following line and this text should be read back in a minute
        memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
         success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 1:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");

          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }

    
  }
    vTaskDelay(pdMS_TO_TICKS(2000));
    // vTaskDelete(NULL);
  
};

/*----------------------------------TAREFAS RTOS----------------------------------------- */

// TAREFA DE MODO DE CONGIGURAÇÃO EM MODO AP
  void xTask_WifiConfig(void *pvParameters)
  {

    unsigned long lastTime = 0;
    const unsigned long interval = 2000; // Chama a cada 50ms

    while (pdTRUE)
    {

      server.handleClient();
      // demostra que o portal esta ativo
      unsigned long currentTime = millis();
      if (currentTime - lastTime >= interval)
      {
        lastTime = currentTime;
        xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVM_VD1Hz);
      }

      vTaskDelay(pdMS_TO_TICKS(25));
    }
  };
// TAFERA DE STATUS DO LED DA PLACA
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
      if ((xEventBits & xEvG_OnLedEsp32) != 0) {  // Ação para o BIT_6
          digitalWrite(LedESP32,HIGH);
      }
      // ledliga led da pcb da placa
      if ((xEventBits & xEvG_OffLedEsp32) != 0) {  // Ação para o BIT_7
          digitalWrite(LedESP32,LOW);
      }
      

      vTaskDelay(pdMS_TO_TICKS(20));
    }
  };
// TAREFA DE VERIFICAÇÃO DE CONEXÃO COM SERVIDOR ELROI
  void xTask_ConnectionTest(void *pvParameters)
  {
    while (pdTRUE)
    {
      xEventGroupSetBits(xEventGroupStatusHandle, xEvG_ClockLedVD1Hz);

      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  };



/*---------------------------------------------------------------------------------------- */


/*-----------------------------------funções do projeto----------------------------------- */
 
/*---------------------------funções modo de congiguração modo AP------------------------- */
  void handleRoot()
  {
    String html = htmlPage; // A página inteira já está em htmlPage

    // Escaneia redes Wi-Fi e substitui a mensagem padrão
    int n = WiFi.scanNetworks();
    String options;
    if (n == 0)
    {
      options = "<option value=''>Nenhuma rede encontrada</option>";
    }
    else
    {
      for (int i = 0; i < n; i++)
      {
        options += "<option value='" + String(WiFi.SSID(i)) + "'>" +
                   String(WiFi.SSID(i)) + " (Sinal: " +
                   String(WiFi.RSSI(i)) + " dBm)</option>";
      }
    }

    // Substitui o placeholder no HTML com a lista de redes
    html.replace("<option value=''>Nenhuma rede encontrada</option>", options);

    // Envia a resposta ao cliente
    server.send(200, "text/html", html);
  }
  void handleScan()
  {
    int n = WiFi.scanNetworks();
    String json = "{\"networks\":[";
    for (int i = 0; i < n; i++)
    {
      if (i > 0)
        json += ",";
      json += "{\"ssid\":\"" + String(WiFi.SSID(i)) + "\",\"signal\":\"" + String(WiFi.RSSI(i)) + "\"}";
    }
    json += "]}";

    server.send(200, "application/json", json);
  }
  void handleSave()
  {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    String porta = server.arg("porta");
    String hwid = server.arg("hwid");

    Serial.println("Salvando dados recebidos do portal : ");
    Serial.println("SSID: " + ssid);
    Serial.println("password : " + password);
    Serial.println("porta : " + porta);
    Serial.println("hwid : " + hwid);
    Serial.println("Portal Configured : " + true);

    preferences.begin("Parameters", false); // false = read/write
    preferences.putBool("Configured", true);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putString("porta", porta);
    preferences.putString("hwid", hwid);
    preferences.end();

    server.send(200, "text/html; charset=UTF-8", "<html><body><h2>Configurações Salvas!</h2></body></html>");
    Serial.println("Deletando task e reiniciando esp");
    xEventGroupSetBits(xEventGroupStatusHandle, xEvG_OnLedVM);
    vTaskDelete(xTask_WifiConfigHandle);
  }
/*---------------------------------------------------------------------------------------- */

/*--------------------------------------funções gerais-------------------------------------*/

// configurações de pinos
void Pin_InOutConfig()
{

  pinMode(DipSwitch_Bit0, INPUT_PULLUP);
  pinMode(DipSwitch_Bit1, INPUT_PULLUP);

  pinMode(LedESP32,OUTPUT);

  vTaskDelay(pdMS_TO_TICKS(100));

  Serial.println("Configuracao de pinos inicializada");
}
// configuração e conecção no wifi
bool _WifiConnect(){


return true;
}






/*---------------------------------------------------------------------------------------- */

  // programa antigo
  /*
  #include "web_content.h"  // Inclua o cabeçalho aqui

  WebServer server;
  Preferences preferences;

  void handleRoot();
  void handleSave();


  void setup() {
      Serial.begin(115200);
      WiFi.softAP("ConfigPortal");
      server.on("/", handleRoot);
      server.on("/save", handleSave);
      server.begin();
      Serial.println("Servidor iniciado");
  }

  void loop() {
      server.handleClient();
  }

  void handleRoot() {
      String html = htmlPage;  // Comece com a parte inicial do HTML

      int n = WiFi.scanNetworks();
      if (n == 0) {
          html += "<option value=''>Nenhuma rede encontrada</option>";
      } else {
          for (int i = 0; i < n; i++) {
              html += "<option value='" + String(WiFi.SSID(i)) + "'>" + String(WiFi.SSID(i)) + " (Sinal: " + String(WiFi.RSSI(i)) + " dBm)</option>";
          }
      }

      html += htmlEnd;  // Adicione a parte final do HTML

      server.send(200, "text/html", html);
  }

  void handleSave() {
      String ssid = server.arg("ssid");
      String password = server.arg("password");
      String porta = server.arg("porta");
      String hwid = server.arg("hwid");

      preferences.begin("wifi", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.putString("porta", porta);
      preferences.putString("hwid", hwid);
      preferences.end();

      server.send(200, "text/html", "<html><body><h2>Configurações Salvas!</h2></body></html>");
  }





  void setup() {



    pinMode(BombaPin,OUTPUT);
    pinMode(LedVDPin,OUTPUT);
    digitalWrite(LedVDPin,HIGH);
    Serial.begin(115200);
    vTaskDelay(100);
    Serial.println("Iniciando Programa");
    digitalWrite(LedVDPin,LOW);
    digitalWrite(BombaPin,LOW);







  }


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
