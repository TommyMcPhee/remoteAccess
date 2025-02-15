#include "ofApp.h"

//--------------------------------------------------------------
constexpr void ofApp::fillWavetable() {
	for (int a = 0; a < wavetableSize; a++) {
		wavetable[a] = sin(TWO_PI * (float)a / wavetableSize);
	}
	primes[0] = true;
	primes[1] = true;
	for (int a = 2; a < limit; a++) {
		primes[a] = true;
		for (int b = a + 1; b < limit; b++) {
			if (b % a == 0) {
				primes[b - 1] = false;
			}
		}
	}
}

bool ofApp::checkPrime(int number) {
	bool prime = true;
	for (int c = 2; c < number; c++) {
		if (number % c == 0) {
			prime = false;
			c = number;
		}
	}
	return prime;
}

constexpr float ofApp::caculateKeyLimit(int maxFactor) {
	bool maxComposite = false;
	int a = maxFactor - 1;
	while (!maxComposite) {
		for (int b = 2; b < maxFactor; b++) {
			if (a % b != 0) {
				a++;
				b = maxFactor;
			}
			else {
				if (b == maxFactor - 1) {
					return (float)a;
					maxComposite = true;
				}
			}
		}
	}
}

void ofApp::hardwareSetup() {
	shader.load("ambientDrone");
	frameBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	frameBuffer.begin();
	ofClear(0, 0, 0, 255);
	frameBuffer.end();
	streamSettings.setOutListener(this);
	streamSettings.setApi(ofSoundDevice::Api::MS_WASAPI);
	streamSettings.sampleRate = sampleRate;
	streamSettings.bufferSize = bufferSize;
	streamSettings.numOutputChannels = channels;
	stream.setup(streamSettings);
}

void ofApp::setup() {
	fillWavetable();
	minimumFloat = std::numeric_limits<float>::min();
	keylimit = caculateKeyLimit(limit);
	hardwareSetup();
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	shader.setUniform2f("mouse", mouse);
	shader.setUniform2f("mouseLeft", mouseLeft);
	shader.setUniform2f("mouseCenter", mouseCenter);
	shader.setUniform2f("mouseRight", mouseRight);
	shader.setUniform1f("keytotal", keytotal);
	shader.setUniform1f("progress", progress);
	shader.setUniform1f("droneAmplitude", droneAmplitude);
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

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings streamSettings) {

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

void ofApp::audioOut(ofSoundBuffer& buffer) {
	if (audioSetup) {
		for (int a = 2; a < limit; a++) {
			float aFloat = (float)a;
			bool aPrime = checkPrime(a);
			for (int b = 1; b < a; b++) {
				bool bPrime = checkPrime(b);
				if (aPrime || bPrime && a % b != 0) {
					float bFloat = (float)b;
					oscillators[bankIndex][5] = aFloat / bFloat;
					oscillators[bankIndex + 1][5] = bFloat / aFloat;
					oscillators[bankIndex][6] = aFloat;
					oscillators[bankIndex + 1][6] = aFloat;
					bankIndex += 2;
				}
			}
		}
		for (int a = 0; a < bankIndex; a++) {
			for (int b = 0; b < 4; b++) {
				oscillators[a][b] = 0.5;
			}
		}
		audioSetup = false;
	}
	int sameSample = 0;
	for (int a = 0; a < buffer.getNumFrames(); a++) {
		progress += progressIncrement;
		fade = 1.0 - pow(abs(0.5 - progress) * 2.0, 16.0);
		if (progress > 1.0) {
			done = true;
			ofSoundStreamClose();
		}
		droneAmplitude = lookup(progress * 0.5) / (float)limit;
		phaseIncrement = (float)sampleRate / pow((float)limit, 1.0 + mouseCenter.y);
		phaseIncrements[0] = phaseIncrement * mouseCenter.x;
		phaseIncrements[1] = phaseIncrement * (1.0 - mouseCenter.x);
		dronePhase += phaseIncrement * 0.5;
		dronePhase = fmod(dronePhase, 1.0);
		droneSample = lookup(dronePhase) * droneAmplitude;
		activeOscillators = (float)bankIndex;
		sample = { 0.0, 0.0 };
		for (int b = 0; b < bankIndex; b++) {
			for (int c = 0; c < 4; c++) {
				oscillators[b][c] = lookup(fmod(progress * oscillators[b][6] * keytotal + oscillators[b][5] + (float)(c % 2 / 2) + (float)(c % 4), 1.0)) * 0.5 + 0.5;
			}
			float spatialize = averageTwo(mouseLeft.x, pow(mouseLeft.y, oscillators[b][5]), 0.5);
			oscillators[b][0] = sqrt(spatialize);
			oscillators[b][1] = sqrt(1.0 - spatialize);
			float localize = averageTwo(mouseRight.x, pow(mouseRight.y, oscillators[b][5]), 0.5);;
			oscillators[b][2] = sqrt(1.0 - localize);
			oscillators[b][3] = sqrt(localize);
		}
		for (int b = 0; b < bankIndex; b++) {
			if (keytotal > 0.0 && fmod(keytotal, oscillators[b][6]) == 0.0) {
				activeOscillators--;
				b++;
			}
			for (int c = 0; c < channels; c++) {
				float increment = phaseIncrements[c] * oscillators[b][5];
				float inverseIncrement = 1.0 - increment;
				oscillators[b][4] += increment + (droneSample * inverseIncrement * oscillators[b][c + 2]);
				oscillators[b][4] = fmod(oscillators[b][4], 1.0);
				sample[c] += oscillators[b][c] * lookup(oscillators[b][4]) / activeOscillators;
			}
		}
		for (int b = 0; b < channels; b++) {
			if (done) {
				sample[b] = 0.0;
			}
			else {
				if (sample[b] == lastSample[b]) {
					sameSample++;
				}
				if (sameSample != bufferSize - 2) {
					sample[b] = averageTwo(averageTwo(-1.0 * lastSample[b], sample[b], 1.0 - mouse.x), averageTwo(lastSample[b], sample[b], mouse.y), 0.5);
				}
				else {
					sample[b] = ofRandomf() / (float)limit;
				}
				buffer[a * channels + b] = sample[b] * fade;
				lastSample[b] = sample[b];
			}
		}
	}

}

void ofApp::checkKeys() {
	if (keytotal < 0) {
		keytotal = 0.0;
	}
	else {
		keytotal = fmod(keytotal, keylimit);
	}
}

void ofApp::updateMouse(int x, int y) {
	mouseX = 1.0 - ((float)x / width);
	mouseY = (float)y / height;
}

void ofApp::keyPressed(int key) {
	keytotal += key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keytotal -= key;
	checkKeys();
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	updateMouse(x, y);
	mouse.set(mouseX, mouseY);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	updateMouse(x, y);
	switch (button) {
	case 0:
		mouseLeft.set(mouseX, mouseY);
		break;
	case 1:
		mouseCenter.set(mouseX, mouseY);
		break;
	case 2:
		mouseRight.set(mouseX, mouseY);
		break;
	}
}