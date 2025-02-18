#include "ofApp.h"

//--------------------------------------------------------------
constexpr void ofApp::fillWavetable() {
	for (int a = 0; a < wavetableSize; a++) {
		wavetable[a] = sin(TWO_PI * (float)a / wavetableSize);
	}
}

void ofApp::setup() {
	fillWavetable();
	int fibonacciA = 1, fibonacciB = 1, fibonacciC = 1;
	for (int a = 0; a < fibonacci.size(); a++) {
		fibonacci[a] = fibonacciC;
		fibonacciC = fibonacciA + fibonacciB;
		fibonacciA = fibonacciB;
		fibonacciB = fibonacciC;
	}
	int modCount = 0;
	int modSeries = 1;
	float sumTerm = 0.5;
	for (int a = 0; a < maxValue; a++) {
		if (modCount >= modSeries) {
			if ((float)a / (float)maxValue < 0.5) {
				modSeries++;
				sumTerm = 0.25 / (float)modSeries;
			}
			else {
				modSeries--;
				sumTerm = 0.75 / (float)modSeries;
			}
			modCount = 0;
		}
		series[a][0] = ((float)(modCount % modSeries) / (float)modSeries) + sumTerm;
		series[a][1] = (float)modSeries;
		phaseIncrements[maxValue - a - 1] = pow(0.5 * series[a][0] / fibonacci[(int)modSeries - 1], 2.0);
		//phaseIncrements[a] = 0.1 / (float)(a + 1);
		panValue[a] = 0.5;
		envelopes[a] = 0.5;
		amplitudes[a] = 0.0;
		modPanValue[a] = 0.5;
		modEnvelopes[a] = 0.5;
		indicies[a] = 0.0;
		for (int b = 0; b < 2; b++) {
			pan[a][b] = 0.5;
			modPan[a][b] = 0.5;
		}
		for (int b = 0; b < 4; b++) {
			values[a][b] = -1;
		}
		cout << phaseIncrements[a] << endl;
		modCount++;
	}

	shader.load("remoteAccess");
	frameBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	frameBuffer.begin();
	ofClear(0, 0, 0, 255);
	frameBuffer.end();
	shader.begin();
	shader.setUniform1fv("series0", series[0], maxValue);
	shader.end();
	streamSettings.setOutListener(this);
	streamSettings.setApi(ofSoundDevice::Api::MS_WASAPI);
	streamSettings.sampleRate = sampleRate;
	streamSettings.bufferSize = bufferSize;
	streamSettings.numOutputChannels = channels;
	stream.setup(streamSettings);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings& settings) {

}

inline float ofApp::averageTwo(float inA, float inB, float mix) {
	return (1.0 - mix) * inA + (inB * mix);
}

inline float ofApp::lookup(float phase) {
	float floatIndex = phase * (float)wavetableSize;
	float remainderIndex = fmod(floatIndex, 1.0);
	int intIndex = (int)floatIndex;
	if (intIndex >= wavetableSize - 1) {
		intIndex = wavetableSize - 2;
	}
	return averageTwo(wavetable[intIndex], wavetable[intIndex + 1], remainderIndex);
}

void ofApp::audioOut(ofSoundBuffer& soundBuffer) {
	for (int a = 0; a < soundBuffer.getNumFrames(); a++) {
		for (int b = 0; b < maxValue; b++) {
			amplitude++;
			for (int c = 0; c < channels; c++) {
				phases[b][c] += sample[phaseIncrements[modulators[b]]] * indicies[b] * modPan[b][c] + phaseIncrements[b];
				phases[b][c] = fmod(phases[b][c], 1.0);
				samples[b][c] = lookup(phases[b][c]) * pan[b][c];
				sample[c] += samples[b][c] * amplitudes[b];
				}
		}
		for (int b = 0; b < channels; b++) {
			sample[b] = (sample[b] + 1.0) / amplitude;
			soundBuffer[a * channels + b] = sample[b];
			sample[b] = 0.0;
			amplitude = 1.0;
		}
	}
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	shader.setUniform1fv("xData", xData, maxValue);
	shader.setUniform1fv("yData", yData, maxValue);
	shader.setUniform1fv("aData", aData, maxValue);
	shader.setUniform1fv("bData", bData, maxValue);
}

//--------------------------------------------------------------
void ofApp::draw() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	window.set(width, height);
	frameBuffer.allocate(width, height);
	frameBuffer.begin();
	ofClear(0, 0, 0, 255);
	shader.begin();
	setUniforms();
	frameBuffer.draw(0.0, 0.0);
	shader.end();
	frameBuffer.end();
	frameBuffer.draw(0.0, 0.0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 44) {
		updateState(number, position);
		number = 0;
		position++;
	}
	if (key == 41) {
		updateState(number, position);
		number = 0;
		position = 0;
	}
	if (key > 47 && key < 59) {
		number *= 10;
		number += key - 48;
	}
}

void ofApp::updateState(int number, int position) {
	cout << number << " " << position << endl;
	cout << "amplitude" << amplitude << endl;
	values[number][position] += 1;
	//float updateValue = series[values[number][position] % 256][0];
	switch (position) {
	case 0:
		panValue[number] = series[values[number][0] % 256][0];
		pan[number][0] = sqrt(panValue[number]);
		pan[number][1] = sqrt(1.0 - panValue[number]);
		envelopes[number] = abs(0.5 - panValue[number]) * 2.0;
		//xData[number] = updateValue;
		break;
	case 1:
		if (values[number][1] < 256) {
			modIndex = number;
			amplitudes[number] = phaseIncrements[values[number][1]] * 2.0;
		}
		else {
			amplitudes[number] = 0.0;
		}
		//yData[number] = pow(updateValue * phaseIncrements[number], 0.25);
		break;
	case 2:
		modPanValue[number] = series[values[number][2] % 256][0];
		modPan[number][0] = sqrt(panValue[number]);
		modPan[number][1] = sqrt(1.0 - panValue[number]);
		modEnvelopes[number] = abs(0.5 - modPanValue[number]) * 2.0;
		//aData[number] = updateValue;
		break;
	case 3:
		if (values[number][1] < 256) {
			modulators[modIndex] = number;
			indicies[number] = phaseIncrements[values[number][1]];
		}
		else {
			indicies[number] = 0.0;
		}
		//bData[number] = pow(updateValue * phaseIncrements[number], 0.25);;
		break;
	}
}

//--------------------------------------------------------------