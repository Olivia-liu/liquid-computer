#include <Servo.h>

const int DELAY = 1;
const int NUM_SERVOS = 3;

Servo servos[NUM_SERVOS];
int speeds[NUM_SERVOS];
int currentDegrees[NUM_SERVOS];
boolean isForward[NUM_SERVOS];

//for reading integers
char intBuffer[12];
String intData = "";
int delimiter = (int)',';
int delimiterEnd = (int)'\n';

void setup() {
  servos[0].attach(3);
  servos[1].attach(5);
  servos[2].attach(7);
  servos[3].attach(9);

  for(int i = 0; i < NUM_SERVOS; i++){
    currentDegrees[i] = 0;
    speeds[i] = 180;
    isForward[i] = true;
  }

  Serial.begin(9600);
}

void loop() {
  //read speeds
  int ch;
  int intLength;
  int index = 0;
  //ref: https://stackoverflow.com/questions/16126219/serial-message-to-integer-on-arduino
  while(Serial.available()){
    ch = Serial.read();
    if(ch == delimiterEnd){
      break;
    }
    
    // Reinitialize intData for use next time around the loop
    intData = "";
    
    while(ch != delimiter && Serial.available()){
      intData += (char) ch;
      ch = Serial.read();
    }
    
    // Include room for the null terminator
    intLength = intData.length() + 1;
    intData.toCharArray(intBuffer, intLength);

    // Convert ASCII-encoded integer to an int
    // Speed is in degree/s
    speeds[index] = atoi(intBuffer);
    index++;
  }

  //control movement
  double unitAngles[NUM_SERVOS];
  for(int i = 0; i < NUM_SERVOS; i++){
    //degrees per DELAY
    unitAngles[i] = (double)speeds[i]/1000.0*DELAY;
  }

  double accumulatedAngles[NUM_SERVOS];
  for(int i = 0; i < NUM_SERVOS; i++){
    accumulatedAngles[i] = 0;
  }
  
  int counter = 0;
  
  while(counter < 1000){
    for(int i = 0; i < NUM_SERVOS; i++){
      accumulatedAngles[i] += unitAngles[i];
      
      currentDegrees[i] = currentDegrees[i] + (int)(accumulatedAngles[i]);
      if(currentDegrees[i] >= 180){
        isForward[i] = !isForward[i];
        currentDegrees[i] = currentDegrees[i]%180;
      }
      if(isForward[i]){
        servos[i].write(currentDegrees[i]);
      }
      else{
        servos[i].write(179-currentDegrees[i]);
      }
      accumulatedAngles[i] = accumulatedAngles[i] - (int)(accumulatedAngles[i]);
    }
    delay(DELAY);
    counter++;
  }

  
}
