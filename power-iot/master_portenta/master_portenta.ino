/*
 * inspired from ___ https://github.com/amrithvenkat12/power-iot/blob/master/power-iot/D6T-only/D6T-only.ino
 * Considers the Portenta H7 used as the master. 
 * The Slave used here is the Arduino DUE with 2 I2C lines.


*/


#include <Wire.h> //Include library

#define SLAVE 0x0B //Define SLAVE address. can be 0-127 in hex

float t=0,p=0,al=0,h=0; //Defining variables that are received via I2C

void setup() {
  Wire.begin(); //Begin the Wire class
  Serial.begin(115200); //Begin the serial monitor
  delay(500);
   }

void requestfrm()//To obtain I2C values
{
  char temp[26]; //temp char array to hold incoming values. change value according to your requirement
  int i=0;
  Wire.requestFrom(SLAVE,27); //request 27 bytes from slave at SLAVE address
  while(Wire.available())
  {
    temp[i]=Wire.read(); // Store the received values in char array 'temp'
    i++;
  }
  //Serial.println(temp);
  String data[4]={"0","0","0","0"}; //Declaring an array of strings 'data'. Increase with increasing variables.
  i=0;
  int j=0;
  
  while(temp[i]!='\n') //Outer delimter from the incoming data to indicate EoT. change according to wish
  {
    while(temp[i]!='n') //inner delimiter to indicate end of individual data
    {
    data[j]+=temp[i]; //storing in 'data' string array
    i++;
    if(i>27)
    break;
    }//end of inner loop to delimit individual data
    //Serial.println(data[j]);
    i++; //to skip the inner delimiter
    j++; //to move to next data array
    if(i>27)
     break;
  }

  //to store the data converted to float datatype in global variables 
  t=data[0].toFloat();
  p=data[1].toFloat();
  al=data[2].toFloat();
  h=data[3].toFloat();

  //printing the variables in Serial Monitor
  Serial.println();
  Serial.println(t);
  Serial.println(p);
  Serial.println(al);
  Serial.println(h);
}


//main loop
void loop() {
  delay(2000);
  requestfrm();// calls the requestfrm Method
  }
