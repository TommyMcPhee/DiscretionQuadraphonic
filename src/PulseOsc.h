#pragma once
#ifndef sinOsc_hpp
#define sinOsc_hpp

#include <stdio.h>
#include "ofMathConstants.h"

class pulseOsc {
public:
    pulseOsc();
    pulseOsc(float _duty, float _freq, float _phase, float _amp, int _sampleRate);
    void setDuty(float _duty);
    void setFreq(float _freq);
    void setAmp(float _amp);
    void setPhase(float _phase);
    void setSampleRate(int _sampleRate);
    float getSample();
    float getPhaseInc();
    float getDuty();
    float getFreq();
    float getPhase();
    float getAmp();
    int getSampleRate();

private:
    void calculatePhaseIncrement();
    void checkPhase();
    void incrementPhase();
    float duty;
    float frequency;
    float phase;
    float amplitude;
    int sampleRate;
    float phaseIncrement;
};

#endif