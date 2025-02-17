#include "ofApp.h"

//--------------------------------------------------------------
constexpr void ofApp::fillWavetable() {
	for (int a = 0; a < wavetableSize; a++) {
		wavetable[a] = sin(TWO_PI * (float)a / wavetableSize);
	}
}

void ofApp::setup() {
	fillWavetable();
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
		cout << series[a] << endl;
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

void ofApp::audioOut(ofSoundBuffer& soundBuffer) {
	for (int a = 0; a < soundBuffer.getNumFrames(); a++) {
		for (int b = 0; b < maxValue; b++) {
			phases[b] += b;
			phases[b] %= wavetableSize;
			amplitudes[b] = wavetable[phases[b]];
			for (int c = 0; c < channels; c++) {
				sample[c] += amplitudes[b] * volumes[b] * pan[b][c];
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
			volumes[a] = series[values[a][1]][0];
			pan[a][0] = sqrt(series[values[a][0]][0]);
			pan[a][1] = sqrt(1.0 - pan[a][0]);
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
	switch(position){
	case 0:
		xData[number] = updateValue;
		break;
	case 1:
		yData[number] = updateValue;
		break;
	case 2:
		aData[number] = updateValue;
		break;
	case 3:
		bData[number] = updateValue;
		break;
	}
}

//--------------------------------------------------------------