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
		phaseIncrements[a] = 0.5 * series[a][0] / fibonacci[(int)modSeries - 1];
		panValue[a] = 0.5;
		modPanValue[a] = 0.5;
		envelopes[a] = 0.5;
		modEnvelopes[a] = 0.5;
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
			//add FM
			for (int c = 0; c < channels; c++) {
				phases[b][c] += phaseIncrements[b];
				phases[b][c] = fmod(phases[b][c], 1.0);
				sample[c] += lookup(phases[b][c]) * volumes[b] * pan[b][c];
			}
		}
		for (int b = 0; b < channels; b++) {
			sample[b] /= (float)maxValue;
			soundBuffer[a * channels + b] = sample[b];
			sample[b] = 0.0;
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
		for (int a = 0; a < maxValue; a++) {
			panValue[a] = series[values[a][0] % 256][0];
			modPanValue[a] = series[values[a][0] % 256][0];
			envelopes[a] = abs(0.5 - panValue[a]) * 2.0;
			modEnvelopes[a] = abs(0.5 - modPanValue[a]) * 2.0;
			//fix volumes
			volumes[a] = series[values[a][1] % 256][0];
			pan[a][0] = sqrt(panValue[a]);
			pan[a][1] = sqrt(1.0 - panValue[a]);
			modPan[a][0] = sqrt(panValue[a]);
			modPan[a][1] = sqrt(1.0 - panValue[a]);
		}
	}
	if (key > 47 && key < 59) {
		number *= 10;
		number += key - 48;
	}
}

void ofApp::updateState(int number, int position) {
	cout << number << " " << position << endl;
	values[number][position] += 1;
	float updateValue = series[values[number][position] % 256][0];
	switch (position) {
	case 0:
		xData[number] = updateValue;
		break;
	case 1:
		yData[number] = pow(updateValue * phaseIncrements[number], 0.25);
		break;
	case 2:
		aData[number] = updateValue;
		break;
	case 3:
		bData[number] = pow(updateValue * phaseIncrements[number], 0.25);;
		break;
	}
}

//--------------------------------------------------------------