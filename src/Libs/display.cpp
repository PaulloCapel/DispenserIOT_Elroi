#include "display.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void DisplayInit(){           
    display.begin(SSD1306_SWITCHCAPVCC, ADDRESSDISPLAY);    
    display.setTextSize(1);
    display.setTextColor(WHITE);    
}

void DisplayPrint(String Datatext,uint8_t x, uint8_t y, bool Cleardisplay){  

  if(Cleardisplay) display.clearDisplay();  
  display.setCursor(x,y);
  display.println(Datatext);
  display.display(); 
  vTaskDelay(10);

}