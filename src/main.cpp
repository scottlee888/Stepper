#include <Arduino.h>
#include <public.h>
#include <TimerOne.h>

const int enPin=8;
const int stepPin_0 = 2; //X.STEP
const int dirXPin = 5; // X.DIR
const int step1Pin = 3; //Y.STEP
const int dirYPin = 6; // Y.DIR
const int step2Pin = 4; //Z.STEP
const int dirZPin = 7; // Z.DIR
int stepPin=step1Pin;
int dirPin=dirYPin;

int pulseWidthMicros = 10; 	// microseconds
int millisBtwnSteps = pulseWidthMicros;

int StepBuffer [3];
int StepPin[3] = {2,3,4};
int DirPin[3] = {5,6,7};

int Delay_Pulse_Width[3];
int Delay_Between_Step[3];

int Counter_Delay_Pulse_Width[3];
int Counter_Delay_Between_Step[3];

int StateMachine_Axis[3] = {0,0,0};

const int PERIOD_PER_DELAY_MICROS  = 5;

void Load_Delay()
{
  for(int iterator = 0; iterator < 3; iterator++)
  {
    Counter_Delay_Pulse_Width[iterator] = Delay_Pulse_Width[iterator];
    Counter_Delay_Between_Step[iterator] = Delay_Between_Step[iterator];
  }
}

void MoveGantry(int Step, int Direction)
{
  int PinState[3];
  
  if(StepBuffer[AXIS_X] > 15)
  {
    //Serial.println(StepBuffer[AXIS_X]);
    Serial.println(StateMachine_Axis[AXIS_X]);
    
    return;
  }

  if(Direction == LEFT)
  {
    PinState[0] = LOW;
    PinState[1] = LOW;
  }
  else if(Direction == RIGHT)
  {
    PinState[0] = HIGH;
    PinState[1] = HIGH;
  }
  else if(Direction == FRONT)
  {
    PinState[0] = HIGH;
    PinState[1] = LOW;
  }
  else if(Direction == BACK)
  {
    PinState[0] = LOW;
    PinState[1] = HIGH;
  }

  digitalWrite(DirPin[STEPPER0], PinState[0]);
  digitalWrite(DirPin[STEPPER1], PinState[1]);

  StepBuffer[AXIS_X] = Step;

  Delay_Pulse_Width[AXIS_X] = pulseWidthMicros;
  Delay_Between_Step[AXIS_X] = pulseWidthMicros;
  StateMachine_Axis[AXIS_X] = STATE_AXIS_TRIGGER_HIGH;
  Load_Delay();
}



void timerIsr()
{
  for(int AxisIterator = 0; AxisIterator < 3; AxisIterator++)
  {
    if(StepBuffer[AxisIterator] > 1)
    {
      switch(StateMachine_Axis[AxisIterator])
      {
        case STATE_AXIS_IDLE:
        Serial.println(F("State_0"));
        //MoveX(1000, RIGHT);
        Load_Delay();
        break;

        case STATE_AXIS_TRIGGER_HIGH:
        digitalWrite(StepPin[STEPPER0], HIGH);
        digitalWrite(StepPin[STEPPER1], HIGH);
        StateMachine_Axis[AxisIterator]++;
        break;

        case STATE_AXIS_PULSE_WIDTH:
        //Serial.print(F("State_2 - "));
        //Serial.println(Counter_Delay_Pulse_Width[Axis_Selection]);
        if(Counter_Delay_Pulse_Width[AxisIterator] >= PERIOD_PER_DELAY_MICROS)
        {
          Counter_Delay_Pulse_Width[AxisIterator] -= PERIOD_PER_DELAY_MICROS;
        }
        else
        {
          StateMachine_Axis[AxisIterator]++;
        }
        break;

        case STATE_AXIS_TRIGGER_LOW:
        digitalWrite(StepPin[STEPPER0], LOW);
        digitalWrite(StepPin[STEPPER1], LOW);
        StateMachine_Axis[AxisIterator]++;
        break;

        case STATE_AXIS_BETWEEN_STEP:
        //Serial.print(F("State_4 - "));
        //Serial.println(StepBuffer[AxisIterator]);
        
        if(Counter_Delay_Between_Step[AxisIterator] >= PERIOD_PER_DELAY_MICROS)
        {
          Counter_Delay_Between_Step[AxisIterator] -= PERIOD_PER_DELAY_MICROS;
        }
        else
        {
          if(StepBuffer[AxisIterator] > 0)
          {
            StepBuffer[AxisIterator]--;
            Load_Delay();
            StateMachine_Axis[AxisIterator] = STATE_AXIS_TRIGGER_HIGH;
          }
          else
          {
            //StateMachine_Axis[AxisIterator] = STATE_AXIS_IDLE;
          }
          
        }



        break;

      }
      
    }
  }
}


void setup() {
 	Serial.begin(9600);
 	pinMode(enPin, OUTPUT);
 	digitalWrite(enPin, LOW);

  for(int iterator = 0; iterator < 3; iterator++)
  {
    pinMode(StepPin[iterator], OUTPUT);
    pinMode(DirPin[iterator], OUTPUT);
  }

 	Serial.println(F("CNC Shield Initialized"));

  // Initialize Timer1
  Timer1.initialize(PERIOD_PER_DELAY_MICROS); // Set timer for 1000 microseconds (1 millisecond)
  Timer1.attachInterrupt(timerIsr); // Attach the service routine

  MoveGantry(2000, LEFT);
}


void loop() 
{
 
    

}

