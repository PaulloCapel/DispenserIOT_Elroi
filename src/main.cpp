#include <main.h>

uint8_t S_Detector_Pin = 34;
uint8_t S_Temporizador_Pin = 35;
uint8_t Bomba_Pin = 26, LedVD_Pin = 25;



A041SK DetectorDeMaos(10,90, S_Detector_Pin); // Inicializa com Min = 10, Max = 90 e Pino 34
Rv_Timer PotenciometroTemporizador(0,10000,S_Temporizador_Pin);
unsigned long GetMillis = 0, EndMillis = 0;
bool LigaBomba = false;

HardwareSerial A0221AU_Serial(1);
#define A0221AU_RX_Pin  33
#define A0221AU_TX_Pin  32
unsigned char data[4]={};
float distance;

void setup() {

  // inicia serial de comunicação com sensor
  A0221AU_Serial.begin(9600,SERIAL_8N1,A0221AU_RX_Pin,A0221AU_TX_Pin);

  pinMode(Bomba_Pin,OUTPUT);
  pinMode(LedVD_Pin,OUTPUT);
  digitalWrite(LedVD_Pin,HIGH);
  Serial.begin(115200);
  vTaskDelay(100);
  Serial.println("Iniciando Programa");
  digitalWrite(LedVD_Pin,LOW);  
  digitalWrite(Bomba_Pin,LOW);







}

void loop() {

  bool _Value = false, _LastValue;
  uint16_t _ValueTemporizador = 0;
  // Verifica se há dados disponíveis no buffer serial
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

 
  //Serial.println("Inicia Leitura de A041SK");
  /*
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
 /*
  vTaskDelay(100);
  do{
     for(int i=0;i<4;i++)
     {
       data[i]=A0221AU_Serial.read();
       Serial.print(data[i]);
     }
  }while(A0221AU_Serial.read()==0xff);
   Serial.println(  );

  A0221AU_Serial.flush();

  if(data[0]==0xff)
    {
      int sum;
      sum=(data[0]+data[1]+data[2])&0x00FF;
      if(sum==data[3])
      {
        distance=(data[1]<<8)+data[2];
        if(distance>30)
          {
           Serial.print("distance=");
           Serial.print(distance/10);
           Serial.println("cm");
          }else 
             {
               Serial.println("Below the lower limit");
             }
      }else Serial.println("ERROR");
     }
     
  */

}






