#pragma once
#include "ofMain.h"
#include "ofxGLFWJoystick.h";

class Controller {
public:

	ofxGLFWJoystick theCtrls = ofxGLFWJoystick::one();

	Controller();

	// Stores the "joystick" ID
	int jID;

	// Stores the amount of buttons
	vector<int> buts;
	// Button state
	vector<bool> btS;
	vector<bool> btSPrev;

	void setup();
	void update();

//	void btnPress(int n, int jID);

	/* Axises */
	// Axis 1 x value
	float ax1x;
	// Axis 1 y value
	float ax1y;
	// Axis 2 x value
	float ax2x;
	// Axis 2 y value
	float ax2y;

	// Array size for averaging.
	int avArrSize;

	// Axis #1. Main for joystick, left stick for GamePad
	ofVec2f ax1tmp;
	// Axis #1 average. Main for joystick, left stick for GamePad
	vector<ofVec2f> ax1arr;
	ofVec2f ax1total;
	ofVec2f ax1; // Axis 1 running average

	// Axis #2. Clutch for joystick, right stick for GamePad
	ofVec2f ax2tmp;
	// Axis #2 average. Clutch for joystick, right stick for GamePad
	vector<ofVec2f> ax2arr;
	ofVec2f ax2total;
	ofVec2f ax2; // Axis 2 running average

};
