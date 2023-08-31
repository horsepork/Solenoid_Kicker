#include "Arduino.h"
#include "SolenoidKicker.h"
#include "Shift_Input.h"
#include "Shift_Output.h"
#include "Debounced_DigitalRead.h"

#define digitalOutputPin 4
#define kickerSensorPin 5
Debounced_DigitalRead kickerSensor(kickerSensorPin, INPUT_PULLUP);

Debounced_DigitalRead otherKickerSensor(6, INPUT_PULLUP);


SolenoidKicker digitalOutputKicker(digitalOutputPin, kickerSensor);

Shift_Output shiftOutput(2, 3, 4, 1); // made up clock, load, and latch pins
#define SHIFT_OUT_KICK_INDEX 0
SolenoidKicker shiftOutputKicker(&shiftOutput, SHIFT_OUT_KICK_INDEX, otherKickerSensor);

void setup(){
    shiftOutput.begin();
    kickerSensor.begin();
    otherKickerSensor.begin();
}

void loop(){
    kickerSensor.update();
    otherKickerSensor.update();
    digitalOutputKicker.update(); // call in loop
    digitalOutputKicker.setKickState(HIGH); // will cause a periodic kick until sensor is disengaged
    digitalOutputKicker.setKickState(LOW); // halt kicking
}