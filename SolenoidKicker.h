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
            kickPin = _kickPin;
            kickerSensor = _kickerSensor;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, BooleanInputBase* _kickerSensor){
            kickerOutputType = SHIFT_REGISTER_OUTPUT;
            shiftOutput = _shiftOutput;
            shiftOutputIndex = _shiftOutputIndex;
            kickerSensor = _kickerSensor;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(uint8_t _kickPin, Debounced_DigitalRead* _reader){
            kickerOutputType = DIGITAL_WRITE_OUTPUT;
            kickPin = _kickPin;
            kickerSensor = &_reader->booleanBaseObject;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, Debounced_DigitalRead* _reader){
            kickerOutputType = SHIFT_REGISTER_OUTPUT;
            shiftOutput = _shiftOutput;
            shiftOutputIndex = _shiftOutputIndex;
            kickerSensor = &_reader->booleanBaseObject;
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(uint8_t _kickPin, Shift_Input* _shiftInput, uint8_t _shiftInputIndex){
            kickerOutputType = DIGITAL_WRITE_OUTPUT;
            kickPin = _kickPin;
            kickerSensor = &_shiftInput->booleanBaseObject[_shiftInputIndex];
            kickerStatus = NOT_KICKING;
        }

        SolenoidKicker(Shift_Output* _shiftOutput, uint8_t _shiftOutputIndex, Shift_Input* _shiftInput, uint8_t _shiftInputIndex){
            kickerOutputType = SHIFT_REGISTER_OUTPUT;
            shiftOutput = _shiftOutput;
            shiftOutputIndex = _shiftOutputIndex;
            kickerSensor = &_shiftInput->booleanBaseObject[_shiftInputIndex];
            kickerStatus = NOT_KICKING;
        }



        void update(){
            switch(kickerStatus){
                case NOT_KICKING:
                    if(kickState == HIGH && isEngaged() && (millis() - inBetweenKicksTimer > timeBetweenKicks){
                        setKickOutputState(HIGH);
                        kickTimer = millis();
                        kickerStatus = KICKING;
                    }
                    break;
                case KICKING:
                    if(!isEngaged() || kickState == LOW){
                        kickerStatus = NOT_KICKING;
                        setKickOutputState(LOW);
                        inBetweenKicksTimer = millis();
                    }
                    else if(millis() - kickTimer > kickLength){
                        setKickOutputState(LOW);
                        kickerStatus = IN_BETWEEN_KICKS;
                        inBetweenKicksTimer = millis();
                    }
                    break;
                case IN_BETWEEN_KICKS:
                    if(kickState == LOW || !isEngaged()){
                        kickerStatus = NOT_KICKING;
                        setKickOutputState(LOW); // for safety, but otherwise unnecessary
                    }
                    else if(millis() - inBetweenKicksTimer > timeBetweenKicks){
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
        uint32_t inBetweenKicksTimer;
        uint16_t kickLength = 75;
        uint32_t timeBetweenKicks = 2000;

        uint8_t kickPin;

        Shift_Output* shiftOutput = nullptr;
        uint8_t shiftOutputIndex;
        
        BooleanInputBase* kickerSensor = nullptr;

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