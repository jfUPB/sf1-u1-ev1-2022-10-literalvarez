#include "SSD1306Wire.h"
#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 32
#define DOWN_BTN 33
#define ARM_BTN 13

bool evBtns = false;
uint8_t evBtnsData = 0;

void btnsTask();
void bombTask();

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void setup() 
{
  btnsTask();
  bombTask();
  Serial.begin(115200);
}

void loop() {
  btnsTask();
  bombTask();
}
void disarmTask() 
{

}
void btnsTask() 
{
  enum class BtnsStates {INIT, WAITING_PRESS , WAITING_STABLE, WAITING_RELEASE};
  static BtnsStates btnsState =  BtnsStates::INIT;
  static uint32_t referenceTime;
  const uint32_t STABLETIMEOUT = 100;
  static uint8_t PressedBtn;

  switch (btnsState) 
  {
    case BtnsStates::INIT: 
    {
        pinMode(ARM_BTN, INPUT_PULLUP);
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(UP_BTN, INPUT_PULLUP);
        btnsState = BtnsStates::WAITING_PRESS;
        break;
      }
    case BtnsStates::WAITING_PRESS: 
    {

        if (digitalRead(UP_BTN) == LOW) 
        {
          referenceTime = millis();
          PressedBtn = UP_BTN;
          btnsState = BtnsStates::WAITING_STABLE;

        }
        else if (digitalRead(DOWN_BTN) == LOW) 
        {
          PressedBtn = DOWN_BTN;
          referenceTime = millis();
          btnsState = BtnsStates::WAITING_STABLE;

        }
        else if (digitalRead(ARM_BTN) == LOW) 
        {
          PressedBtn = ARM_BTN;
          referenceTime = millis();
          btnsState = BtnsStates::WAITING_STABLE;
        }

        break;
      }
    case BtnsStates::WAITING_STABLE: 
    {
        if (PressedBtn == UP_BTN) 
        {
          if (digitalRead(UP_BTN) == HIGH) 
          {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) 
          {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }
        else if (PressedBtn == DOWN_BTN) 
        {
          if (digitalRead(DOWN_BTN) == HIGH) 
          {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) 
          {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }
        else if (PressedBtn == ARM_BTN) 
        {
          if (digitalRead(ARM_BTN) == HIGH) 
          {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) 
          {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }

        break;
      }
    case BtnsStates::WAITING_RELEASE: 
    {

        if (PressedBtn == UP_BTN) 
        {
          if (digitalRead(UP_BTN) == HIGH) 
          {
            evBtns = true;
            evBtnsData = UP_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("UP_BTN");
          }
        }
        else if (PressedBtn == DOWN_BTN) 
        {
          if (digitalRead(DOWN_BTN) == HIGH) 
          {
            evBtns = true;
            evBtnsData = DOWN_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("DOWN_BTN");
          }
        }
        else if (PressedBtn == ARM_BTN) 
        {
          if (digitalRead(ARM_BTN) == HIGH) 
          {
            evBtns = true;
            evBtnsData = ARM_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("ARM_BTN");
          }
        }

        break;
      }
    default:
      Serial.println("ERROR.");
      break;
  }

}


void bombTask() 
{
  enum class BombStates {INIT, CONFIG, ARMED, BOOM};
  static BombStates bombState =  BombStates::INIT;
  static uint8_t counter = 20;

  switch (bombState) 
  {

    case BombStates::INIT: 
    {        
        pinMode(LED_COUNT, OUTPUT);
        pinMode(BOMB_OUT, OUTPUT);

        digitalWrite(LED_COUNT, HIGH);
        digitalWrite(BOMB_OUT, LOW);

        counter = 20;
        display.init();
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.clear();
        display.drawString(20, 0, String(counter));
        display.display();
        

        bombState = BombStates::CONFIG;

        break;
      }

    case BombStates::CONFIG: 
    {

        if (evBtns == true) 
        {
          evBtns = false;

          if (evBtnsData == UP_BTN) 
          {
            if (counter < 60) 
            {
              counter++;
            }
            display.clear();
            display.drawString(20, 0, String(counter));
            display.display();
          }
          else if (evBtnsData == DOWN_BTN) 
          {
            if (counter > 10) 
            {
              counter--;
            }
            display.clear();
            display.drawString(20, 0, String(counter));
            display.display();
          }
          else if (evBtnsData == ARM_BTN) 
          {
            bombState = BombStates::ARMED;
            Serial.println("BombStates::ARMED");
          }
        }

        break;
      }

    case BombStates::ARMED: 
    {
        const uint32_t interval = 400;

        static uint32_t previousMillis = 0;
        static uint8_t ledState_BOMB_OUT = LOW;

        uint32_t currentMillis = millis();
        bool boomState = false;

        if (currentMillis - previousMillis >= interval) 
        {
          previousMillis = currentMillis;
          if (ledState_BOMB_OUT == LOW) 
          {
            ledState_BOMB_OUT = HIGH;
          }
          else 
          {
            ledState_BOMB_OUT = LOW;
          }
          digitalWrite(LED_COUNT, ledState_BOMB_OUT);
          if (ledState_BOMB_OUT == HIGH) {
            counter--;
            display.clear();
            display.drawString(20, 0, String(counter));
            display.display();
          }
          if(Serial.available() >= 2)
          {
            int dataRx1 = Serial.read();
            int dataRx2 = Serial.read();
            int dataRx3 = Serial.read();
          }
        }
        else if (counter == 0) 
        {
          bombState = BombStates::BOOM;          
        }
        break;
      }
     case BombStates::BOOM:
     {
        digitalWrite(LED_COUNT, LOW);
        digitalWrite(BOMB_OUT, HIGH);
        display.clear();
        display.drawString(9, 0, "BOOM!");
        display.display();

        delay(2500);
        
        bombState = BombStates::INIT;
     }
    default:
    
      break;
  }

}
