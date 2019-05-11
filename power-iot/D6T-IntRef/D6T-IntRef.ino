//#include <Event.h>
//#include <Timer.h>

#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

//String apiKey = "BHOJVKKIJVMBSR2Z";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Amrith Venkat";     // replace with your wifi ssid and wpa2 key
const char *pass =  "12345678";
//const char* server = "api.thingspeak.com";
//Timer t;

HTTPClient machine;
#define D6T_ID 0x0A
#define D6T_CMD 0x4C
#define SLAVE 0x0B
const int ledPin = LED_BUILTIN;
int ReadBuffer[35],x ;
char receive[50];
float ptat,curr=0,vol=0,powers=0,temper=0,humi=0 ;
float tempC[16], tempF[16], ref[16], t1, t2, j=0 ;
String tempS[16];
//
//WiFiClient client;

void setup() {
  Wire.begin(0,2);//D3/0->SDA D4/2->SCL
  pinMode(ledPin,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(500);
  //refsensor();
  WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
      //t.every(10000,dataupdate);
 
}
//
//void requestfrm()
//{
//  char temp[30];
//  int i=0;
//  Wire.requestFrom(0x0B,29);
//  while(Wire.available())
//  {
//    temp[i]=Wire.read();
//    i++;
//  }
//  Serial.println(temp);
//  String data[5]={"0","0","0","0","0"};
//  i=0;
//  int j=0;
//  
//  while(temp[i]!='\n')
//  {
//    while(temp[i]!='n')
//    {
//    data[j]+=temp[i];
//    i++;
//    }
//    Serial.println(data[j]);
//    i++;
//    j++;
//    if(i>30)
//     break;
//  }
//  
//  curr=data[0].toFloat();
//  vol=data[1].toFloat();
//  powers=data[2].toFloat();
//  temper=data[3].toFloat();
//  humi=data[4].toFloat();
//  Serial.println(curr);
//  Serial.println(vol);
//  Serial.println(powers);
//  Serial.println(temper);
//  Serial.println(humi);
//}

void refsensor(void)
{
  int i=0;
  Wire.beginTransmission(D6T_ID) ;
  Wire.write(D6T_CMD) ;
  Wire.endTransmission(D6T_ID) ;
  Wire.requestFrom(D6T_ID,35) ;
  for( i=0 ; i<35 ; i++ )
  {
    ReadBuffer[i] = Wire.read() ;
  }
  ptat = ( ReadBuffer[0] + ( ReadBuffer[1] * 256 ) ) * 0.1 ;
  for( i=0 ; i<16 ; i++ )
  {
    tempC[i] = ( ReadBuffer[i*2+2] + ( ReadBuffer[i*2+3] * 256 ) ) * 0.1 ;
    tempF[i] = ( tempC[i] * 9.0/5.0 ) + 32.0 ;
  }
   if( ( ( ptat * 9.0 / 5.0 ) + 32.0 ) > 0 )
   {
  for( i = 0 ; i < 16 ; i++ )
    {
      ref[i] = tempF[i] ;
    }
   }
}

int presence(void)
{
  int i ;
  int flag=0;
  
  Wire.beginTransmission(D6T_ID) ;
  Wire.write(D6T_CMD) ;
  Wire.endTransmission(D6T_ID) ;
  Wire.requestFrom(D6T_ID,35) ;
  for( i=0 ; i<35 ; i++ )
  {
    ReadBuffer[i] = Wire.read() ;
    Serial.println(ReadBuffer[i]);
  }
  ptat = ( ReadBuffer[0] + ( ReadBuffer[1] * 256 ) ) * 0.1 ;
  //Serial.println() ;
  for( i=0 ; i<16 ; i++ )
  {
    tempC[i] = ( ReadBuffer[i*2+2] + ( ReadBuffer[i*2+3] * 256 ) ) * 0.1 ;
    tempF[i] = ( tempC[i] * 9.0/5.0 ) + 32.0 ;
    Serial.println(tempC[i]);
   }
  if( ( ( ptat * 9.0 / 5.0 ) + 32.0 ) > 0 )
  {
      //Serial.println();
    for(i=0;i<16;i++)
    {
      //if( (i%4)==0 )
      tempS[i] = String(tempC[i]);
//      Serial.print(tempS[i]);
      
      
//      t1=ptat+1.9;
//      t2=ptat-1.9;
//      if( ( tempF[i] > t1 ) || ( tempF[i] < t2 ) )
//      {
//       Serial.print("0");
//        flag=1;
//      }
Serial.println();
    }  
        
  
//    if(flag==1)
//    {
//      //digitalWrite(ledPin,HIGH);
//      Serial.println("ON");
//      return 1;
//    }
//    else
//    {
//      //digitalWrite(ledPin,LOW);
//      Serial.println("OFF");
//      return 0;
//    }
  }
    
  
  delay(500) ;
}

//void dataupdate()
//{
//  sendthingspeak(curr,vol,powers,temper,humi);
//}
//
//void sendthingspeak(float current,float volt,float power,float humidity,float temperature)
//{
//   if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
//   {
//    String postStr=apiKey;
//    postStr +="&field1=";
//    postStr += String(current);
//    postStr +="&field2=";
//    postStr += String(volt);
//    postStr +="&field3=";
//    postStr += String(power);
//    postStr +="&field4=";
//    postStr += String(humidity);
//    postStr +="&field5=";
//    postStr += String(temperature);
//    postStr +="&field6=";
//    postStr += String(presence());
//    postStr += "\r\n\r\n";
//
//    client.print("POST /update HTTP/1.1\n");
//    client.print("Host: api.thingspeak.com\n");
//    client.print("Connection: close\n");
//    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
//    client.print("Content-Type: application/x-www-form-urlencoded\n");
//    client.print("Content-Length: ");
//    client.print(postStr.length());
//    client.print("\n\n");
//    client.print(postStr);
//   }
//   client.stop();
//}
void posts(){
  String url="http://192.168.43.164/seed/write.php?v0=";
  url+=tempS[0];
  url+="&v1=";
  url+=tempS[1];
  url+="&v2=";
  url+=tempS[2];
  url+="&v3=";
  url+=tempS[3];
  url+="&v4=";
  url+=tempS[4];
  url+="&v5=";
  url+=tempS[5];
  url+="&v6=";
  url+=tempS[6];
  url+="&v7=";
  url+=tempS[7];
  url+="&v8=";
  url+=tempS[8];
  url+="&v9=";
  url+=tempS[9];
  url+="&v10=";
  url+=tempS[10];
  url+="&v11=";
  url+=tempS[11];
  url+="&v12=";
  url+=tempS[12];
  url+="&v13=";
  url+=tempS[13];
  url+="&v14=";
  url+=tempS[14];
  url+="&v15=";
  url+=tempS[15];
   
   if(WiFi.status()==WL_CONNECTED){
machine.begin(url);
Serial.println(url);
int httpCode=machine.GET();
Serial.println(httpCode);
if(httpCode>0)
{
  Serial.println("Sent to Cloud");
}
//else
//{
//  Serial.println("Not sent");
//}
machine.end();
}}
void loop() {
  //t.update();
  x=presence();
//  if(x==1)
//  {
//    digitalWrite(5,LOW);
//    Serial.println("Light ON");
//  }
//  else
//  {
//    digitalWrite(5,HIGH);
//    Serial.println("Light OFF");
//  }
 posts();//  requestfrm();
}
