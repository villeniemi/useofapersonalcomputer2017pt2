#include "controller.h"



Controller::Controller() { // Constructor function
//	jId = jId;
//	void update();
}


void Controller::setup() {
//	jID = newJID;

	avArrSize = 30;
	ax1arr.resize(avArrSize);
	ax2arr.resize(avArrSize);

	for (int i = 0; i < buts.size(); i++) {
		btSPrev[i] = btS[i] = false;
	}

}

void Controller::update() {

		ax1x = theCtrls.getAxisValue(0, jID)*100;
		ax1y = theCtrls.getAxisValue(1, jID)*100;
	ax1tmp = ofVec2f(ax1x, ax1y);
	ax1total = ofVec2f(0, 0);
	if (ax1arr.size() < avArrSize) { ax1arr.push_back(ax1tmp); }
	else { ax1arr.erase(ax1arr.begin()); ax1arr.push_back(ax1tmp); }
	for (int i = 0; i < ax1arr.size(); i++) {
			ax1total += ax1arr[i];
		}
	ax1 = ax1total / ax1arr.size();
	

		ax2x = theCtrls.getAxisValue(2, jID)*100;
		ax2y = theCtrls.getAxisValue(3, jID)*100;
	ax2tmp = ofVec2f(ax2x, ax2y);
	ax2total = ofVec2f(0, 0);
	if (ax2arr.size() < avArrSize) { ax2arr.push_back(ax2tmp); }
	else { ax2arr.erase(ax2arr.begin()); ax2arr.push_back(ax2tmp); }
	for (int i = 0; i < ax2arr.size(); i++) {
		ax2total += ax2arr[i];
	}
	ax2 = ax2total / ax2arr.size();


	if (ax1.x < -10 || ax1.x > 10 || ax1.y < -10 || ax1.y > 10 ||
		ax2.x < -10 || ax2.x > 10 || ax2.y < -10 || ax2.y > 10) {
	//	cout << "Controller #" << jID << "z ax1 " << ax1 << " ax2 " << ax2 << "\n";
	}
	
	// Check button on/off
	for (int i = 0; i < buts.size(); i++) {
		btSPrev[i] = btS[i];
		if (theCtrls.getButtonValue(i, jID)>0)
			btS[i] = true;
		else
			btS[i] = false;
		if (btS[i]) {
	//		cout << "Controller #" << jID << " button " << i << " is " << btS[i] << "\n";
		}
		/*
		if (btS[i] == false && btSPrev[i] == true) {
			btnPress(i, jID);
		}
		*/
	}
	/*
	cout << ofToString(xval*100) << "\n";
	cout << derp << "\n";
	*/
}

/*
	/* GamePad
	---------------------------------------------
	AXIS#0 = L STICK X	|	AXIS#1 = L STICK Y
	AXIS#2 = R STICK X	|	AXIS#3 = L STICK Y

	0 = []		1 = X		2 = ()		3 = /\

	4 = L1		5 = R1		6 = L2		7 = R2

	10 = L3		11 = L4

	8 = SELECT		9 = START

	12 = UP		13 = RIGHT	14 = DOWN	15 = LEFT
	---------------------------------------------
	Joystick
	---------------------------------------------
	AXIS#0 = L/R		|	AXIS#1 = UP/DOWN
	AXIS#2 = VIPU

	0 = trigger				1 = middle button
	2 = right button		3 = "b"

	4 = UP	5 = RIGHT

	---------------------------------------------
	*/


/*
void Controller::btnPress(int n) {
	cout << "Btn " << n << " released";
}
*/