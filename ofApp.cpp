#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	setupWav();
	oscillatorA = pulseOsc(0.5, 800, 0.0, 0.5, sampleRate);
	oscillatorB = pulseOsc(0.5, 800, 0.0, 0.5, sampleRate);
	oscillatorC = pulseOsc(0.5, 800, 0.0, 0.5, sampleRate);
	nyquistOscillator = pulseOsc(0.5, nyquist, 0.0, 0.0, sampleRate);
	shader.load("discretionFixedShader");
	buffer0.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	buffer0.clear();
	buffer1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	buffer1.clear();
	buffer0.begin();
	ofClear(0, 0, 0, 255);
	buffer0.end();
	buffer1.begin();
	ofClear(0, 0, 0, 255);
	buffer1.end();
	x = 0.0;
	y = 0.0;
	for (int a = 0; a < controls.size(); a++) {
		controls[a] = 0.0;
		updateParameter(a);
	}
	audioSetup();
}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	buffer0.begin();
	shader.begin();
	setUniforms();
	buffer1.draw(x, y);
	shader.end();
	buffer0.end();
	buffer1.begin();
	buffer0.draw(x, y);
	buffer1.end();
	buffer0.draw(x, y);
}

//--------------------------------------------------------------
void ofApp::exit() {
	for (int a = 0; a < wavFiles.size(); a++) {
		wavFiles[a].close();
	}
	ofSoundStreamClose();
}


void ofApp::ofSoundStreamSetup(ofSoundStreamSettings& settings) {

}

void ofApp::audioOut(ofSoundBuffer& buffer) {
	float average = 0.0;
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		getSample();
		for (int b = 0; b < 2; b++) {
			stereoSample = { 0.0, 0.0 };
			for (int c = 0; c < 6; c++) {
				stereoSample[b] += sample[2 * c + b] / 6.0;
				recordStereo(b);
			}
			buffer[a * 2 + b] = stereoSample[b];
		}
		/*
		for (int b = 0; b < 8; b++) {
			buffer[a * 8 + b] = sample[b];
		}
		*/
		for (int b = 0; b < channels; b++) {
			average += sample[b];
			recordSample(b);
		}
	}
	average /= ((float)bufferSize * channels);
}

void ofApp::setupWav() {
		wavFile.open("discretionFixedMediaMultichannel.wav", ios::binary);
		wavFile << "RIFF";
		wavFile << "----";
		wavFile << "WAVE";
		wavFile << "fmt ";
		writeToFile(wavFile, byteDepth * 8, 4);
		writeToFile(wavFile, 1, 2);
		writeToFile(wavFile, channels, 2);
		writeToFile(wavFile, sampleRate, 4);
		writeToFile(wavFile, sampleRate * byteDepth, 4);
		writeToFile(wavFile, byteDepth, 2);
		writeToFile(wavFile, byteDepth * 8, 2);
		wavFile << "data";
		wavFile << "----";
		preAudioP = wavFile.tellp();
		maxSampleInt = pow(2, byteDepth * 8 - 1) - 1;
		stereoWavFile.open("discretionFixedMediaStereo.wav", ios::binary);
		stereoWavFile << "RIFF";
		stereoWavFile << "----";
		stereoWavFile << "WAVE";
		stereoWavFile << "fmt ";
		writeToFile(stereoWavFile, byteDepth * 8, 4);
		writeToFile(stereoWavFile, 1, 2);
		writeToFile(stereoWavFile, 2, 2);
		writeToFile(stereoWavFile, sampleRate, 4);
		writeToFile(stereoWavFile, sampleRate * byteDepth, 4);
		writeToFile(stereoWavFile, byteDepth, 2);
		writeToFile(stereoWavFile, byteDepth * 8, 2);
		stereoWavFile << "data";
		stereoWavFile << "----";
		preAudioP = stereoWavFile.tellp();
	for (int a = 0; a < wavFiles.size(); a++) {
		wavFiles[a].open("discretionFixedMediaChannel" + to_string(a + 1) + ".wav", ios::binary);
		wavFiles[a] << "RIFF";
		wavFiles[a] << "----";
		wavFiles[a] << "WAVE";
		wavFiles[a] << "fmt ";
		writeToFile(wavFiles[a], byteDepth * 8, 4);
		writeToFile(wavFiles[a], 1, 2);
		writeToFile(wavFiles[a], 1, 2);
		writeToFile(wavFiles[a], sampleRate, 4);
		writeToFile(wavFiles[a], sampleRate * byteDepth, 4);
		writeToFile(wavFiles[a], byteDepth, 2);
		writeToFile(wavFiles[a], byteDepth * 8, 2);
		wavFiles[a] << "data";
		wavFiles[a] << "----";
		preAudioP = wavFiles[a].tellp();
		maxSampleInt = pow(2, byteDepth * 8 - 1) - 1;
	}
}

void ofApp::writeToFile(ofstream& file, int value, int size) {
	file.write(reinterpret_cast<const char*> (&value), size);
}

void ofApp::recordSample(int channel) {
	sampleInt = static_cast<int>(sample[channel] * maxSampleInt);
	wavFile.write(reinterpret_cast<char*> (&sampleInt), byteDepth);
	wavFiles[channel].write(reinterpret_cast<char*> (&sampleInt), byteDepth);
}

void ofApp::recordStereo(int channel) {
	sampleInt = static_cast<int>(stereoSample[channel] * maxSampleInt);
	stereoWavFile.write(reinterpret_cast<char*> (&sampleInt), byteDepth);
}

void ofApp::audioSetup() {
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numOutputChannels = 2;
	settings.setApi(ofSoundDevice::Api::MS_DS);
	stream.setup(settings);
	nyquist = (float)sampleRate * 0.5;
	startPan = sqrt(0.5);
	minimumfloat = std::numeric_limits<float>::min();
	dutyATotal = minimumfloat;
	dutyBTotal = minimumfloat;
	dutyCTotal = minimumfloat;
	frequencyATotal = minimumfloat;
	frequencyBTotal = minimumfloat;
	frequencyCTotal = minimumfloat;
	sampleATotal = minimumfloat;
	sampleBTotal = minimumfloat;
	sampleCTotal = minimumfloat;
	samplesElapsed = 0.0;
	sample = { 0.0, 0.0 };
	length = 262.5;
	phasor1 = 0.0;
	phasor2 = 0.0;
	phasor3 = 0.0;
	phasor5 = 0.0;
	phasor7 = 0.0;
	phasor11 = 0.0;
	phasor13 = 0.0;
	phasor17 = 0.0;
	phasor19 = 0.0;
	phasor23 = 0.0;
	increment1 = getIncrement(1.0);
	increment2 = getIncrement(2.0);
	increment3 = getIncrement(3.0);
	increment5 = getIncrement(5.0);
	increment7 = getIncrement(7.0);
	increment11 = getIncrement(11.0);
	increment13 = getIncrement(13.0);
	increment17 = getIncrement(17.0);
	increment19 = getIncrement(19.0);
	increment23 = getIncrement(23.0);
}

float ofApp::getIncrement(float cycles) {
	return cycles / (length * (float)sampleRate);
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	buffer0.allocate(width, height);
	buffer1.allocate(width, height);
	window.set(width, height);
	ofClear(0, 0, 0, 255);
}

void ofApp::updateParameter(int control) {
	if (control >= 1 && control <= 6 || control >= 9 && control <= 14 || control >= 17 && control <= 22 || control >= 25 && control <= 30) {
		if (control >= 4 && control <= 6) {
			parameters[control] = nyquist * scaleControl(control);
		}
		if (control >= 9 && control <= 14 || control >= 17 && control <= 22 || control >= 25 && control <= 30) {
			parameters[control] = scaleControl(control);
		}
	}
	else {
		parameters[control] = unipolar(scaleControl(control));
	}
}

float ofApp::scaleControl(int control) {
	return pow(controls[control], 3.0);
}

float ofApp::unipolar(float input) {
	return input * 0.5 + 0.5;
}

void ofApp::getSample() {
	samplesElapsed++;
	if (samplesElapsed > length * (float)sampleRate) {
		exit();
	}
	phasor1 = incrementPhasor(phasor1, increment1);
	phasor2 = incrementPhasor(phasor2, increment2);
	phasor3 = incrementPhasor(phasor3, increment3);
	phasor5 = incrementPhasor(phasor5, increment5);
	phasor7 = incrementPhasor(phasor7, increment7);
	phasor11 = incrementPhasor(phasor11, increment11);
	phasor13 = incrementPhasor(phasor13, increment13);
	phasor17 = incrementPhasor(phasor17, increment17);
	phasor19 = incrementPhasor(phasor19, increment19);
	phasor23 = incrementPhasor(phasor23, increment23);
	feedback = pow(phasor1, 0.5);
	controls[1] = phasor19;
	controls[2] = phasor23;
	controls[3] = 1.0 - phasor23;
	controls[4] = 1.0 - phasor19;
	controls[5] = 1.0 - phasor17;
	controls[6] = phasor17;
	controls[7] = 1.0 - phasor1;
	controls[8] = 1.0 - phasor2;
	controls[0] = 1.0 - phasor3;
	controls[9] = phasor1 * phasor11;
	controls[10] = phasor1 * (1.0 - phasor11);
	controls[11] = phasor1 * phasor7;
	controls[12] = phasor1 * (1.0 - phasor7);
	controls[13] = phasor1 * phasor5;
	controls[14] = phasor1 * (1.0 - phasor5);
	controls[15] = phasor1 * phasor13;
	controls[16] = phasor1 * (1.0 - phasor13);
	controls[17] = phasor2 * phasor11;
	controls[18] = phasor2 * (1.0 - phasor11);
	controls[19] = phasor2 * phasor7;
	controls[20] = phasor2 * (1.0 - phasor7);
	controls[21] = phasor2 * phasor5;
	controls[22] = phasor2 * (1.0 - phasor5);
	controls[23] = phasor2 * phasor13;
	controls[24] = phasor2 * (1.0 - phasor13);
	controls[25] = phasor3 * phasor11;
	controls[26] = phasor3 * (1.0 - phasor11);
	controls[27] = phasor3 * phasor7;
	controls[28] = phasor3 * (1.0 - phasor7);
	controls[29] = phasor3 * phasor5;
	controls[30] = phasor3 * (1.0 - phasor5);
	controls[31] = phasor3 * phasor13;
	controls[32] = phasor3 * (1.0 - phasor13);
	for (int b = 0; b < 33; b++) {
		updateParameter(b);
	}
	dutyA = getDuty(1, 11, 13, sampleB, sampleC);
	dutyB = getDuty(2, 9, 14, sampleA, sampleC);
	dutyC = getDuty(3, 10, 12, sampleA, sampleC);
	frequencyA = getFrequency(4, 19, 21, sampleB, sampleC);
	frequencyB = getFrequency(5, 17, 22, sampleA, sampleC);
	frequencyC = getFrequency(6, 18, 20, sampleA, sampleB);
	amplitudeA = getAmplitude(7, 27, 29, sampleB, sampleC);
	amplitudeB = getAmplitude(8, 25, 30, sampleA, sampleC);
	amplitudeC = getAmplitude(0, 26, 28, sampleA, sampleC);
	oscillatorA.setDuty(dutyA);
	oscillatorA.setFreq(frequencyA);
	oscillatorA.setAmp(amplitudeA);
	oscillatorB.setDuty(dutyB);
	oscillatorB.setFreq(frequencyB);
	oscillatorB.setAmp(amplitudeB);
	oscillatorC.setDuty(dutyC);
	oscillatorC.setFreq(frequencyC);
	oscillatorC.setAmp(amplitudeC);
	dutyATotal += dutyA / samplesElapsed;
	dutyBTotal += dutyB / samplesElapsed;
	dutyCTotal += dutyC / samplesElapsed;
	frequencyATotal += pow(frequencyA / nyquist, 0.5) / samplesElapsed;
	frequencyBTotal += pow(frequencyB / nyquist, 0.5) / samplesElapsed;
	frequencyCTotal += pow(frequencyC / nyquist, 0.5) / samplesElapsed;
	nyquistOscillator.setAmp(phasor1);
	nyquistSample = nyquistOscillator.getSample();
	sampleA = mix2(oscillatorA.getSample(), nyquistSample);
	sampleB = mix2(oscillatorB.getSample(), nyquistSample);
	sampleC = mix2(oscillatorC.getSample(), nyquistSample);
	sampleATotal += sampleA / samplesElapsed;
	sampleBTotal += sampleB / samplesElapsed;
	sampleCTotal += sampleC / samplesElapsed;
	pannedA = spatialize(15, 16, sampleB, sampleC, sampleA);
	pannedB = spatialize(23, 24, sampleA, sampleC, sampleB);
	pannedC = spatialize(31, 32, sampleB, sampleC, sampleA);
	for (int a = 0; a < channels; a++) {
		lastSample[a] = sample[a];
		float channelSample;
		int oscillator = a % 3;
		int panIndex = (a - oscillator) % 4;
		switch (oscillator) {
		case 0:
			channelSample = pannedA[panIndex];
			break;
		case 1:
			channelSample = pannedB[panIndex];
			break;
		case 2:
			channelSample = pannedC[panIndex];
			break;
		}
		sample[a] = (lastSample[a] * feedback) + channelSample * (1.0 - feedback);
	}
}

array<float, 4>& ofApp::spatialize(int controlA, int controlB, float sampleA, float sampleB, float inputSample) {
	float unipolarA = unipolar(sampleA);
	float inverseA = 1.0 - unipolarA;
	float unipolarB = unipolar(sampleB);
	float inverseB = 1.0 - unipolarB;
	array<float, 4> panned;
	panned[0] = pow(unipolarA * unipolarB, 0.25);
	panned[1] = pow(inverseA * unipolarB, 0.25);
	panned[2] = pow(unipolarA * inverseB, 0.25);
	panned[3] = pow(inverseA * inverseB, 0.25);
	array<float, 4>& panReference = panned;
	return panReference;
}

float ofApp::incrementPhasor(float phasor, float increment) {
	phasor += increment;
	phasor = fmod(phasor, 1.0);
	return phasor;
}

float ofApp::getDuty(int control, int controlA, int controlB, float sampleA, float sampleB) {
	float centerWidth = parameters[control];
	float maximumWidthIndex = abs(0.5 - centerWidth) / 2.0;
	return getArgument(centerWidth, maximumWidthIndex, controlA, controlB, sampleA, sampleB);
}

float ofApp::getFrequency(int control, int controlA, int controlB, float sampleA, float sampleB) {
	float centerFrequency = parameters[control];
	float maximumFrequencyIndex = (nyquist - abs(centerFrequency)) / 2.0;
	return centerFrequency + (parameters[controlA] * sampleA * maximumFrequencyIndex) + (parameters[controlB] * sampleB * maximumFrequencyIndex);
}

float ofApp::getAmplitude(int control, int controlA, int controlB, float sampleA, float sampleB) {
	float centerAmplitude = parameters[control];
	float maximumAmplitudeIndex = abs(0.5 - centerAmplitude) / 2.0;
	float maxFloat = std::numeric_limits<float>::max();
	return ((phasor1 * maxFloat) + getArgument(centerAmplitude, maximumAmplitudeIndex, controlA, controlB, sampleA, sampleB)) / (phasor1 * maxFloat);
}

float ofApp::getArgument(float center, float maximumIndex, int controlA, int controlB, float sampleA, float sampleB) {
	return center + (parameters[controlA] * sampleA * maximumIndex) + (parameters[controlB] * sampleB * maximumIndex);
}

inline float ofApp::mix2(float a, float b) {
	return a * (1.0 - abs(b)) * ofSign(b) + b;
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	shader.setUniform1f("feedback", phasor1);
	cout << feedback << endl;
	xTranslate.set(getXY(dutyATotal), getXY(dutyBTotal), getXY(dutyCTotal));
	yTranslate.set(getXY(frequencyATotal), getXY(frequencyBTotal), getXY(frequencyCTotal));
	zTranslate.set(getZ(sampleATotal), getZ(sampleBTotal), getZ(sampleCTotal));
	shader.setUniform3f("xTranslate", xTranslate);
	shader.setUniform3f("yTranslate", yTranslate);
	shader.setUniform3f("zTranslate", zTranslate);
	shader.setUniform3f("xDuty", getVec(9));
	shader.setUniform3f("yDuty", getVec(10));
	shader.setUniform3f("xFrequency", getVec(11));
	shader.setUniform3f("yFrequency", getVec(12));
	shader.setUniform3f("xAmplitude", getVec(13));
	shader.setUniform3f("yAmplitude", getVec(14));
	shader.setUniform3f("xPan", getPanVec(15));
	shader.setUniform3f("yPan", getPanVec(16));
}

float ofApp::getXY(float total) {
	return total * 2.0 / samplesElapsed - 1.0;
}

float ofApp::getZ(float sampleTotal) {
	return pow(((sampleTotal + 1.0) / samplesElapsed), 2.0);
}

ofVec3f ofApp::getVec(int control) {
	ofVec3f vec;
	vec.set(unipolar(controls[control]), unipolar(controls[control + 8]), unipolar(controls[control + 16]));
	return vec;
}

ofVec3f ofApp::getPanVec(int control) {
	ofVec3f vec;
	vec.set(parameters[control], parameters[control + 8], parameters[control + 16]);
	return vec;
}