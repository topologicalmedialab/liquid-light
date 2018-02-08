/*
      PREAMBLE

  Topological Media Lab
  Concordia University
  Montreal, Canada

  Liquid Light project
  Sink control for House simulation


  Written by Thierry Dumont in January 2018
  thierrydumont.td@gmail.com

  potentiometer to PWM power out
  PWM out to transistor
  Converts potentiometer values to a voltage out.
  Varies depending on input voltage. E.g. 12vdc in goes from 0 to 12. 24vdc goes from 0 to 24

  WIRING:
  Potentiometer
  Pin out Left ground Center to analog right to 5V

  PWM pin to transistor collector
  Regular TIP120 circuit
*/

int potPin = 0;    // select the input pin for the potentiometer
int pumpPin = 9;   // select the output pin for the transitor to the pump
int val = 0;       // variable to store the value coming from the sensor
//Range between 0 1023
int minVal = 50; //minimal value to start pump
int maxVal = 900; //max value for pump

void setup() {
  pinMode(9 , OUTPUT);
  Serial.begin (115200);
}

void loop() {
  val = analogRead(potPin);    // read the value from the sensor
  //Serial.println(val);
  int outputPWM = map(val, 0, 1023, 0, 255);//map sensor value between 0 and 255 for PWM
  
  //just to be certain there will not be nay wierd value to the PWM and to have a closed state
  if (outputPWM > 5 && outputPWM < 255) {
    analogWrite(pumpPin, outputPWM);
    //Serial.println(outputPWM);
  } else {
    analogWrite(pumpPin, 0);
  }

  
  delay(20);                  // stop the program for some time
}

