#include "PulseOsc.h"

pulseOsc::pulseOsc() {

}

pulseOsc::pulseOsc(float _duty, float _freq, float _phase, float _amp, int _sampleRate) {
    duty = _duty;
    frequency = _freq;
    phase = _phase;
    amplitude = _amp;
    sampleRate = _sampleRate;
    calculatePhaseIncrement();
}
float pulseOsc::getSample()
{
    float sample;
    checkPhase();
    if (phase <= duty) {
        sample = amplitude;
    }
    else {
        sample = amplitude * -1.0;
    }
    incrementPhase();
    return sample;
}

void pulseOsc::setDuty(float _duty) {
    duty = _duty;
}

void pulseOsc::setFreq(float _freq) {
    frequency = _freq;
    calculatePhaseIncrement();
}

void pulseOsc::setAmp(float _amp) {
    amplitude = _amp;
}

void pulseOsc::setPhase(float _phase) {
    phase = _phase;
}

void pulseOsc::setSampleRate(int _sampleRate) {
    sampleRate = _sampleRate;
    calculatePhaseIncrement();
}

float pulseOsc::getPhaseInc() {
    return phaseIncrement;
}

void pulseOsc::calculatePhaseIncrement() {
    phaseIncrement = frequency / (float)sampleRate;
};

void pulseOsc::checkPhase() {
    if(phase > 1.0 || phase < 0.0) { 
        phase = fmod(phase, 1.0); 
    }
}

void pulseOsc::incrementPhase() {
    phase += phaseIncrement;
}

float pulseOsc::getDuty() {
    return duty;
}

float pulseOsc::getFreq() {
    return frequency;
}

float pulseOsc::getAmp() {
    return amplitude;
}

float pulseOsc::getPhase() {
    return phase;
}

int pulseOsc::getSampleRate() {
    return sampleRate;
}
