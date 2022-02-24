/*
 * Estado ---> Eventos ----> Acciones
 * Mejor para el timer actualizar el tiempo cada segundo
 */
#include "SSD1306Wire.h"

#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48); 

void setup() 
{
   Serial.begin(115200);
   
   digitalWrite(LED_COUNT, LOW);
   pinMode(LED_COUNT, OUTPUT);
   
   pinMode(UP_BTN, INPUT_PULLUP);
   pinMode(DOWN_BTN, INPUT_PULLUP);
   pinMode(ARM_BTN, INPUT_PULLUP);
   
   display.init();
   display.setContrast(255);
   display.setTextAlignment(TEXT_ALIGN_LEFT);
   display.setFont(ArialMT_Plain_16);
   display.clear();
}

void task()
  {
    enum class bombStates {ARMED, DISARMED};
    static bombStates bombState =  bombStates::DISARMED;
    static uint8_t timer = 20;
    
  
    switch (bombState)
    {
      case bombStates::DISARMED:
      {
        
        display.clear();
        display.drawString(10, 20, String(timer));  
        display.display();

        if (digitalRead(UP_BTN);
        
        break;
      }
  
      case bombStates::ARMED:
      {
        
       break;
      }
    }
  }
void loop() 
  {
    
  }
