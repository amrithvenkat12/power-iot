//#include <Servo.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//String apiKey = "BHOJVKKIJVMBSR2Z";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Amrith Venkat";     // replace with your wifi ssid and wpa2 key //Amrith Venkat //Airtel-MW40-244B
const char *pass =  "12345678";                       // 12345678  // 73742290
//const char* server = "api.thingspeak.com";
const char* phpclient = "bassrec.azurewebsites.net";

#define D6T_ID 0x0A
#define D6T_CMD 0x4C
#define SLAVE 0x0B
const int ledPin = LED_BUILTIN;
int ReadBuffer[36],x,flag=0,tflag=0,fflag=0,servo=0, zebra=45; 
char receive[40];
float ptat,curr=0,vol=0,powers=0,temper=0,humi=0 ;
float tempC[16], tempF[16], t1, t2, j=0, average=0;/*ref[16],*/
String postStr;
//Servo myservo;

WiFiClient client;

void setup() {
  Wire.begin(0,2);//0->SDA 2->SCL
  pinMode(ledPin,OUTPUT);
  pinMode(5,OUTPUT);
  //pinMode(4,OUTPUT);
  digitalWrite(5,LOW);
  Serial.begin(115200);
  delay(500);
//  myservo.attach(4);
//  refsensor();
  WiFi.begin(ssid, pass);
  int temppp=0;
      while ((WiFi.status() != WL_CONNECTED)&&(temppp<10)) 
     {
            delay(500);
            Serial.print(".");
            temppp++;
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

/*void refsensor(void)
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
}*/

int presence(void)
{
  int i ;
  float minimum=0,maximum=0;
  Serial.println("IR Request Begin");
  Wire.beginTransmission(D6T_ID) ;
  Wire.write(D6T_CMD) ;
  Wire.endTransmission(D6T_ID) ;
  Wire.requestFrom(D6T_ID,35) ;
  for( i=0 ; i<35 ; i++ )
  {
    ReadBuffer[i] = Wire.read() ;
    Serial.print(ReadBuffer[i]);
    Serial.print(" ");
  }
  Serial.println("IR Received");
  ptat = ( ReadBuffer[0] + ( ReadBuffer[1] * 256 ) ) * 0.1 ;
  ptat=((ptat*9.0)/5.0)+32;
  Serial.println(ptat);
  Serial.println() ;
  for( i=0 ; i<16 ; i++ )
  {
    tempC[i] = ( ReadBuffer[i*2+2] + ( ReadBuffer[i*2+3] * 256 ) ) * 0.1 ;
    tempF[i] = ( tempC[i] * 9.0/5.0 ) + 32.0 ;
  }
  if ( ptat > 0 )
  {
    flag=0;
    servo=0;
    average=0;
      Serial.println();
      minimum=maximum=tempF[0];
    for(i=0;i<16;i++)
    {
      average=average+tempF[i];
      if(tempF[i]<minimum)
      {
        minimum=tempF[i];
      }
      if(tempF[i]>maximum)
      {
        maximum=tempF[i];
      }
      if( (i%4)==0 )
      {
        Serial.print("  ");
        Serial.print(average);
        Serial.println();
      }
      t1=ptat+0.4;
      t2=ptat-1.0;
      if( ( tempF[i] > t1 ) ||((maximum-minimum)>1.5)) /*|| ( tempF[i] < t2 ))*/
      {
       Serial.print("0");
       
        flag=1;
        
       
       
      }
      else
      {
         
        
        
        Serial.print("1");
        
      }
      Serial.print(tempF[i]);
      Serial.print("  "); 
    }    
        Serial.println();
    Serial.println(average);
    average=average/16;

    Serial.println(average);
     Serial.println(flag); 
     Serial.println(maximum);
     Serial.println(minimum); 
      Serial.println();
        
  }
  else
  {
    delay(100);
  }
    
  if(flag==1)
    {
      tflag++;
      //servo=0;
      //digitalWrite(5,HIGH);
      //Serial.println("ON");
      return 1;
    }
    else
    {
      fflag++;
      //servo=1;
      //digitalWrite(5,LOW);
      //Serial.println("OFF");
      
      return 0;
    }
  delay(500) ;
}


void sendthingspeak(float current,float volt,float power,float humidity,float temperature,int pres)
{
//  Serial.println("Uploading");
//  delay(10);
//   if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
//   {
//    Serial.println("Connected");
//    postStr=apiKey;
//    postStr +="&field1=";
//    postStr += String(current);
//    postStr +="&field2=";
//    postStr += String(volt);
////    postStr +="&field3=";
////    postStr += String(power);
//    postStr +="&field4=";
//    postStr += String(humidity);
//    postStr +="&field5=";
//    postStr += String(temperature);
//    postStr +="&field6=";
//    postStr += String(pres);
//    postStr += "\r\n\r\n";
//    Serial.println(postStr);
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
//    Serial.println("Upload Complete");
//   }
//   client.stop();
//   Serial.println("Client Disconnection");
WiFiClient client;
Serial.println("Begin");
if((WiFi.status() == WL_CONNECTED)&&client.connect(phpclient, 80)){
    Serial.println("Connected to Azure");
    client.print("GET /write_power.php?");
    client.print("volt=");
    client.print(volt);
    client.print("&current=");
    client.print(current);
    client.print("&power=");
    client.print(power);
    client.print("&pres=");
    client.print(pres);
    client.print("&temp=");
    client.print(temperature);
    client.print("&humi=");
    client.print(humidity);
    client.println(" HTTP/1.1");
    client.println("Host:bassrec.azurewebsites.net");
    //client.println("Connection:Keep-Alive");
    client.println();
    client.println();

     while(client.connected())
   {    
      // stay in this loop until the server closes the connection

       while(client.available())
       {
         // The server will not close the connection until it has sent the last packet
         // and this buffer is empty

         char read_char = client.read(); 
         Serial.write(read_char);
       }
       client.stop();
   }
   // close your end after the server closes its end
   
}
Serial.println("END");
}

String getmanual(void)
{
  HTTPClient http;
  http.begin("http://bassrec.azurewebsites.net/getmanual.php");
  int returns = http.GET();

  if(returns>0)
  {
    String values=http.getString();
    Serial.println(values);
    return values;
  }
  return "0";
}


void loop() {
  if(tflag>2||fflag>2)
  {
    tflag=0;
    fflag=0;
  }
  String stat=getmanual();
  Serial.println(stat);
  if(stat=="2")
  {
  if(tflag<fflag)
  {
    digitalWrite(5,LOW);
    Serial.println("OFF");
  }
  else if(tflag>fflag)
  {
    digitalWrite(5,HIGH);
    Serial.println("ON");
    
  }
  else
  {
    tflag=0;
    fflag=0;
  }
  }
  else if(stat=="0")
  {
    digitalWrite(5,LOW);
  }
  else if(stat=="1")
  {
    digitalWrite(5,HIGH);
  }
  
  
  Serial.print("Tflag");
  Serial.println(tflag);
  Serial.print("FFlag");
  Serial.println(fflag);
//  myservo.write(45);
//  delay(15);
//  myservo.write(135);
  int x=presence();
  delay(2000);
  requestfrm();
  sendthingspeak(curr,vol,powers,temper,humi,x);
//  char z=(int)x;
//  Wire.beginTransmission(0x0B);
//  Wire.write(z);
//  Wire.endTransmission();
}


