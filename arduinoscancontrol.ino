/* Single stepper motor control */

#define DEBUG_OUT

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

int enabled = 2;
int dir = 3;
int step  = 6;
int laser = 5;
int endPin = 12;
int homePin = 11;

enum CommandModeType {
  MOVE,
  IDLE,
  HOME,
  FORWARD,
  BACK,
  LASER_ON,
  LASER_OFF
};


int stepdelay = 32;
unsigned long long dist = 0;
unsigned long long stepsOneRound = 3200;
unsigned long long stepsOnePoint = 32;
unsigned long long stepsPending = 0;
long long stepsMoved = 0
bool motorEnabled;
CommandModeType currentCommand = IDLE;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);

  pinMode(enabled, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);

  pinMode(laser, OUTPUT);

  pinMode(homePin,INPUT);
  pinMode(endPin,INPUT);

  digitalWrite(enabled, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {

  switch (currentCommand) {
    case LASER_ON: {
        digitalWrite(laser, HIGH);
        currentCommand = IDLE;
        break;
      }
    case MOVE: {
        stepsPending = stepsOnePoint * dist;
        digitalWrite(enabled, LOW);
        currentCommand = IDLE;
        break;
      }
    case LASER_OFF: {
        digitalWrite(laser, LOW);
        currentCommand = IDLE;
        break;
      }
    case FORWARD: {
        digitalWrite(dir, HIGH);
        currentCommand = IDLE;
        break;
      }
    case BACK: {
        digitalWrite(dir, LOW);
        currentCommand = IDLE;
        break;
      }
    case HOME: {
        stepsPending = 1000000000;
        digitalWrite(dir,LOW);
        currentCommand=IDLE;
        break;
    }
    case IDLE: {
        if (stepsPending > 0)
        {
          motorEnabled = true;
          digitalWrite(step, HIGH);
          delayMicroseconds(stepdelay);
          digitalWrite(step, LOW);
          delayMicroseconds(stepdelay);
          stepsPending--;
        } else if(motorEnabled){
          motorEnabled = false;
          digitalWrite(enabled, HIGH);
          Serial.println("STOP");
        }
      }
  }
/*
  if(digitalRead(homePin) == LOW)
  {
    stepsPending = 1000;
    digitalWrite(dir,HIGH);
    currentCommand = IDLE;
  }
  if(digitalRead(endPin == LOW))
  {
    stepsPending = 1000;
    digitalWrite(dir,LOW);
    currentCommand = IDLE;
  }*/
  if (stringComplete) {
    Serial.println(inputString);
    boolean parseOk = true;
    do {
      const char STEPS_COMMAND[] = "MOVE ";
      if (inputString.startsWith(STEPS_COMMAND)) {
        String num = inputString.substring(sizeof(STEPS_COMMAND) - 1);
        if (stepsPending != 0)
        {
          parseOk = false;
          break;
        }
        dist = num.toInt();
        currentCommand = MOVE;
        break;
      }
      if (inputString.startsWith("LASER_ON")) {
        currentCommand = LASER_ON;
        break;
      }
      if (inputString.startsWith("LASER_OFF")) {
        currentCommand = LASER_OFF;
        break;
      }
      if (inputString.startsWith("FORWARD") && !motorEnabled)
      {
        currentCommand = FORWARD;
        break;
      }
      if (inputString.startsWith("BACK") && !motorEnabled)
      {
        currentCommand = BACK;
        break;
      }
      if (inputString.startsWith("HOME"))
      {
        //TODO
        currentCommand=HOME;
        break;
      }
      parseOk = false;
    } while (false);

    if (parseOk) {
      Serial.println("OK");
    } else {
      Serial.println("ERR");
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  else {
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();

      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') {
        stringComplete = true;
      }
    }
  }
}
