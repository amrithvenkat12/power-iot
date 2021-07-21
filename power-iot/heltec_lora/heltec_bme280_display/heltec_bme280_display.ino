/* Heltec Automation  BMP180 Sensors test example
 *
 * Function:
 * Temperature and Pressure measurement
 * 
 * HelTec AutoMation, Chengdu, China
 * www.heltec.org
 *
 * this project also realess in GitHub:
 * https://github.com/HelTecAutomation/ASR650x-Arduino
 * 
*/
#include "Arduino.h"
#include "heltec.h" 
#include <Wire.h>
#include <Adafruit_Sensor.h>//use both libraries  
#include <Adafruit_BME280.h>
#include "string.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define BAND    868E6


//char a[26]; //char array to store the values to be sent
String sendto; // string of delimited values

Adafruit_BME280 bme; // I2C for BME280


void setup() {
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext,LOW);
  Serial.begin(115200);
  Wire.begin(SDA_OLED, SCL_OLED);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
}
void drawFontFaceDemo(double T,double P,double A,double H) {
    
    Heltec.display->flipScreenVertically();
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0   , 0  ,   "Temperature =");
    Heltec.display->drawString(76  , 0  ,   (String)T);
    Heltec.display->drawString(106 , 0  ,   " *C");
    Heltec.display->drawString(0   , 16 ,   "Pressure =");
    Heltec.display->drawString(56  , 16 ,   (String)P);
    Heltec.display->drawString(100 , 16 ,    "Pa");
    Heltec.display->drawString(0   , 32 ,   "Altitude   =");
    Heltec.display->drawString(56  , 32 ,   (String)A);
    Heltec.display->drawString(86 , 32 ,   " m");
    Heltec.display->drawString(0   , 48 ,   "Humidity =");
    Heltec.display->drawString(76  , 48 ,   (String)H);
    Heltec.display->drawString(106 , 48 ,   " %");
    Heltec.display->display();
}  
void loop() {
    if (!bme.begin(0x76,&Wire)) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
    double T =bme.readTemperature();
    Serial.print("Temperature = ");
    Serial.print(T);
    Serial.println(" *C");

    double P =  bme.readPressure()/100.0F;
    Serial.print("Pressure = ");
    Serial.print(P);
    Serial.println(" Pa");

    double A = bme.readAltitude(SEALEVELPRESSURE_HPA);
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(A);
    Serial.println(" meters");

    double H = bme.readHumidity();
    Serial.print("Humidity = ");
    Serial.print(H);
    Serial.println(" %");
    Serial.println();

  sendto = String(T);
  sendto += 'n';//inner delimiter
  sendto += String(P);
  sendto += 'n';
  sendto += String(A);
  sendto += 'n';
  sendto += String(H);
  sendto +='n';
  sendto += 'x';//Outer final delimiter
  
//  int x = sendto.length();
//  sendto.toCharArray(a,x+1); 
  
  delay(500);
	drawFontFaceDemo(T, P, A, H) ;


  LoRa.beginPacket();
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(sendto);
//  LoRa.print(counter);
  LoRa.endPacket();
  
	delay(3000);
}
