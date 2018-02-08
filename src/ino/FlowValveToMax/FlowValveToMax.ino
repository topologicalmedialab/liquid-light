/*
      PREAMBLE
      
  Topological Media Lab
  Concordia University
  Montreal, Canada
  
  Liquid Light project
  Flow sensor interface firmware
  
  Written by Thierry Dumont in January 2018
  thierrydumont.td@gmail.com
  based on a version ot the code by Joseph Thibodeau in July 2016
  joseph.thibodeau@gmail.com
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


  WIRING:
  5v to Power
  gnd to gnd
  reading to pin2
  10k ohm between pin2 and 5v
*/


//--------------------------------------
//      DECLARING STUFF
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
volatile bool test_pin_val = 0;
int count = 0;
int test_pin = 13;

// Flow Sensor Variables
int num_sensors = 5;
volatile int flow_pin=2;
volatile int flow_pin_new_state = 0;
volatile int flow_pin_old_state= 0;
volatile int flow_pulse_count = 0;

//======================================
//      SETUP FUNCTION
//======================================
void setup()
{
  cli(); //disable interrupts
  
  // test pin setup
  pinMode(test_pin,OUTPUT);
  digitalWrite(test_pin,LOW);
  
  // setup input pins

    pinMode(flow_pin,INPUT);

  // initialize Timer1
  TCCR1A = 0;    // set entire TCCR1A register to 0
  TCCR1B = 0;    // set entire TCCR1B register to 0 
                 // (as we do not know the initial  values) 
  
  // enable Timer1 overflow interrupt:
  TIMSK1 |= (1 << TOIE1); //tells timer to trigger interupt on overflow
  
  // Preload with value 3036
  //use 64886 for 100Hz
  //use 64286 for 50 Hz
  //use 34286 for 2 Hz
  TCNT1=RESET_COUNT;
  // Set CS10 bit so timer runs at clock speed: (no prescaling)
  TCCR1B |= (1 << CS12); // Sets bit CS12 in TCCR1B meaning timer overflows +/- 1s
    
  Serial.begin(9600);
  Serial.print(0);
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
  test_pin_val = HIGH;
  digitalWrite(test_pin,test_pin_val);
  TCNT1=RESET_COUNT; // reload the timer preload
  //Serial.println("ISR");
}


//======================================
//      MAIN LOOP
//======================================
void loop()
{
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
    //Serial.println("ReadVal");
    //Serial.println(flow_pin_new_state);
    //is it different? Looking for a rising edge
    if ((flow_pin_new_state == HIGH) && (flow_pin_old_state == LOW))
    {
      flow_pulse_count += TIME_DIV;
      test_pin_val = LOW;
      digitalWrite(test_pin,test_pin_val);
    }
    flow_pin_old_state = flow_pin_new_state;


  
}
