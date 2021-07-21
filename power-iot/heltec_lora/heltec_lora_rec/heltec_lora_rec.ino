/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"


char temp[26];
float t=0,p=0,al=0,h=0;

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
void setup() {
    //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

}

void drawFontFaceDemo(float T,float P,float A,float H) {
    
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
  // try to parse packet
  int i=0;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    // read packet
    while (LoRa.available()) {
      temp[i]=(char)LoRa.read();
      i++;
      Serial.print(temp);
    }
  }
    //Serial.println(temp);
  String data[4]={"0","0","0","0"}; //Declaring an array of strings 'data'. Increase with increasing variables.
  i=0;
  int j=0;
  
  while(temp[i]!='x') //Outer delimter from the incoming data to indicate EoT. change according to wish
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

  delay(500);
  drawFontFaceDemo(t,p,al,h) ;

  
}
