#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int wavetableSize = 2056;
	array<float, wavetableSize> wavetable;
	static const int limit = 9;
	static const int pseudoLimit = limit - 1;
	array<bool, limit> primes;
	constexpr void fillWavetable();
	constexpr float caculateKeyLimit(int maxFactor);
	array<array<float, 8>, pseudoLimit* pseudoLimit> oscillators;
	bool checkPrime(int number);
	ofShader shader;
	ofFbo frameBuffer;
	ofSoundStreamSettings streamSettings;
	const int sampleRate = 48000;
	const int bufferSize = 2056;
	const int channels = 2;
	const float length = (float)sampleRate * 1200.0;
	const float progressIncrement = 1.0 / length;
	float phaseIncrement = 0.5 * (float)sampleRate / (float)(limit);
	int bankIndex = 0;
	ofSoundStream stream;
	void hardwareSetup();
	float minimumFloat, keylimit;
	void setup();
	float width, height;
	ofVec2f window, mouse, mouseLeft, mouseCenter, mouseRight;
	void setUniforms();
	void draw();
	void ofSoundStreamSetup(ofSoundStreamSettings settings);
	bool audioSetup = true, done = false;
	float averageTwo(float inA, float inB, float mix);
	float lookup(float phase);
	float progress = 0.0, droneAmplitude = 0.0, dronePhase = 0.0, droneSample = 0.0, activeOscillators, fade = 1.0;
	float keytotal = 0.0, mouseXLeft = 0.0, mouseX = 0.5, mouseY = 0.5;
	array<float, 2> phaseIncrements, lastSample, sample;
	void audioOut(ofSoundBuffer& audioBuffer);
	void checkKeys();
	void updateMouse(int x, int y);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
};