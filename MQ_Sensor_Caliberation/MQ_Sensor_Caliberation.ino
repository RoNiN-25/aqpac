//program for caloberating the sensor values

int analogPin = A0; //pin for reading input
float r0 = 10; //kilo ohm
float Rl=5;   //kilo ohm

void setup() {
  // put your setup code here, to run once:
  pinMode(analogPin,INPUT);
  Serial.begin(9600);
  Serial.println("Beginning Caliberation");

  float mqval=0;
  for(int i=0;i<=50;i++)
  {
    int adc = analogRead(analogPin);
    mqval += (float)(Rl*(1023-adc)/adc);  //The Rs and Ro form a voltage divider for input
                                                                                   //This command calculates the value of Ro in clean 
                                                                                   //air which can be used later for the calculation 
                                                                                   //of gas concentration
    delay(500);  //sampling time
  }
  mqval/=51;
  r0 = mqval/10;     //10 is Ro clean factor, derived from datasheet
  Serial.println(r0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
