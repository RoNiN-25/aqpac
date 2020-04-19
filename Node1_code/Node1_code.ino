/*Code for Node 1
 * Contains Dust sensor, MQ-2, and DHT-11
 * MQ 2 is for LPG and Propane
 * Dust sensor gives output in mg/dm3 or ppb
 * Temperature will be in degree celsius
 * RH in %
 * ID is an int and is equal to PinCode+sensor number.......Currently 1 is given for simplicity

*/

int id = 1;

#include <SPI.h>
#include<nRF24L01.h>
#include "RF24.h"
#include "RF24Network.h";
RF24 radio(7, 8); // CE, CSN
//const byte address[6] = "00001";
const uint16_t this_node = 02;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t sink = 00;      // Address of the other node in Octal format
const uint16_t node01 = 01;

unsigned long last_sent;
unsigned long interval = 0;

RF24Network network(radio);


/*******pin Defenitions*******/

int ledPin=2;
int dustPin=A0;
int mq2Pin=A1;
float dustDensity,LPG,Propane;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(dustPin,INPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(mq2Pin,INPUT);

  //radio.begin();
  //radio.openWritingPipe(address);
 // radio.setPALevel(RF24_PA_MIN);
  //radio.stopListening();
  SPI.begin();
  radio.begin();
  network.begin(90,this_node);     //channel, current node address
  radio.setDataRate(RF24_2MBPS);


}

void loop() 
{
  network.update();
  // put your main code here, to run repeatedly:
  dustDensity = dustCalculate();
  LPG = calcLPG();
  Propane = calcPropane();
  
  Serial.print("Dust=");
  Serial.println(dustDensity);
  Serial.print("LPG=");
  Serial.println(LPG);
  Serial.print("Propane=");
  Serial.println(Propane);

 RF24NetworkHeader header(node01); 
  unsigned long now = millis();
  if(now-last_sent>interval){

 network.write(header,&id,sizeof(id));
 network.write(header,&dustDensity,sizeof(dustDensity));
 network.write(header,&LPG,sizeof(LPG));
 network.write(header,&Propane,sizeof(Propane));
  }
 
  

}


float dustCalculate()
{

  float voltMeasured,calcVoltage,dust;

  digitalWrite(ledPin,LOW);   //Turn the led on
  delayMicroseconds(280);

  voltMeasured = analogRead(dustPin);  //read voltage from adc

  delayMicroseconds(40);
  digitalWrite(ledPin,HIGH);   //turn off LED
  delayMicroseconds(9680);

  calcVoltage = voltMeasured * (5.0 / 1024.0);  //Calculate adc value in Volts
  
  if(calcVoltage>=3.5)
  {
    dust = 0.65;
  }
  else
  {
    dust = abs((calcVoltage-0.5)/5.8333);   // Convert to dust density, Formula from datasheet : density=(V-0.5)/5.8333
  }  

  return dust;
}





float calcLPG()
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=1.75, x1=10000 ,y1=0.275;  // data points taken from the datasheet
  float Ro=18.80;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcPropane()
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=1.85, x1=10000 ,y1=0.295;  // data points taken from the datasheet
  float Ro=18.80;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}
