#include <main.h>
#include <Pin_InOut.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
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

// programa antigo 
/*



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







