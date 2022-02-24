/*
 * Estado ---> Eventos ----> Acciones
 * Mejor para el timer actualizar el tiempo cada segundo
 */
#include "SSD1306Wire.h"

#define BOMB_OUT 25//Led 1
#define LED_COUNT 26//Led 2
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void bntsTask();
void bombTask();
void serialTask();

void setup() {
  serialTask();
  bntsTask(); //me avisa que botón a sido pulsado
  bombTask();
}


boolean evBtns = false; //Nos avisa cuando el evento Btns está activo...
uint8_t evBtnsData = 0; //nos dece que botón se presionó

void loop() {
  serialTask();
  bntsTask(); //me avisa que botón a sido pulsado
  bombTask();
}

void bntsTask() {
  enum class BntsStates {INIT, WAITING_PRESS, WAITING_STABLE, WAITING_RELEASE};
  static BntsStates bntsStates = BntsStates::INIT;
  static uint32_t referenceTime;
  const uint32_t STABLETIMEOUT = 100;

  switch (bntsStates) {
    case BntsStates::INIT: {
        pinMode(UP_BTN, INPUT_PULLUP); //Decimos que son pulsadores
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(ARM_BTN, INPUT_PULLUP);
        bntsStates = BntsStates::WAITING_PRESS;
        break;
      }
    case BntsStates::WAITING_PRESS: {
        if (digitalRead(DOWN_BTN) == LOW) {
          referenceTime = millis();
          bntsStates = BntsStates::WAITING_STABLE;
        }
        break;
      }
    case BntsStates::WAITING_STABLE: {
        if (digitalRead(DOWN_BTN) == HIGH) {
          bntsStates = BntsStates::WAITING_PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          bntsStates = BntsStates::WAITING_RELEASE;
        }
        break;
      }
    case BntsStates::WAITING_RELEASE: {
        if (digitalRead(DOWN_BTN) == HIGH) {
          evBtns = true;
          evBtnsData = DOWN_BTN;
          Serial.println("DOWN_BTN");
          bntsStates = BntsStates::WAITING_PRESS;
        }
        break;
      }
    default:
      break;

  }
}

void bombTask() {

  enum class BombStates {INIT, WAITING_CONFIG, COUNTING, BOOM};
  static BombStates bombStates = BombStates::INIT;
  static uint8_t counter;

  switch (bombStates) {
    case BombStates::INIT: {
        pinMode(LED_COUNT, OUTPUT);
        pinMode(BOMB_OUT, OUTPUT);
        display.init(); //Para usar la pantalla
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        counter = 20;

        display.clear();
        display.drawString(0, 5, String(counter));
        display.display();
        bombStates = BombStates::WAITING_CONFIG;
        break;
      }
    case BombStates::WAITING_CONFIG: {

        if (evBtns == true) {
          evBtns = false;

          if (evBtnsData == DOWN_BTN) {
            if (counter > 10) {
              counter--;
            }
            display.clear();
            display.drawString(0, 5, String(counter));
            display.display();
          }

        }
        break;
      }
    case BombStates::COUNTING: {
        break;
      }
    case BombStates::BOOM: {
        break;
      }
    default:
      break;

  }

}

void serialTask() {
  enum class SerialStates {INIT, READING_COMMANDS};
  static SerialStates serialStates = SerialStates::INIT;

  switch (serialStates) {
    case SerialStates::INIT: {
        Serial.begin(115200);
        serialStates = SerialStates::READING_COMMANDS;
        break;
      }
    case SerialStates::READING_COMMANDS: {

        if (Serial.available() > 0) {
          int dataIn = Serial.read();
          if (dataIn == 'd') {
            evBtns = true;
            evBtnsData = DOWN_BTN;
            Serial.println("DOWN_BTN");
          }
        }

        break;
      }
    default:
      break;

  }
}
