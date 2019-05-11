#include <DHT.h>
#include <Wire.h>
//#include <Servo.h>
DHT dht(7,DHT22);

const int sensorIn = A2;
const int sensor2In= A1;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VOLTAGE =0;//for the volatge test
double VRMS = 0;
double AmpsRMS = 0;
//int stat;
char a[31];
String sendto;

bool flag = false;

void setup(){
dht.begin();
pinMode(12,INPUT);
//pinMode(11,INPUT);
//pinMode(6,OUTPUT);
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
digitalWrite(9,LOW);
Wire.onRequest(requestEvent);
Wire.onReceive(receiveEvent);


Wire.begin(0x0B);
Serial.begin(115200);

}



void loop(){
Voltage = getVPP();
//Serial.println(Voltage);
VRMS = (Voltage*0.707)/2;
AmpsRMS = (VRMS * 1000)/mVperAmp;
VOLTAGE = getVol();
float power = AmpsRMS*230;
float h = dht.readHumidity();
float t = dht.readTemperature();
if(isnan(h)||isnan(t))
{
  h=t=0;
}
//Serial.print(AmpsRMS);
//Serial.println(" Amps");
//Serial.print(VOLTAGE);
//Serial.println("volts");
//Serial.print(power);
//Serial.println("watts");
//Serial.print("Humidity: ");
//Serial.print(h);
//Serial.println(" %\t");
//Serial.print("Temperature: ");
//Serial.print(t);
//Serial.println(" *C ");
  
  sendto = String(AmpsRMS);
  sendto += 'n';
  sendto += String(VOLTAGE);
  sendto += 'n';
  sendto += String(power);
  sendto += 'n';
  sendto += String(h);
  sendto +='n';
  sendto += String(t);
  sendto += 'n';
  sendto += '\n';
  
  int x = sendto.length();
  
  sendto.toCharArray(a,x+1);
  Serial.println(x);
  Serial.println(a);
//  delay(1000);
  Serial.println(digitalRead(12));
  if(digitalRead(12)==1)
  {
    digitalWrite(8,HIGH);
    Serial.println(1);
  }
  else
  {
    digitalWrite(8,LOW);
    Serial.println(0);
  }
  if(t<=20)
  {
    digitalWrite(9,HIGH);
  }
  else if(t>=22)
  {
    digitalWrite(9,LOW);
  }


//if(digitalRead(11)==1)
//{
//  flag=~flag;
// if(flag)
// {
//  myservo.write(45);              // tell servo to go to position in variable 'pos'
//   
//  }
// else
// {
//  myservo.write(135); 
// }
//}
}




float getVPP()
{
float result;
int readValue; //value read from the sensor
//int readValue2;
int maxValue = 0; // store max value here
int minValue = 1024; // store min value here
uint32_t start_time = millis();
while((millis()-start_time) < 1000) //sample for 1 Sec
{
readValue = analogRead(sensorIn);
// see if you have a new maxValue
if (readValue > maxValue)
{
/*record the maximum sensor value*/
maxValue = readValue;
}
if (readValue < minValue)
{
/*record the maximum sensor value*/
minValue = readValue;
}
}
// Subtract min from max
result = ((maxValue - minValue) * 5.0)/1024.0;
result/=1.414;
return result;
}



float getVol()
{
 int sensorValue = analogRead(sensor2In);
// Convert the analog reading (which goes from 0 - 1023) to a volta
float voltage = sensorValue * (280.0 / 1024.0);
// print out the value you read:
return voltage;
}




void requestEvent()
{
 Wire.write(a);
}



void receiveEvent(int bytes)
{
//  char temp;
//  while(Wire.available())
//  {
//    temp=Wire.read();
//  }
//  stat=(int)temp;
//  Serial.println(stat);
}

