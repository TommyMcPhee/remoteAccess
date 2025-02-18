#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	static const int wavetableSize = 512, maxValue = 256, channels = 2, sampleRate = 48000, bufferSize = 256;
	array<float, 16> fibonacci;
	float series[maxValue][2];
	void setup();
	array<float, wavetableSize> wavetable;
	constexpr void fillWavetable();
	ofSoundStreamSettings streamSettings;
	ofSoundStream stream;
	void ofSoundStreamSetup(ofSoundStreamSettings& settings);
	int number = 0, position = 0;
	array<array<int, 4>, maxValue> values;
	array<array<float, 2>, maxValue> phases;
	array<float, maxValue> phaseIncrements;
	inline float averageTwo(float inA, float inB, float mix);
	inline float lookup(float phase);
	array<float, maxValue> volumes;
	array<array<float, channels>, maxValue> pan;
	array<float, channels> sample;
	void audioOut(ofSoundBuffer& soundBuffer);
	float width, height;
	ofVec2f window;
	ofFbo frameBuffer;
	ofShader shader;
	float xData[maxValue], yData[maxValue], aData[maxValue], bData[maxValue];
	void setUniforms();
	void draw();
	void updateState(int number, int position);
	void keyPressed(int key);
};