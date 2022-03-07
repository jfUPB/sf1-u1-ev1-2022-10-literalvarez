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
        
        const uint32_t interval = 500;
        static uint8_t passwordCounter = 0;
        const uint8_t passwordMax = 7;
        static uint8_t password [passwordMax] = {UP_BTN, UP_BTN, DOWN_BTN, DOWN_BTN, UP_BTN, DOWN_BTN, ARM_BTN}; //<- Defusing password
        static uint8_t passwordTry [passwordMax] = {0};
        static uint32_t previousMillis = 0;
        static uint8_t led_countState = LOW;
        bool isPasswordCorrect = false;


        uint32_t currentMillis = millis();
        bool boomState = false;

        if (currentMillis - previousMillis >= interval)
        {
          previousMillis = currentMillis;
          if (led_countState == LOW)
          {
            led_countState = HIGH;
          }
          else
          {
            led_countState = LOW;
          }
          digitalWrite(LED_COUNT, led_countState);
          if (led_countState == HIGH)
          {
            counter--;
            display.clear();
            display.drawString(20, 0, String(counter));
            display.display();
          }
          if (counter == 0) 
          {
          bombState = BombStates::BOOM;
          }

          // --------------------------DEFUSING---------------------------
          if (evBtns == true) 
          {
            evBtns = false;
            if (passwordCounter < passwordMax) 
            {
              if (evBtnsData == UP_BTN) 
              {
                passwordTry[passwordCounter] = UP_BTN;
                Serial.println("Up");
              }
              else if (evBtnsData == DOWN_BTN) 
              {
                passwordTry[passwordCounter] = DOWN_BTN;
                Serial.println("Down");
              }
              else if (evBtnsData == ARM_BTN) 
              {
                passwordTry[passwordCounter] = ARM_BTN;
                Serial.println("Arm");
              }
              passwordCounter++;
            }
          }
          else if (passwordCounter == passwordMax) 
          {
            DefuseTask (passwordTry, password, passwordMax, &isPasswordCorrect);
            if (isPasswordCorrect == true) 
            {

              Serial.println("Correct pass");
              display.clear();
              display.drawString(0, 5, String("Disarm"));
              display.display();
              for (uint8_t k = 0; k < passwordMax; k++)
              {
                passwordTry[k] = 0;
              }
              delay(3000);
              bombState = BombStates::INIT;
            }
            else 
            {
              Serial.println("Wrong Pass");
              passwordCounter = 0;
              for (uint8_t j = 0; j < passwordMax; j++) 
              {
                passwordTry[j] = 0;
              }
              delay(500);
            }
          }
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
void DefuseTask (uint8_t *passTry, uint8_t *pass, uint8_t passMax, bool *isCorrect) //<--------------- Defusing task
{
  for (uint8_t i = 0; i < passMax; i++)
  {
    if (pass[i] == passTry[i])
    {
      *isCorrect = true;
    }
    else
    {
      *isCorrect = false;
      break;
    }
  }
}
