#include <SPI.h>
#include<nRF24L01.h>
#include "RF24.h"
#include <Arduino.h>
#include "RF24Network.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


#include <WiFiClient.h>


 WiFiClient client;

const char *ssid="it hurts when IP";
const char *pass="no it does not";
//const char *ssid="Ronin";
//const char *pass="Aravind080";
const char *link="http://192.168.43.222:8000/post/";
String postData;


RF24 radio(0, 2); // CE, CSN
//const byte address[6] = "00001";
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t node02 = 02;      // Address of the other node in Octal format
const uint16_t node01 = 01;
RF24Network network(radio);

void setup() {
  Serial.begin(115200);

  /*radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();*/

  SPI.begin();
  radio.begin();
  network.begin(90,this_node);     //channel, current node address
  radio.setDataRate(RF24_2MBPS);


  WiFi.mode(WIFI_OFF);
  Serial.println("Beginning");
  
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED){delay(100);}

  Serial.print("Connected:");
  Serial.println(WiFi.localIP());
}
void loop() {
  network.update();
  int prev_id, id;
  float prev_LPG,prev_dust,prev_propane,LPG,dust,propane,methane,alcohol,smoke,CO,CO2,NH3,SO2,NOx;
  if (network.available()) 
  {
    
    RF24NetworkHeader header;

    
      network.read(header,&prev_id,sizeof(prev_id));        
      network.read(header,&dust,sizeof(dust));
      network.read(header,&prev_LPG,sizeof(prev_LPG));
      network.read(header,&prev_propane,sizeof(prev_propane));
      
    
    network.read(header,&id, sizeof(id));
    network.read(header,&methane, sizeof(methane));
    network.read(header,&alcohol, sizeof(alcohol));
    network.read(header,&smoke, sizeof(smoke));
    network.read(header,&CO2,sizeof(CO2));
    network.read(header,&CO,sizeof(CO));
    network.read(header,&NH3,sizeof(NH3));
    network.read(header,&SO2,sizeof(SO2));
    network.read(header,&NOx,sizeof(NOx));
    network.read(header,&LPG,sizeof(LPG));
    network.read(header,&propane,sizeof(propane));


      prev_id=1;
      id=2;
    String j="id="+String(prev_id)+" dust="+String(dust)+" LPG="+String(prev_LPG)+" Propane="+String(prev_propane)+"~";
    postData=j+"id="+String(id)+" LPG="+String(LPG)+" Propane="+String(propane)+" methane="+String(methane)+" alcohol="+String(alcohol)
                              +" smoke="+String(smoke)+" CO2="+String(CO2)+" CO="+String(CO)+" NH3="+String(NH3)+" SO2="+String(SO2)+" NOx="+String(NOx)
                              ;
    


   // delay(10000);
    
    
  }
  
  Serial.println(postData);
  delay(5000);

  HTTPClient http;
  http.begin(client,link);
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);
  String payload = http.getString();    //Get the response payload
   
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
 
  http.end();  //Close connection
}
