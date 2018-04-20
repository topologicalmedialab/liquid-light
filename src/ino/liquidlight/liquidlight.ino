/***********************************************************************************
             Liquid Light || Serial Control From Max
             Deep Performance Dwelling, Team MTL, Solar Decathlon
             Topological Media Lab

    Author Nima Navab, Thierry Dumont
    Last Edit: March 29, 2018

    The following sketch will allow you to control 6 solenoids, a light and a motor.
    intensity in max/msp.
    The sketch also includes a flow valve sensor reading
    based on timer interrupt code snippets from:
  https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
  Using this flow valve
  https://abra-electronics.com/sensors/sensors-liquid-flow/sens-flow-5-micro-hydro-generator-w-5-volt-dc-output-voltage.html
  This will read values from 1 flow sensor and pass it to MAX MSP via Serial communication
  The sensors output a pulse train according to a spinning water wheel.

  The firmware updates the pulse counts continuously, and every
  so often a timer interrupt causes the firmware to output and
  to reset the pulse counts. These counts are multiplied by the
  frequency of the interrupt (in Hz), so they give a value of
  pulses per second.


 ***********************************************************************************/
#define MESSAGE_BYTES  2  // the total bytes in a message Id and value

int lightPin = 11;
int motorPin = 3;

// Solenoids
int sol1 = 9;
int sol2 = 8;
int sol3 = 7;
int sol4 = 6;
int sol5 = 5;
int sol6 = 4;

int type;
int value;


int messageReceived = 0;

//--------------------------------------
//      FLOW Valve
//--------------------------------------

// Timer variables
int ONE_SECOND = 62500;
int TIME_DIV = 8;
//^^^ THIS DOESNT WORK YET BUT TIME_DIV SHOULD MATCH the Hz of RESET_COUNT!
//int RESET_COUNT = 65536 - (ONE_SECOND / TIME_DIV);
//int RESET_COUNT = 49910; //4Hz
int RESET_COUNT = 57722; //8Hz (approximately)

// Testing variables
volatile bool isr_flag = false;

int count = 0;


// Flow Sensor Variables
volatile int flow_pin = 2;
volatile int flow_pin_new_state = 0;
volatile int flow_pin_old_state = 0;
volatile int flow_pulse_count = 0;




void setup()
{
  pinMode(lightPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(sol1, OUTPUT);
  pinMode(sol2, OUTPUT);
  pinMode(sol3, OUTPUT);
  pinMode(sol4, OUTPUT);
  pinMode(sol5, OUTPUT);
  pinMode(sol6, OUTPUT);


  //FLOW VALVE TIMER
  cli(); //disable interrupts

  // setup input pins
  pinMode(flow_pin, INPUT);

  // initialize Timer1
  TCCR1A = 0;    // set entire TCCR1A register to 0
  TCCR1B = 0;    // set entire TCCR1B register to 0
  // (as we do not know the initial  values)
  // enable Timer1 overflow interrupt:
  TIMSK1 |= (1 << TOIE1); //tells timer to trigger interupt on overflow
  TCNT1 = RESET_COUNT;
  // Set CS10 bit so timer runs at clock speed: (no prescaling)
  TCCR1B |= (1 << CS12); // Sets bit CS12 in TCCR1B meaning timer overflows +/- 1s


  Serial.begin(115200);
  while (! Serial);
  sei();


}



//======================================
//      TIMER INTERRUPT
//======================================
//This is the timer overflow interrupt
//This is where we will determine the input pulse states
//and keep a count of the number of pulses

ISR(TIMER1_OVF_vect)
{
  isr_flag = true;
  TCNT1 = RESET_COUNT; // reload the timer preload
  //Serial.println("ISR");
}


void loop() {
  //------this happens when the timer overflows

  if (isr_flag == true)
  {
    Serial.println(255); //start byte
    Serial.println(flow_pulse_count);//in the last timer loop
    flow_pulse_count = 0;
    isr_flag = false;

  }

  //-------this happens all the time as fast as possible
  // poll each sensor input

  // read the input value

  flow_pin_new_state = digitalRead(flow_pin);
  //is it different? Looking for a rising edge
  if ((flow_pin_new_state == HIGH) && (flow_pin_old_state == LOW))
  {
    flow_pulse_count += TIME_DIV;
  }
  flow_pin_old_state = flow_pin_new_state;



  if (Serial.available() >= MESSAGE_BYTES) {
    type = Serial.read();
    value = Serial.read();
    Serial.flush();    // Clear Serial Incoming Buffer
    messageReceived = 1;

  }//end if serial available


  if (messageReceived)
  {

    switch (type) {
      case 11:
        // LIGHT
        if (value >= 0 && value <= 255) {
          //if (value == 42) value == 0;
          analogWrite(lightPin, value);
        }

        break;
      case 22:
        // MOTOR
        if (value >= 1 && value <= 255) {
          //if (value == 1) value = 0;
          analogWrite(motorPin, value);
        }
        break;
      case 33:
        if (value == 1) {
          digitalWrite(sol1, HIGH);
        } else {
          digitalWrite(sol1, LOW);
        };
        break;
      case 44:
        if (value == 1) {
          digitalWrite(sol2, HIGH);
        } else {
          digitalWrite(sol2, LOW);
        }
        break;
      case 55:
        if (value == 1) {
          digitalWrite(sol3, HIGH);
        } else {
          digitalWrite(sol3, LOW);
        }
        break;
      case 66:
        if (value == 1) {
          digitalWrite(sol4, HIGH);
        } else {
          digitalWrite(sol4, LOW);
        }
        break;
      case 77:
        if (value == 1) {
          digitalWrite(sol5, HIGH);
        } else {
          digitalWrite(sol5, LOW);
        }
        break;
      case 88:
        if (value == 1) {
          digitalWrite(sol6, HIGH);
        } else {
          digitalWrite(sol6, LOW);
        }
        break;
    }//End switch
  }//if message received

  //delay(2);
}
