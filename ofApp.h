#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	static const int wavetableSize = 512, maxValue = 256, channels = 2, sampleRate = 48000, bufferSize = 256;
	array<float, 16> fibonacci;
	float series[maxValue][2];
	float phaseIncrements[maxValue];
	void setup();
	array<float, wavetableSize> wavetable;
	constexpr void fillWavetable();
	ofSoundStreamSettings streamSettings;
	ofSoundStream stream;
	void ofSoundStreamSetup(ofSoundStreamSettings& settings);
	int number = 0, position = 0, modIndex = 0;
	array<array<int, 4>, maxValue> values;
	array<array<float, 2>, maxValue> phases;
	array<int, maxValue> modulators;
	inline float averageTwo(float inA, float inB, float mix);
	inline float triangle(float phase, float skew);
	inline float lookup(float phase);
	array<float, maxValue> amplitudes, panValue, indicies, modPanValue, envelopes, increments, volumes;
	array<array<float, channels>, maxValue> pan, modPan;
	array<array<float, channels>, maxValue> samples;
	array<float, channels> sample, lastSample;
	float maxRoot, amplitude = 1.0, timer = 0.0, time = 1.0, recipriocalTime = 1.0 / time, width, height;
	void audioOut(ofSoundBuffer& soundBuffer);
	ofVec2f window;
	float data[maxValue * 4];
	ofFbo frameBuffer;
	ofShader shader;
	void setUniforms();
	void draw();
	void updateState(int number, int position);
	void keyPressed(int key);
};