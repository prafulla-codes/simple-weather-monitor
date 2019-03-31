#include <SoftwareSerial.h>
#include <dht.h>
#include <MQ135.h>
#include <SFE_BMP180.h>
#include <math.h>
dht DHT;
MQ135 mq = MQ135(A0); // set MQ135 gas sensor output to analog input a0
SFE_BMP180 bmp180;
#define RX 9  // if you connected your ESP8266 TX on 9th pin , then set RX as 9 . change this as per your settings
#define TX 10 // if you connected your ESP8266 RX on 10th pin , then set TX as 10. change this as per your settings
String AP = "<YourWiFiName>";       // put your wifi name here
String PASS = "<YourWiFiPassword"; // put your wifi password here
String API = "<YourAPIKey>";   // put the API key from things speak , "Write API KEY"
String HOST = "api.thingspeak.com"; // keep this as it is for thingspeak
String PORT = "80"; // keep this as it is
String field = "field1"; // reduce or increase the field as per your need , i have 3 fields so i created 3
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
String field5 = "field5";
int countTrueCommand; // keep as it is
int countTimeCommand; // keep as it is
boolean found = false; // keep as it is
float Po = 1013.0;
float valAirQuality = 1;   //stores value of airquality initialized 1
float valTemperature = 1; // stores value of temperature initialized 1
float valHumidity = 1;  // stores value of humidity initialized 1
double valPressure = 1;  // stores air pressure initialized 1
double valAltitude = 1; // stores altitude
SoftwareSerial esp8266(RX,TX);  // this is used to create esp8266 object and convert digital pins as communication pins

  
void setup() {
  Serial.begin(9600); // start serial monitor , baud rate 9600
  Serial.println("Weather Monitor System v1.0 Initialized");
  esp8266.begin(115200); //  start Esp8266 object
  sendCommand("AT",5,"OK"); // keep as it is
  sendCommand("AT+CWMODE=1",5,"OK");  // keep as it is
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK"); // keep as it is
  bool success = bmp180.begin();
  if(success==1)
  {
    Serial.println("Baromatric Sensor Active!");
  }
}
void loop() {
  int rzero = (int) mq.getRZero();
 Serial.print("Rzero is :");
 Serial.println(rzero);
 valAirQuality = getAirQuality(); // create a function to get each sensor value , and set the value as the result of the function
 valTemperature = getTemp();  // same as above
 valPressure = pressureVal();
 valAltitude = getAltitude();

 //  Send a GET Request and update the field value's 
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+valAirQuality+"&"+field2 +"="+valTemperature +"&" + field3 + "=" + valHumidity + "&" +field4 + "=" + valPressure + "&" + field5 + "=" + valAltitude; // the main thing to send the data , edit this as per your fields
sendCommand("AT+CIPMUX=1",5,"OK"); // keep as it is
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK"); // keep as it is
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");  // keep as it is
 esp8266.println(getData);delay(1500);countTrueCommand++; //keep as it is
 sendCommand("AT+CIPCLOSE=0",5,"OK"); // keep as it is
}
int getAirQuality(){ //function to get air quality , change this as per your fields
  return analogRead(mq.getPPM()); // change the pin as per your pins , in my case i got Air Quality at pin A0
}
float getTemp() //change as per your case
{
  DHT.read11(A1); // dht11 library was in my case , change this as per your case;
  return DHT.temperature;
}
float getHumidity() //change as per your case
{
  DHT.read11(A1);
  return DHT.humidity;
}
double getAltitude()
{
    boolean status = bmp180.startTemperature();
  double T,P;
  double alt;
  if (status != 0) {
    delay(1000);
    status = bmp180.getTemperature(T);
    if (status != 0) {
      status = bmp180.startPressure(3);
      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);
        if(status!=0)
        {
           alt = bmp180.altitude(P,Po);
           return alt;
        }
       else
       {
        return 0;
       }
            }
            else
            {
              return 0;
            }
    }
    else {
      return 0;
    }
    }
    else {
      return 0;
    }
}
double pressureVal()
{
  boolean status = bmp180.startTemperature();
  double T,P;
  if (status != 0) {
    delay(1000);
    status = bmp180.getTemperature(T);
    if (status != 0) {
      status = bmp180.startPressure(3);
      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);
        if(status!=0)
        {
           return (P);
        }
       else
       {
        return 0;
       }
            }
            else
            {
              return 0;
            }
    }
    else {
      return 0;
    }
    }
    else {
      return 0;
    }
}
void sendCommand(String command, int maxTime, char readReplay[]) {  // keep as it is
  Serial.print(countTrueCommand); // keep as it is
  Serial.print(". at command => "); // keep as it is
  Serial.print(command); // keep as it is
  Serial.print(" "); // keep as it is
  while(countTimeCommand < (maxTime*1)) // keep as it is
  {  
    esp8266.println(command);//at+cipsend   // keep as it is
    if(esp8266.find(readReplay))//ok   // keep as it is
    {
      found = true;  // keep as it is
      break;  // keep as it is
    }
     countTimeCommand++;  // keep as it is
  }
  if(found == true)
  {
    Serial.println(" - Successful");  // keep as it is
    countTrueCommand++;   // keep as it is
    countTimeCommand = 0;   // keep as it is
  }
  
  if(found == false)
  {
    Serial.println("Fail");  // keep as it is
    countTrueCommand = 0;   // keep as it is
    countTimeCommand = 0;   // keep as it is
  }
  found = false;
 }
