/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/
/*Inspired from __ https://github.com/amrithvenkat12/power-iot/blob/master/power-iot/ACS712/ACS712.ino
 * Implements both I2C lines. 

*/


//include libraries
#include <Wire.h>
#include <Adafruit_Sensor.h>//use both libraries  
#include <Adafruit_BME280.h>


#define SEALEVELPRESSURE_HPA (1013.25) //define pressure at Sea Level 

Adafruit_BME280 bme; // I2C for BME280


char a[26]; //char array to store the values to be sent
String sendto; // string of delimited values


void setup(){
Wire.begin();// SDA - pin 20, SCL -pin 21 ie. I2C 0

Serial.begin(115200); //Begin the serial port
bme.begin(0x76,&Wire); //begin the BME280 sensor with wire instance of i2c line 0 passed

Wire1.onRequest(requestEvent); //SDA1, SCL1 ie. I2C 1
Wire1.begin(0x0B); // begin the wire1 instance with the slave address. can be 0-127 hex. make sure it doesn't clash with other device addresses.
}


//main loop
void loop(){
  requestfrm();//call the requestfrm function to measure  and send values
  delay(2000);
}



void requestfrm()
{
  float t,p,al,h; //initialize values to be measured
  
  t=bme.readTemperature();
  p = bme.readPressure() / 100.0F;
  al=bme.readAltitude(SEALEVELPRESSURE_HPA);
  h=bme.readHumidity();
  
  //append the sendto string with the string formatted values that are measured
  sendto = String(t);
  sendto += 'n';//inner delimiter
  sendto += String(p);
  sendto += 'n';
  sendto += String(al);
  sendto += 'n';
  sendto += String(h);
  sendto +='n';
  sendto += '\n';//Outer final delimiter
  
  int x = sendto.length(); //find length of the string
  
  sendto.toCharArray(a,x+1); //Convert to character array
  Serial.println(x);
  Serial.println(a);
}

//request event handler
void requestEvent()
{
  int x =0;
 x=Wire1.write(a); //Write the character arrray value on the I2C 1 line, returns the number of bytes sent 
 Serial.println("Excuted byte");
 Serial.print(x);//print the number of bytes sent
}
