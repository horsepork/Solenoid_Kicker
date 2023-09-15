#ifndef SOLENOIDKICKER_H
#define SOLENOIDKICKER_H

#include "Arduino.h"
#include "BooleanInputBase.h"
#include "Shift_Input.h"
#include "Shift_Output.h"
#include "Debounced_DigitalRead.h"

// forceKick() not implemented, not sure if needed

class SolenoidKicker{
    public:
        SolenoidKicker(uint8_t _kickPin, BooleanInputBase* _kickerSensor){
            kickerOutputType = DIGITAL_WRITE_OUTPUT;
            kickerSensor = _kickerSensor;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, BooleanInputBase* _kickerSensor){
            shiftOutput = _shiftOutput;
            shiftOutputIndex = _shiftOutputIndex;
            kickerOutputType = SHIFT_REGISTER_OUTPUT;
            kickerSensor = _kickerSensor;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(uint8_t _kickPin, Debounced_DigitalRead* reader){
            SolenoidKicker(_kickPin, &reader->booleanBaseObject);
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, Debounced_DigitalRead* _reader){
            SolenoidKicker(_shiftOutput, _shiftOutputIndex, &_reader->booleanBaseObject);
        }

        SolenoidKicker(uint8_t _kickPin, Shift_Input* _shiftInput, uint8_t _shiftInputIndex){
            SolenoidKicker(_kickPin, &_shiftInput->booleanBaseObject[_shiftInputIndex]);
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, Shift_Input* _shiftInput, uint8_t _shiftInputIndex){
            SolenoidKicker(_shiftOutput, _shiftOutputIndex, &_shiftInput->booleanBaseObject[_shiftInputIndex]);
        }

        void update(){
            switch(kickerStatus){
                case NOT_KICKING:
                    if(kickState == HIGH && isEngaged()){
                        Serial.println("kicking now");
                        setKickOutputState(HIGH);
                        kickTimer = millis();
                        kickerStatus = KICKING;
                    }
                    break;
                case KICKING:
                    if(!isEngaged() || kickState == LOW){
                        Serial.println("not engaged or kickstate set to low");
                        kickerStatus = NOT_KICKING;
                        setKickOutputState(LOW);
                    }
                    else if(millis() - kickTimer > kickLength){
                        kickerStatus = IN_BETWEEN_KICKS;
                        kickTimer = millis();
                        Serial.println("in between kicks");
                    }
                    break;
                case IN_BETWEEN_KICKS:
                    if(kickState == LOW){
                        Serial.println("not kicking");
                        kickerStatus = NOT_KICKING;
                        setKickOutputState(LOW); // for safety, but otherwise unnecessary
                    }
                    else if(millis() - kickTimer > timeBetweenKicks){
                        Serial.println("Kicking again");
                        kickerStatus = KICKING;
                        setKickOutputState(HIGH);
                        kickTimer = millis();
                    }
                    break;
            }
        }

        void setKickState(bool newKickState){
            kickState = newKickState;
        }

        void kick(){
            setKickState(true);
        }

        void unkick(){
            setKickState(false);
        }

    
        void invertOutput(bool _outputInverted){ // if we end up running the output through a relay, say
            outputInverted = _outputInverted;
        }

        void setKickLength(uint16_t newKickLength){
            kickLength = newKickLength;
        }

        void setTimeBetweenKicks(uint32_t _timeBetweenKicks){
            timeBetweenKicks = _timeBetweenKicks;
        }

        bool isEngaged(){
            return kickerSensor->read();
        }

        void forceKick(){
            
        }
    
    private:
        bool kickState = LOW;
        bool outputInverted = false;

        uint32_t kickTimer;
        uint16_t kickLength = 75;
        uint32_t timeBetweenKicks = 2000;

        uint8_t kickPin;

        Shift_Output* shiftOutput = nullptr;
        uint8_t shiftOutputIndex;
        
        BooleanInputBase* kickerSensor;

        enum {
            DIGITAL_WRITE_OUTPUT,
            SHIFT_REGISTER_OUTPUT
        } kickerOutputType;

        enum {
            NOT_KICKING,
            KICKING,
            IN_BETWEEN_KICKS
        } kickerStatus;

        void setKickOutputState(bool newState){
            if(outputInverted){
                newState = !newState;
            }
            switch(kickerOutputType){
                case DIGITAL_WRITE_OUTPUT:
                    digitalWrite(kickPin, newState);
                    break;
                case SHIFT_REGISTER_OUTPUT:
                    shiftOutput->write(shiftOutputIndex, newState);
                    break;
            }
        }


};

#endif