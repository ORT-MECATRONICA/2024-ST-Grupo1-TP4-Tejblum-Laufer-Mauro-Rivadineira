//GRUPO 1-TEJBLUM-LAUFER-RIVADINEIRA-MAURO
#include <Preferences.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define SENSORT 23
#define BTN1 35
#define BTN2 34

DHT dht(SENSORT, DHT11);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//estados
#define PANTALLATEMPERATURA 0
#define VBTN1 1
#define PANTALLAUMBRAL 2
#define UMBRALMAS 3
#define VBTN2 4

int pantallas;
int temperatura;
int umbral = 20;
unsigned long tiempo = 0;
char stringumbral[5];
char stringtemp[5];
Preferences preferences;  // creacion de una instancia

void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  Serial.begin(9600);
  u8g2.begin();
  dht.begin();
  pinMode(SENSORT, INPUT);
  preferences.begin("guardado", false);              // crea un espacio para el guardado. namespace{key:value}
  umbral = preferences.getUInt("valor_umbral", 20);  // Si no hay, se crea la llave y se le asigna un valor inicial.
}

void maquinaEstado() {

  switch (pantallas) {

    case PANTALLATEMPERATURA:

      pantalla1();
      Serial.println("PANTALLA TEMPERATURA");
      if (digitalRead(BTN1) == LOW) {
        tiempo = millis();
        pantallas = VBTN1;
      }
      break;

    case VBTN1:

      pantalla1();
      Serial.print("VBTN1");
      if (digitalRead(BTN1) == HIGH && millis() - tiempo < 5000) {
        pantallas = PANTALLATEMPERATURA;
      }
      if (digitalRead(BTN1) == HIGH && millis() - tiempo > 5000) {
        tiempo = millis();
        pantallas = PANTALLAUMBRAL;
      }
      break;

    case PANTALLAUMBRAL:

      pantalla2();
      Serial.println("PANTALLA UMBRAL");
      if (digitalRead(BTN1) == LOW && digitalRead(BTN2) == HIGH) {
        pantallas = UMBRALMAS;
      }
      if (digitalRead(BTN1) == HIGH && digitalRead(BTN2) == LOW) {
        tiempo = millis();
        pantallas = VBTN2;
      }
      break;

    case UMBRALMAS:

      Serial.println("UMBRAL MAS");
      if (digitalRead(BTN1) == HIGH && digitalRead(BTN2) == HIGH) {
        umbral++;
        tiempo = millis();
        pantallas = PANTALLAUMBRAL;
      }
      break;

    case VBTN2:

      pantalla2();
      Serial.println("VBTN2");
      if (millis() - tiempo < 5000 && digitalRead(BTN1) == HIGH && digitalRead(BTN2) == HIGH) {
        umbral--;
        tiempo = millis();
        pantallas = PANTALLAUMBRAL;
      }
      if (millis() - tiempo > 5000 && digitalRead(BTN1) == HIGH && digitalRead(BTN2) == HIGH) {
        tiempo = millis();
        preferences.putUInt("valor_umbral", umbral);  // Se guarda el ultimo valor
        pantallas = PANTALLATEMPERATURA;
      }
      break;
  }
}
void pantalla1() {
  temperatura = dht.readTemperature();
  u8g2.clearBuffer();                  // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  sprintf(stringtemp, "%d", temperatura);
  u8g2.drawStr(15, 20, "Temperatura:");
  u8g2.drawStr(70, 20, stringtemp);
  u8g2.drawStr(90, 20, "C");
  u8g2.drawStr(15, 50, "Umbral:");
  sprintf(stringumbral, "%d", umbral);
  u8g2.drawStr(70, 50, stringumbral);
  u8g2.drawStr(90, 50, "C");
  u8g2.sendBuffer();
}

void pantalla2() {
  u8g2.clearBuffer();                  // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  u8g2.drawStr(15, 50, "Umbral:");
  sprintf(stringumbral, "%d", umbral);
  u8g2.drawStr(70, 50, stringumbral);
  u8g2.drawStr(90, 50, "C");
  u8g2.sendBuffer();
}

void loop() {
  maquinaEstado();
}