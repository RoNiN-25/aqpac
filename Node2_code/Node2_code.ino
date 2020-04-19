/*This is the program of node 2
 * COntains of arduino and NRF24L01 module
 * Has the sensors MQ-135, Mq-4, MQ-2
 * MQ-2 measures LPG and Propane and SO2
 * MQ-135 measures Co, CO2, NH3 and NOx
 * MQ-4 measures ch4,alcohol, smoke
*/

int id = 2;

#include <SPI.h>
#include<nRF24L01.h>
#include "RF24.h"
#include "RF24Network.h"
RF24 radio(7, 8); // CE, CSN
RF24Network network(radio);
const uint16_t this_node = 01;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t sink = 00;      // Address of the other node in Octal format
const uint16_t node02 = 02;

/************Pin definitions***********/
int mq135Pin=A0;
int mq4Pin=A1;
int mq2Pin=A2;


unsigned long last_sent;
unsigned long interval=0;     //interval to send data
float methane,alcohol,smoke,CO,CO2,NH3,SO2,NOx,LPG,Propane;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(mq135Pin,INPUT);
  pinMode(mq4Pin,INPUT);
  pinMode(mq2Pin,INPUT);

  SPI.begin();
  radio.begin();
  network.begin(90,this_node);     //channel, current node address
  radio.setDataRate(RF24_2MBPS);

}

void loop() {
  // put your main code here, to run repeatedly:
  network.update();
  String s;
  int prev_id;
  float prev_dust,prev_LPG,prev_propane;
  while(network.available())
  {
        RF24NetworkHeader header;
      network.read(header,&prev_id,sizeof(prev_id));        
      network.read(header,&prev_dust,sizeof(prev_dust));
      network.read(header,&prev_LPG,sizeof(prev_LPG));
      network.read(header,&prev_propane,sizeof(prev_propane));
      s="id="+String(id)+" dust="+String(prev_dust)+" LPG="+String(LPG)+" Propane="+String(prev_propane)+"~~";

  }
  
  methane=calcCH4();
  alcohol=calcAlcohol();
  smoke=calcSmoke();
  CO2=calcCO2();
  CO=calcCO();
  NH3=calcNH3();
  SO2=calcSO2();
  NOx=calcNOx();
  LPG=calcLPG();
  Propane=calcPropane();

  Serial.print("CH4=");
  Serial.println(methane);
  
  Serial.print("Alcohol=");
  Serial.println(alcohol);
  
  Serial.print("Smoke=");
  Serial.println(smoke);

  Serial.print("CO2=");
  Serial.println(CO2);
  
  Serial.print("CO=");
  Serial.println(CO);
  
  Serial.print("NH3=");
  Serial.println(NH3);
  
  Serial.print("SO2=");
  Serial.println(SO2);
  
  Serial.print("NOx=");
  Serial.println(NOx);
  
  Serial.print("LPG=");
  Serial.println(LPG);
  
  Serial.print("Propane=");
  Serial.println(Propane);



  RF24NetworkHeader header(sink);  
  unsigned int now = millis();
  if(now-last_sent>+interval){

  last_sent=now;
  network.write(header,&prev_id,sizeof(prev_id));        
  network.write(header,&prev_dust,sizeof(prev_dust));
  network.write(header,&prev_LPG,sizeof(prev_LPG));
  network.write(header,&prev_propane,sizeof(prev_propane));



      
  network.write(header,&id,sizeof(id));
  network.write(header,&methane,sizeof(methane));
  network.write(header,&alcohol,sizeof(alcohol));
  network.write(header,&smoke,sizeof(smoke));
  network.write(header,&CO2,sizeof(CO2));
  network.write(header,&CO,sizeof(CO));
  network.write(header,&NH3,sizeof(NH3));
  network.write(header,&SO2,sizeof(SO2));
  network.write(header,&NOx,sizeof(NOx));
  network.write(header,&LPG,sizeof(LPG));
  network.write(header,&Propane,sizeof(Propane));
  }

  //delay(5000);
  
}


float calcCO()   //MQ-135
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=10, y0=3, x1=200 ,y1=1.5;  // data points taken from the datasheet
  float Ro=76.06;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq135Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcCO2()   //MQ-135
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=10, y0=2.2, x1=200 ,y1=0.8;  // data points taken from the datasheet
  float Ro=76.06;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq135Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}





float calcNH3()  //MQ-135
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=10, y0=2.3, x1=200 ,y1=07;  // data points taken from the datasheet
  float Ro=76.06;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq135Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcNOx()     //MQ-135
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=10, y0=1.6, x1=200 ,y1=0.65;  // data points taken from the datasheet
  float Ro=76.06;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq135Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}





float calcCH4()      //Mq-4
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=1000, y0=1, x1=10000 ,y1=0.43;  // data points taken from the datasheet
  float Ro=75.09;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq4Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcAlcohol()   //MQ-4
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=4, x1=10000 ,y1=3.05;  // data points taken from the datasheet
  float Ro=75.09;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq4Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm/100;
  
}


float calcSmoke()     //MQ-4
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=3.8, x1=10000 ,y1=2.6;  // data points taken from the datasheet
  float Ro=75.09;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcSO2()      //MQ-2
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=3.12, x1=10000 ,y1=0.6;  // data points taken from the datasheet
  float Ro=18.80;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcLPG()     //MQ-2
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=1.75, x1=10000 ,y1=0.275;  // data points taken from the datasheet
  float Ro=18.80;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}



float calcPropane()   //MQ-2
{
  //ppm=x0*(y/y0)^(log(x1/x0)/log(y1/y0))
  float x0=200, y0=1.85, x1=10000 ,y1=0.295;  // data points taken from the datasheet
  float Ro=18.80;    // calculated by caliberation (in clean air)
  float Rs=analogRead(mq2Pin);   // read value from the analog output of the sensor
  float y=Rs/Ro; 

  float ppm = x0*(pow((y/y0),(log(x1/x0)/log(y1/y0))));  // calculate from the graph
  return ppm;
  
}
