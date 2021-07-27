#include <TinyGPS.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

String apiKey = "BHOJVKKIJVMBSR2Z";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Airtel-MW40-244B";     // replace with your wifi ssid and wpa2 key
const char *pass =  "73742290";
const char* server = "api.thingspeak.com";

#define D6T_ID 0x0A
#define D6T_CMD 0x4C
#define SLAVE 0x0B
const int ledPin = LED_BUILTIN;
int ReadBuffer[35],x,flag=0 ;
char receive[50];
float ptat,curr=0,vol=0,powers=0,temper=0,humi=0 ;
float tempC[16], tempF[16], ref[16], t1, t2, j=0 ;

WiFiClient client;

void setup() {
  Wire.begin(0,2);//0->SDA 2->SCL
  pinMode(ledPin,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(5,LOW);
  Serial.begin(115200);
  delay(500);
  refsensor();
  WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}

void requestfrm()
{
  char temp[32];
  int i=0;
  Wire.requestFrom(0x0B,32);
  while(Wire.available())
  {
    temp[i]=Wire.read();
    i++;
  }
  //Serial.println(temp);
  String data[5]={"0","0","0","0","0"};
  i=0;
  int j=0;
  
  while(temp[i]!='\n')
  {
    while(temp[i]!='n')
    {
    data[j]+=temp[i];
    i++;
    if(i>32)
    break;
    }
    //Serial.println(data[j]);
    i++;
    j++;
    if(i>32)
     break;
  }
  
  curr=data[0].toFloat();
  vol=data[1].toFloat();
  powers=data[2].toFloat();
  temper=data[3].toFloat();
  humi=data[4].toFloat();
  Serial.println(curr);
  Serial.println(vol);
  Serial.println(powers);
  Serial.println(temper);
  Serial.println(humi);
}

void refsensor(void)
{
  int i=0;
  int z=0;
  while(z==0)
  {
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
    z=1;
  for( i = 0 ; i < 16 ; i++ )
    {
      ref[i] = tempF[i] ;
    }
   }
  }
}

int presence(void)
{
  int i ;
  Serial.println("IR Request Begin");
  Wire.beginTransmission(D6T_ID) ;
  Wire.write(D6T_CMD) ;
  Wire.endTransmission(D6T_ID) ;
  Wire.requestFrom(D6T_ID,35) ;
  for( i=0 ; i<35 ; i++ )
  {
    ReadBuffer[i] = Wire.read() ;
  }
  Serial.println("IR Received");
  ptat = ( ReadBuffer[0] + ( ReadBuffer[1] * 256 ) ) * 0.1 ;
  Serial.println() ;
  for( i=0 ; i<16 ; i++ )
  {
    tempC[i] = ( ReadBuffer[i*2+2] + ( ReadBuffer[i*2+3] * 256 ) ) * 0.1 ;
    tempF[i] = ( tempC[i] * 9.0/5.0 ) + 32.0 ;
  }
  if( ( ( ptat * 9.0 / 5.0 ) + 32.0 ) > 0 )
  {
    flag=0;
      Serial.println();
    for(i=0;i<16;i++)
    {
      
      if( (i%4)==0 )
      Serial.println();
      t1=ref[i]+1.9;
      t2=ref[i]-1.9;
      if( ( tempF[i] > t1 ) || ( tempF[i] < t2 ) )
      {
       Serial.print("0");
        flag=1;
      }
      else
      {
        Serial.print("1");
        
      }
      Serial.print("  "); 
    }  
        //
       
      Serial.println();
        //
  }
  else
  {
    delay(100);
  }
    
  if(flag==1)
    {
      digitalWrite(5,HIGH);
      Serial.println("ON");
      return 1;
    }
    else
    {
      digitalWrite(5,LOW);
      Serial.println("OFF");
      return 0;
    }
  delay(500) ;
}


void sendthingspeak(float current,float volt,float power,float humidity,float temperature,int pres)
{
  Serial.println("Uploading");
   if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
   {
    String postStr=apiKey;
    postStr +="&field1=";
    postStr += String(current);
    postStr +="&field2=";
    postStr += String(volt);
    postStr +="&field3=";
    postStr += String(power);
    postStr +="&field4=";
    postStr += String(humidity);
    postStr +="&field5=";
    postStr += String(temperature);
    postStr +="&field6=";
    postStr += String(pres);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("Upload Complete");
   }
   client.stop();
   Serial.println("Client Disconnection");
}

void loop() {
  int x=presence();
  requestfrm();
  sendthingspeak(curr,vol,powers,temper,humi,x);
//  char z=(int)x;
//  Wire.beginTransmission(0x0B);
//  Wire.write(z);
//  Wire.endTransmission();
}
