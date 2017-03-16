#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	ofDisableArbTex();
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofEnableDepthTest();
	ofSetFrameRate(35);
	ofHideCursor();
	ofSetLineWidth(1);

	w = ofGetWidth();
	h = ofGetHeight();

		// ---------------- camera ---------------- 
		ofEnableNormalizedTexCoords();
		cam.setFarClip(80000.0f);
		cout << "cam.getFarClip: " << ofToString(cam.getFarClip()) << endl;
		moveSpeed = 3.0;

		camLockedView = ofVec3f(0, -h/3, 0);


		// ---------------- models ---------------- 
		material.setShininess(120);
		// the light highlight of the material //
//		material.setDiffuseColor(ofColor(0,255));
//		material.setSpecularColor(ofColor(255, 255, 255, 255));
		macScreenMaterial.setShininess(120);
//		macScreenMaterial.setSpecularColor(ofColor(255, 255, 255, 255));

		modelVille.loadModel("ville.obj", false);
//		modelVille.loadModel("ville_bl.obj", false);
		modelVille.setRotation(1, 180, 0, 0, 1);

		modelMac.loadModel("mpm_F18.OBJ", false);
		macRotateSpeed = 0;
		macSelfRotateSpeed = 0;
		macRotateAmount = 0.0;
		macSelfRotateAmount.resize(1000);
		for (int i = 0; i < macSelfRotateAmount.size(); i++) {
			macSelfRotateAmount[i] = 0.0;
		}

		// ---------------- screen grab ---------------- 
		initScreenGrab();
		bScreenGrab = false;
		bScreenGrabDraw = false;
		bLiveFootage = false;

		ofScreenGrab.allocate(w, h, GL_RGB);

		// ---------------- stock screenshots ---------------- 
		screenshotSpeed = 0;
		ofDirectory dir;
		int nFiles = dir.listDir("screenshots");
		if (nFiles) {
			for (int i = 0; i < dir.size(); i++) {
				// add the image to the vector
				string filePath = dir.getPath(i);
				screenshotImgs.push_back(ofImage());
				screenshotImgs.back().load(filePath);
			}
			cout << ofToString(nFiles) << " images found in bin/data/screenshots." << endl;
		}
		else ofLog(OF_LOG_WARNING) << "Could not find folder";

		for (int i = 0; i < screenshotImgs.size(); i++) {
			screenshotImgs[i].mirror(true, false);
		}


		screenshotIndex = 0;

		fbScreenshot.load("fbScreenshot.jpg");
		fbScreenshot.mirror(true, false);

		webcamGrabber.listDevices();
		webcamGrabber.setDeviceID(1);
		webcamGrabber.setup(640, 480, true);
		

		// ---------------- image cubes ---------------- 
		boxCols = 10;
		boxRows = 6;
		boxCount = boxCols * boxRows;
		splitW = capture_dst_width / boxCols;
		splitH = capture_dst_height / boxRows;
		// captureImgSplits.resize(boxCount);
		for (int i = 0; i < boxCount; i++) {
			//		captureImgSplits[i].setFromPixels(captureImg.getPixels());
		//	captureImgSplits[i].allocate(splitW, splitH, GL_RGBA);
		}


		// ---------------- sound ---------------- 
		ofSoundStreamListDevices();
		ofSoundStreamSetup(0, 2, this, 44100, beat.getBufferSize(), 4);

		beatBandsMult = 4;
		bands.resize(16);
		bandAvLength = 20;
		for (int i = 0; i < bands.size(); i++) {
			bands[i].bandTot = 0;
			bands[i].band.resize(bandAvLength);
			for (int b = 0; b < bandAvLength; b++) {
				bands[i].band[b] = 0;
				bands[i].bandTot += bands[i].band[b];
			}
			bands[i].bandAv = bands[i].bandTot / bandAvLength;
		}

		beatCounter = 0;

		// ---------------- random boxes ---------------- 
		initRandomBoxes();


		// ---------------- controller  ---------------- 
		ofxGLFWJoystick::one().printJoystickList();
		for (int i = 0; i < ofxGLFWJoystick::one().getNumJoysticksAvailable(); i++) {
			Controller tmpCtrl;
			tmpCtrl.jID = i;
			int buttonCount = ofxGLFWJoystick::one().getNumButtons(i);
			for (int n = 0; n < buttonCount; n++) {
				tmpCtrl.buts.push_back(n);
				tmpCtrl.btS.push_back(n);
				tmpCtrl.btSPrev.push_back(n);
			}
			ctrls.push_back(tmpCtrl);
			ctrls[i].setup();
		}

		mult = 0.8;


		// ---------------- booleans ---------------- 
		bInfo = false;
		bDrawScreenCubes = false;
		bDrawGridFloor = true;
		bDrawBeatInfo = false;
		bDrawBands = true;
		bJoystickInfo = false;

		moveUp = moveDown = moveIn = moveOut = moveLeft = moveRight = panLeft = panRight = tiltUp = tiltDown = rollLeft = rollRight = false;

		floorSize = (tileCount*tileSize);

		roof = floorSize*0.90625 - h/2;
		floor = -h/2;
		



		// Post FX
		post.init(ofGetWidth(), ofGetHeight());

		post.createPass<FxaaPass>()->setEnabled(true);
		post.createPass<BloomPass>()->setEnabled(false);
		dof = post.createPass<DofPass>();
		dof->setEnabled(true);
		//		dofalt = post.createPass<DofAltPass>();
//		dofalt->setEnabled(false);
		zoomblur = post.createPass<ZoomBlurPass>();
		zoomblur->setEnabled(false);
		post.createPass<GodRaysPass>()->setEnabled(false);
		post.createPass<RimHighlightingPass>()->setEnabled(false);
		post.createPass<SSAOPass>()->setEnabled(false);
		kaleido = post.createPass<KaleidoscopePass>();
		kaleido->setEnabled(false);
		noisewarp = post.createPass<NoiseWarpPass>();
		noisewarp->setEnabled(false);
		rgbshift = post.createPass<RGBShiftPass>();
		rgbshift->setEnabled(false);



		// ---------------- lights ---------------- 
		ofSetSmoothLighting(true);
		pointLight.setDiffuseColor(ofFloatColor(255, 255, 255, 40.0f));
		//		pointLight.setDiffuseColor(ofFloatColor(.85, .85, .55));
		//		pointLight.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));

		//		pointLight2.setDiffuseColor(ofFloatColor(238.f / 255.f, 57.f / 255.f, 135.f / 255.f));
		pointLight2.setDiffuseColor(ofFloatColor(255, 255, 255, 40.0f));
		//		pointLight2.setSpecularColor(ofFloatColor(.8f, .8f, .9f));

		//		pointLight3.setDiffuseColor(ofFloatColor(19.f / 255.f, 94.f / 255.f, 77.f / 255.f));
		pointLight3.setDiffuseColor(ofFloatColor(255, 255, 255, 40.0f));
		//		pointLight3.setSpecularColor(ofFloatColor(18.f / 255.f, 150.f / 255.f, 135.f / 255.f));

		//		ambientLight.setDiffuseColor(ofColor(255));
		//		ambientLight.setSpecularColor(ofColor(255));


		wallLight1.setDiffuseColor(ofColor(255.0f, 255.0f, 255.0f, 80));
		wallLight1.setSpecularColor(ofColor(255.0f, 255.0f, 255.0f));
		wallLight1.setSpotlight(66.0f, 40.0f);
		// cutoff: 66 | concent: 40
		wallLight1.tilt(-90);
		wallLight1.pan(20);

		wallLight2.setDiffuseColor(ofColor(255.0f, 255.0f, 255.0f, 80));
		wallLight2.setSpecularColor(ofColor(255.0f, 255.0f, 255.0f));
		wallLight2.setSpotlight(66.0f, 40.0f);
		wallLight2.tilt(-70);

		wallLight3.setDiffuseColor(ofColor(255.0f, 255.0f, 255, 80));
		wallLight3.setSpecularColor(ofColor(255.0f, 255.0f, 255.0f));
		wallLight3.setSpotlight(66.0f, 40.0f);
		wallLight3.tilt(-90);
		wallLight3.pan(-20);

		ambientLight.setDiffuseColor(ofColor(0,0));
		ambientLight.setSpecularColor(ofColor(0,0));
		ambientLight.setAmbientColor(ofColor(255, 120));
		ambientLight.setPosition(0, roof/2, 0);
		
		lightCircleCount = 0.0;

		cout << "focus: " << ofToString(dof->getFocus()) << endl;
		cout << "maxblur: " << ofToString(dof->getMaxBlur()) << endl;
		cout << "aperture: " << ofToString(dof->getAperture()) << endl;

}

//--------------------------------------------------------------
void ofApp::update() {
	ofBackground(0);

	// ---------------- controller ---------------- 
	ofxGLFWJoystick::one().update();
	for (int i = 0; i < ctrls.size(); i++) {
		ctrls[i].update();
		for (int b = 0; b < ctrls[i].buts.size(); b++) {
			if (ctrls[i].btS[b] == false && ctrls[i].btSPrev[b] == true) {
				// After comparing to previous state, if change from true to false (released), do btnPress.
				btnPress(b, i);
			}
		}
		checkCtrl(i);
		controllerAsCamera(i);
	}


	// ---------------- lights ---------------- 
	if (bPoint1) {
		pointLight.setPosition((ofGetWidth()*.5) + cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.3), ofGetHeight() / 2, 500);
	}
	if (bPoint2) {
		pointLight2.setPosition((ofGetWidth()*.5) + cos(ofGetElapsedTimef()*.15)*(ofGetWidth()*.3),
			ofGetHeight()*.5 + sin(ofGetElapsedTimef()*.7)*(ofGetHeight()), -300);
	}
	if (bPoint3) {
		pointLight3.setPosition(
			cos(ofGetElapsedTimef()*1.5) * ofGetWidth()*.5,
			sin(ofGetElapsedTimef()*1.5f) * ofGetWidth()*.5,
			cos(ofGetElapsedTimef()*.2) * ofGetWidth()
		);
	}

	lightCircleCount += 0.01;
	
//	wallLight1.setPosition(0, floorSize*0.99-h/2, 0);
	wallLight1.setPosition(-floorSize/2.5, roof*0.99, 0);
	wallLight2.setPosition(0, roof*0.99, -floorSize / 2.5);
	wallLight3.setPosition(floorSize/2.5, roof*0.99, 0);

	pointLight.setDiffuseColor(ofColor(spotsAlpha, spotsAlpha));
	pointLight2.setDiffuseColor(ofColor(spotsAlpha, spotsAlpha));
	pointLight3.setDiffuseColor(ofColor(spotsAlpha, spotsAlpha));

	wallLight1.setSpotlightCutOff(spotsCutoff);
	wallLight2.setSpotlightCutOff(spotsCutoff);
	wallLight3.setSpotlightCutOff(spotsCutoff);
	wallLight1.setSpotConcentration(spotsConcent);
	wallLight2.setSpotConcentration(spotsConcent);
	wallLight3.setSpotConcentration(spotsConcent);
	

//	wallLight1.setPosition(floorSize / 2, floorSize - h/2, floorSize / 2 - 100);
//	wallLight1.rotate(mouseX / 360, 1, 0, 0);


	ambientLight.setAmbientColor(ofColor(ambientLightAlpha, ambientLightAlpha));


	// ---------------- camera ---------------- 
	if (bCamBreathe) {
		ofVec3f curPos = cam.getPosition();
		float sinPosY = sin(ofGetElapsedTimef()/2);
		cam.move(ofVec3f(0, sinPosY*0.5, 0));
		float sinTilt = sin(ofGetElapsedTimef()/2);
		cam.tilt(sinTilt*0.05);
	}

	if (bCamAutomatic) {
		ofVec3f moveVals = camAutoMove.get()*mult;
		cam.truck(moveVals.x);
		cam.boom(moveVals.y);
		cam.dolly(moveVals.z);		
		cam.pan(camAutoPan*mult);
	}


	// ----------------- post fx ---------------------

	zoomblur->setDensity(zoomBlurDensity);
	zoomblur->setDecay(zoomBlurDecay);
	dof->setEnabled(dofOn);
	dof->setAperture(dofAperture);
	dof->setFocus(dofFocus);
	dof->setMaxBlur(dofMaxBlur);
//	dofalt->setFocalDepth(focalDepth);
//	dofalt->setFocalLength(focalLength);
//	dofalt->setShowFocus(true);


	// ---------------- screen grabs ---------------- 

	if (bScreenGrab) {
		updateScreenGrab();
		/*
		if (bDrawScreenCubes) {
			int curRow = 0;
			int curCol = 0;
			for (int i = 0; i < boxCount; i++) {
				//		captureImgSplits[i].setFromPixels(captureImg.getPixels());
				captureImgSplits[i].clear();
				captureImgSplits[i].begin();
//				captureImg.draw(0,0);
				captureImg.drawSubsection(0, 0, splitW, splitH, splitW*curCol, splitH*curRow);
//				captureImgSplits[i] = captureImg;
//				captureImgSplits[i].crop(splitW*curCol, splitH*curRow, splitW, splitH);
				captureImgSplits[i].end();
				curCol++;
				if (curCol == boxCols) {
					curRow++;
					curCol = 0;
				}
			}
		}
		*/
	}

	// ---------------- webcam grab ---------------- 
	if (bDrawFacebookWall) {
		webcamGrabber.update();
	}


	// ----------------------- sound -----------------------
	beat.update(ofGetElapsedTimeMillis());
	for (int i = 0; i < bands.size(); i++) {
		bands[i].bandTot = 0;
		for (int b = 0; b < bandAvLength; b++) {
	//		bands[i].band[b] = 0;
			bands[i].band.erase(bands[i].band.begin());
			bands[i].band.push_back(beat.getBand(i));
			bands[i].bandTot += bands[i].band[b];
		}
		bands[i].bandAv = bands[i].bandTot / bandAvLength;
	}

	if (beat.kick()>0.95) {
		if (beatTimer > ofGetElapsedTimeMillis()) {
			beatCounter++; if (beatCounter > 4) { beatCounter = 1; }
			beatTimer = ofGetElapsedTimeMillis();
		}
	}
	else {
		beatTimer = ofGetElapsedTimeMillis()+50;
	}
		
	// ----------------------- randombox update -----------------------
	if (but_initRandoms) { initRandomBoxes(); but_initRandoms = false; }
	for (int i = 0; i < randomBoxCount; i++) {
		int b = randomBeatBoxes[i].listeningTo;
		float band = bands[b].bandAv;
		float sY = bandCubeSize*band * beatBandsMult;
		randomBeatBoxes[i].setHeight(sY);
		ofVec3f curPos = randomBeatBoxes[i].getPosition();
		float pY = (bandCubeSize*band * beatBandsMult) / 2;
		randomBeatBoxes[i].setPosition(curPos.x, pY, curPos.z);

	}

	// ----------------------- stock screenshots -----------------------
	if (screenshotSpeed>0) {
			screenshotIndex = (int)(ofGetElapsedTimef() * screenshotSpeed) % screenshotImgs.size();
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	float cloudSize = w;
	float maxBoxSize = 100;
	float spacing = 1;

//	cam.begin();


	ofEnableLighting();

	material.begin();
	
	if (bPoint1) { pointLight.enable(); }
	if (bPoint2) { pointLight2.enable(); }
	if (bPoint3) { pointLight3.enable(); }
	if (bWall1) { wallLight1.enable(); }
	if (bWall2) { wallLight2.enable(); }
	if (bWall3) { wallLight3.enable(); }
	if (bAmbient) { ambientLight.enable(); }

	post.begin(cam);


		// ----------------------- helper grid floor ----------------------- 

		if (bDrawHelperGrid) {
			// Helper grid
			glPushMatrix();
				glTranslatef(0, -h / 2, 0);
				ofSetColor(255);
				ofRotateZ(90.0f);
				ofDrawGridPlane(tileSize, tileCount, true);
			glPopMatrix();
		}

		// ----------------------- grid floor ----------------------- 

		if (bDrawGridFloor) {
			ofPushMatrix();
			glTranslatef(0, -h / 2 + 30, 0);
			if (bDrawGridFloorPlane) {
				glPushMatrix();
			//	ofSetColor(255);
			//	material.begin();
				glTranslatef(0, -2, 0);
				glRotatef(90, 1, 0, 0);
				ofDrawBox(floorSize, floorSize, 2);
			//	material.end();
				glPopMatrix();
			}
			glPushMatrix();
			// Center the floor we bout to draw
			glTranslatef(-tileCount*tileSize / 2, 0, -tileCount*tileSize / 2);
			for (int i = 0; i < tileCount; i++) {
//					ofSetColor(0,0,0,120);
//				ofSetColor(255, 255, 255);
			//	material.begin();
					ofDrawLine(i*tileSize + i*tileGap, 0, 0, i*tileSize + i*tileGap, 0, tileCount*tileSize);
					ofDrawLine(0, 0, i*tileSize + i*tileGap, tileCount*tileSize, 0, i*tileSize + i*tileGap);
			//	material.end();
			}
			glPopMatrix();
			ofPopMatrix();
		}

		// ----------------------- draw bands ----------------------- 
		if (bDrawBands) {
			glPushMatrix();
			// start from tile corner
			//			ofTranslate(-(tileCount / 2 * tileSize + tileGap) / 2, -h / 2, -(tileCount*tileSize) / 2);
			ofTranslate(tileSize / 2, -h / 2, tileSize / 2);
			// adjust to middle of tiles
			//			ofTranslate(tileSize * (tileCount/4) + (tileCount / 4), 0, tileSize * (tileCount / 4) + (tileCount / 4));

			int bandIterator = bands.size() / 2;
			for (int bI = 0; bI < bandIterator; bI++) {
				glPushMatrix();
				//		ofTranslate(-bI*bandCubeSize - (bI*tileGap), 0, -bI*bandCubeSize - (bI*tileGap));
				//				ofTranslate(0, 0, -bI*bandCubeSize - (bI*tileGap));
				for (int i = 0; i < bandIterator - bI; i++) {
					float beatMult = beatBandsMult;
					if (i == bandIterator - 1) {
						beatMult = beatBandsMult*0.6;
					}
					//					float band = bands[bandIterator - 1 - i].bandAv;
					float band = bands[bandIterator - 1 - i].bandAv;
					float pX = i*(bandIterator - i)*bandCubeSize - (bI + 1)*bandCubeSize;
					float pY = (bandCubeSize*band * beatMult) / 2;
					float pZ = -bI*(bandCubeSize) - (bI*tileGap);
					float sX = bandCubeSize - 2;
					float sY = bandCubeSize*band * beatMult;
					float sZ = bandCubeSize - 2;
					ofFill();
		//		material.begin();
		//			ofSetColor(0, 0, 0); //fill color  
										 //				ofDrawBox(i*bandCubeSize + (i*tileGap), (bandCubeSize*band * 4) / 2, 0, bandCubeSize, bandCubeSize*band * 4, bandCubeSize);
					ofDrawBox(pX, pY, pZ, sX, sY, sZ);
					ofDrawBox(pX, pY, -pZ, sX, sY, sZ);
					ofDrawBox(-pX, pY, pZ, sX, sY, sZ);
					ofDrawBox(-pX, pY, -pZ, sX, sY, sZ);
			//		material.end();
					ofPushStyle();
					ofNoFill();
					//						ofSetLineWidth(2);
//					ofSetColor(255, 255, 255);//stroke color 
			//		material.begin();
					ofDrawBox(pX, pY, pZ, sX, sY, sZ);
					ofDrawBox(pX, pY, -pZ, sX, sY, sZ);
					ofDrawBox(-pX, pY, pZ, sX, sY, sZ);
					ofDrawBox(-pX, pY, -pZ, sX, sY, sZ);
			//		material.end();

					//					ofDrawBox(i*bandCubeSize + (i*tileGap), (bandCubeSize*band * 4)/2, 0, bandCubeSize, bandCubeSize*band * 4, bandCubeSize);
					ofPopStyle();
				}
				glPopMatrix();
			}

			for (int i = 0; i < randomBoxCount; i++) {
				ofFill();
			//	ofSetColor(255);
			//	material.begin();
				randomBeatBoxes[i].draw();
			//	material.end();

			}

			glPopMatrix();

		}

		// ----------------------- cube wall ----------------------- 

		if (bInaCube) {
			
			ofColor curColor = material.getDiffuseColor();
			ofColor curColorSpecu = material.getSpecularColor();
			material.setDiffuseColor(ofColor(curColor.r, curColor.g, curColor.b, bInaCubeAlpha));
			material.setSpecularColor(ofColor(curColorSpecu.r, curColorSpecu.g, curColorSpecu.b, bInaCubeAlpha));

			if (bOfScreenGrabDraw) {
				ofScreenGrab.bind();
			}
			else if (!bScreenGrabDraw) {
				screenshotImgs[screenshotIndex].bind();
			}
			else if (bScreenGrabDraw) {
				captureImg.bind();
			}

			//			ofDrawBox(ofPoint(0, floorSize/2 - h/2 - 100, 0), floorSize+2, floorSize+2, floorSize+2);
			for (int i = 0; i < 4; i++) {
				glPushMatrix();
					glRotatef(90 * i, 0, 1, 0);
					glTranslatef(0, (floorSize*0.90625) / 2 - h / 2, -floorSize / 2);
//					glRotatef(180, 0, 1, 0);
					ofPushStyle();
						ofPushMatrix();
//							glRotatef(180, 0, 0, 1);
							ofDrawBox(floorSize, floorSize*0.90625, 2);
						ofPopMatrix();
					ofPopStyle();
				glPopMatrix();
			}
			glPushMatrix();
				glTranslatef(0, (floorSize*0.90625) - h / 2, 0);
//			glPopMatrix();
//				glRotatef(180, 1, 0, 0);
				ofDrawBox(floorSize, 2, floorSize);
//			glPushMatrix();
				glTranslatef(0, -(floorSize*0.90625 + h/2), 0);
//				glRotatef(180, 1, 0, 0);
				ofDrawBox(floorSize, 2, floorSize);
			glPopMatrix();

			if (bScreenGrabDraw) {
				captureImg.unbind();
			}
			else if (!bScreenGrabDraw) {
				screenshotImgs[screenshotIndex].unbind();
			}
			else if (bOfScreenGrabDraw) {
				ofScreenGrab.unbind();
			}
//			material.setSpecularColor(prevColor);
		//	material.end();


			material.setDiffuseColor(ofColor(curColor.r, curColor.g, curColor.b, curColor.a));
			material.setSpecularColor(ofColor(curColorSpecu.r, curColorSpecu.g, curColorSpecu.b, curColorSpecu.a));


		}

		/*
		if (bScreenGrabDraw) {
			ofSetColor(255);
			captureImg.draw(-w/2, -h/2, -100, w, h);
		}
		*/

		// ----------------------- facebook wall ----------------------- 

		if (bDrawFacebookWall) {
			for (int i = 0; i < 4; i++) {
				glPushMatrix();
				glRotatef(90*i, 0, 1, 0);
//				glTranslatef(0, floorSize*0.90625, -floorSize / 2);
				glTranslatef(0, (floorSize*0.90625)/2 - h/2, -floorSize / 2 - 10);
				//		glTranslatef(-floorSize / 2, -floorSize / 2, 0);
				ofPushStyle();
				ofSetColor(255);
				fbScreenshot.getTexture().bind();
				ofPushMatrix();
//					ofPushMatrix();
					ofDrawBox(floorSize, floorSize*0.90625,1);
//					ofPopMatrix();
		//		ofDrawPlane(floorSize,floorSize*0.90625);
	//				ofDrawPlane(1920, 1740);
					fbScreenshot.getTexture().unbind();
					ofTranslate(-floorSize*0.09375, -floorSize*0.04023);
					webcamGrabber.getTexture().bind();
					ofPushMatrix();
					glTranslatef(0, 800, 3);
				//	glRotatef(180, 1, 0, 0);
					ofDrawBox(floorSize*0.2484375, floorSize*0.21264, 2);
					ofPopMatrix();
					webcamGrabber.getTexture().unbind();
				ofPopMatrix();
				ofPopStyle();
				glPopMatrix();
			}
		}


		// ----------------------- screen cubes ----------------------- 
		if (bDrawScreenCubes) {
			ofPushMatrix();
			ofTranslate(0, -h / 2);
//			ofTranslate(-capture_dst_width / 2, -h/2);
			int curRow = 0;
			int curCol = 1;
			for (int i = 0; i < boxCount; i++) {
				ofPushMatrix();

				float t = (ofGetElapsedTimef() + i * spacing) * cubeMovementSpeed;
				ofVec3f pos(
					ofSignedNoise(t, 0, 0),
					ofSignedNoise(0, t, 0),
					ofSignedNoise(0, 0, t));

				float boxSize = maxBoxSize * ofNoise(pos.x, pos.y, pos.z);

				pos *= cloudSize;
				ofTranslate(pos);
				ofRotateX(pos.x);
				ofRotateY(pos.y);
				ofRotateZ(pos.z);
				
//			ofTranslate(splitW*curCol, splitH*curRow);
//			captureImgSplits[i].crop(splitW*curCol, splitH*curRow, splitW, splitH);
//			ofTranslate(0, 0, left[i*(left.size()/captureImgSplits.size())] * 120.0f);

				if (bLiveFootage) {
					captureImg.getTexture().bind();
				}
				else {
					screenshotImgs[screenshotIndex].getTexture().bind();
				}
				ofPushStyle();
					ofFill();
					ofSetColor(255);
					ofSetLineWidth(0);
					ofDrawBox(splitW*curCol, splitH*curRow, 0, splitW, splitH, 20);
					ofPopStyle();
				if (bLiveFootage) {
					captureImg.getTexture().unbind();
				}
				else {
					screenshotImgs[screenshotIndex].getTexture().unbind();
				}
				ofPopMatrix();

				curCol++;
				if (curCol > boxCols) {
					curRow++;
					curCol = 1;
				}
			}
			ofPopMatrix();
		}

		// ----------------------- macs ----------------------- 

		if (bDrawMacs) {

			glPushMatrix();

//			glTranslatef((-(tileCount*tileSize) / 2), 0, ((tileCount*tileSize) / 2));

			float macsAngle = 360 / macCount;
			glRotatef(macRotateAmount, 0, 1, 0);
			macRotateAmount += macRotateSpeed*0.01;

			for (int i = 0; i < macCount; i++) {

				macSelfRotateAmount[i] += i*0.025;

				glRotatef(macsAngle, 0, 1, 0);

				glPushMatrix();

				glTranslatef(0, 0, ((tileCount*tileSize) / 2.5));

				// MAC
				ofRotateY(macSelfRotateAmount[i]);
				glRotatef(180, 1, 0, 1);
				glTranslatef(0, 100, 0);
				modelMac.drawFaces();
				ofPushStyle();
				ofPushMatrix();
				//				macScreenMaterial.begin();
				ofRotateY(180.0f);
				ofRotateX(-11.0f);
				ofSetColor(255);
				//				captureImg.getTexture().bind();
				//				screenshotImgs[screenshotIndex].getTexture().bind();
				// live or screenshot images?

				if (bLiveFootage) {
					captureImg.getTexture().bind();
				}
				else {
					screenshotImgs[screenshotIndex].getTexture().bind();
				}
				// simple, just screenshotimages

				if (bDrawFbOnMacs) {
					glPushMatrix();
//					glRotatef(90 * i, 0, 1, 0);
					//				glTranslatef(0, floorSize*0.90625, -floorSize / 2);
//					glTranslatef(0, (floorSize*0.90625) / 2 - h / 2, -floorSize / 2 - 10);
					//		glTranslatef(-floorSize / 2, -floorSize / 2, 0);
					ofPushStyle();
					ofSetColor(255);
					fbScreenshot.getTexture().bind();
					ofPushMatrix();
					//					ofPushMatrix();
			//		ofDrawBox(floorSize, floorSize*0.90625, 1);
					ofDrawBox(ofVec3f(0, -410, -160), 820, 490, 2);
					//					ofPopMatrix();
					//		ofDrawPlane(floorSize,floorSize*0.90625);
					//				ofDrawPlane(1920, 1740);
					fbScreenshot.getTexture().unbind();
		//			ofTranslate(-floorSize*0.09375, -floorSize*0.04023);
					webcamGrabber.getTexture().bind();
					ofPushMatrix();
					glTranslatef(0, 800, 3);
					glRotatef(180, 1, 0, 0);
					/*

					FIKSAA FACEBOOK RUUTUUN ... FRAMERATE LASKEE, KEKSI RATKAISU.

					*/
			//		ofDrawBox(floorSize*0.2484375, floorSize*0.21264, 2);
					ofDrawBox(ofVec3f(0, -110, -20), 220, 190, 2);
					ofPopMatrix();
					webcamGrabber.getTexture().unbind();
					ofPopMatrix();
					ofPopStyle();
					glPopMatrix();
				}
				else {
					ofDrawBox(ofVec3f(0, -410, -160), 820, 490, 2);
				}
				//ofDrawPlane(ofVec3f(0, -410, -160), 820, 490);
				//				screenshotImgs[screenshotIndex].getTexture().unbind();
				//				captureImg.getTexture().unbind();
				if (bLiveFootage) {
					captureImg.getTexture().unbind();
				}
				else {
					screenshotImgs[screenshotIndex].getTexture().unbind();
				}
				//				macScreenMaterial.end();
				ofPopMatrix();
				ofPopStyle();
				glPopMatrix();

//				glTranslatef(0, 0, ((tileCount*tileSize) / 2));
			}

			glPopMatrix();
		}

		// ----------------------- ville 3d model ----------------------- 
		if (bDrawVille) {
			glPushMatrix();
			ofPushStyle();
			ofSetColor(255);
			//		material.begin();
			if (bLiveFootage) {
				captureImg.getTexture().bind();
			}
			else {
				screenshotImgs[screenshotIndex].getTexture().bind();
			}
			modelVille.drawFaces();
			if (bLiveFootage) {
				captureImg.getTexture().unbind();
			}
			else {
				screenshotImgs[screenshotIndex].getTexture().unbind();
			}
			//		material.end();
			ofPopStyle();
			glPopMatrix();
		}

		

	material.end();


	post.end();


	ofDisableLighting();

	if (!bPoint1) { pointLight.disable(); }
	if (!bPoint2) { pointLight2.disable(); }
	if (!bPoint3) { pointLight3.disable(); }
	if (!bWall1) { wallLight1.disable(); }
	if (!bWall2) { wallLight2.disable(); }
	if (!bWall3) { wallLight3.disable(); }
	if (!bAmbient) { ambientLight.disable(); }

	/*
	ofFill();
	ofSetColor(pointLight.getDiffuseColor());
	pointLight.draw();
	ofSetColor(pointLight2.getDiffuseColor());
	pointLight2.draw();
	ofSetColor(pointLight3.getDiffuseColor());
	pointLight3.draw();
	ofSetColor(ambientLight.getDiffuseColor());
	ambientLight.draw();
	ofSetColor(wallLight1.getDiffuseColor());
	wallLight1.draw();
	ofSetColor(wallLight2.getDiffuseColor());
	wallLight2.draw();
	ofSetColor(wallLight3.getDiffuseColor());
	wallLight3.draw();
	*/

//	cam.end();

	ofScreenGrab.loadScreenData(0, 0, w, h);
	
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	unsigned idx = key - '0';
	if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
	switch (key) {
	// camera movements
	case OF_KEY_LEFT:
			moveLeft = true;
		break;
	case OF_KEY_RIGHT:
			moveRight = true;
		break;
	case OF_KEY_UP:
		if (ofGetKeyPressed(OF_KEY_SHIFT)) {
			moveUp = true;
		}
		else {
			moveIn = true;
		}
		break;
	case OF_KEY_DOWN:
		if (ofGetKeyPressed(OF_KEY_SHIFT)) {
			moveDown = true;
		}
		else {
			moveOut = true;
		}
		break;
	case 'w':
		tiltUp = true;
		break;
	case 'a':
		panLeft = true;
		break;
	case 's':
		tiltDown = true;
		break;
	case 'd':
		panRight = true;
		break;
	case 'e':
		rollRight = true;
		break;
	case 'q':
		rollLeft = true;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case 'b':
		bDrawBands = !bDrawBands;
		break;
	case 'f':
		bScreenGrab = !bScreenGrab;
		break;
	case 'g':
		bScreenGrabDraw = !bScreenGrabDraw;
		break;
	case 'i':
		bInfo = !bInfo;
		break;
	case 'x':
		bDrawBeatInfo = !bDrawBeatInfo;
		break;
	case 'c':
		bDrawScreenCubes = !bDrawScreenCubes;
		break;
	case 'p':
//		soundStream.start();
		break;
	case 't':
//		soundStream.stop();
		break;
	// camera movements
	case OF_KEY_LEFT:
		moveLeft = false;
//		panLeft = false;
		break;
	case OF_KEY_RIGHT:
		moveRight = false;
//		panRight = false;
		break;
	case OF_KEY_UP:
		moveUp = false;
		moveIn = false;
//		tiltUp = false;
		break;
	case OF_KEY_DOWN:
		moveDown = false;
		moveOut = false;
//		tiltDown = false;
		break;
	case 'w':
		tiltUp = false;
		break;
	case 'a':
		panLeft = false;
		break;
	case 's':
		tiltDown = false;
		break;
	case 'd':
		panRight = false;
		break;
	case 'e':
		rollRight = false;
		break;
	case 'q':
		rollLeft = false;
		break;
	case '.':
		moveSpeed += 10;
		break;
	case ',':
		moveSpeed -= 10;
		break;
	case '+':
		cubeMovementSpeed += 0.002;
		break;
	case '-':
		cubeMovementSpeed -= 0.002;
		break;
	case 'j':
		bJoystickInfo = !bJoystickInfo;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int wRsz, int hRsz) {
	w = wRsz;
	h = hRsz;
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::exit() {
//	soundStream.stop();
	ofSoundStreamClose();
}




//--------------------------------------------------------------
void ofApp::initScreenGrab()
{

	capture_src_width = ofGetScreenWidth();
	capture_src_height = ofGetScreenHeight();

	capture_dst_width = capture_src_width;
	capture_dst_height = capture_src_height;

	captureImg.allocate(capture_dst_width, capture_dst_height, OF_IMAGE_COLOR);

	// image
	hSourceDC = GetDC(NULL);//GetDC( NULL ); for entire desktop
	hCaptureDC = CreateCompatibleDC(hSourceDC);
	hCaptureBitmap = CreateCompatibleBitmap(hSourceDC, capture_dst_width, capture_dst_height);

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = capture_dst_width;
	bmi.bmiHeader.biHeight = capture_dst_height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	capturedPixelData = new unsigned char[capture_dst_width * capture_dst_height * 3];

}

//--------------------------------------------------------------
void ofApp::updateScreenGrab()
{
	SelectObject(hCaptureDC, hCaptureBitmap);
	StretchBlt(hCaptureDC, 0, capture_dst_height, capture_dst_width, -capture_dst_height, hSourceDC, 0, 0, capture_src_width, capture_src_height, SRCCOPY);
	if (GetDIBits(hCaptureDC, hCaptureBitmap, 0, capture_dst_height, (void*)capturedPixelData, &bmi, DIB_RGB_COLORS) == 0)
	{
		//		cout << "Error: Failed to copy the pixels for hwnd:" << id << flush;
	}
	captureImg.setFromPixels(capturedPixelData, capture_dst_width, capture_dst_height, OF_IMAGE_COLOR, false);
	captureImg.update();
}


void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
	beat.audioReceived(input, bufferSize, nChannels);
}


// Controller bindings - hold + adjustments
/*
			GamePad
			---------------------------------------------
			AXIS#0 = L STICK X	|	AXIS#1 = L STICK Y
			AXIS#2 = R STICK X	|	AXIS#3 = L STICK Y

			0 = []		1 = X		2 = ()		3 = /\

			4 = L1		5 = R1		6 = L2		7 = R2

			10 = L3		11 = L4

			8 = SELECT		9 = START

			12 = UP		13 = RIGHT	14 = DOWN	15 = LEFT

*/
void ofApp::checkCtrl(int jID) {

	// Controller #1 - gamepad ------------------------------

	// Movement speed toggle for X and []
	if (ctrls[jID].btS[1] || ctrls[jID].btS[10]) { // X for faster
		mult += 0.05;
		mult = MIN(mult, 4);
	}
	else if (ctrls[jID].btS[0]) { // [] for slower
		mult -= 0.05;
		mult = MAX(mult, 0.1);
	}
	else {
		if (mult < 0.8) {
			mult += 0.01;
			mult = MIN(mult, 0.8);
		}
		if (mult > 0.8) {
			mult -= 0.05;
			mult = MAX(mult, 0.8);
		}
	}

	if (ctrls[0].btS[4]) { // L1
		float derpX = ofMap(ctrls[0].ax1.x, -100, 100, 0, 255);
		float derpY = ofMap(ctrls[0].ax1.y, -100, 100, 0, 255);
		float derpY2 = ofMap(ctrls[0].ax2.y, -100, 100, 0, 255);
//		lightSpot.setDiffuseColor(ofColor(derpX, derpY, derpY2));
	}


	if (ctrls[0].btS[0]) { // if [] pressed
		if (ctrls[0].ax1.y > 10 || ctrls[0].ax1.y < 10) {
			float derp = ofMap(ctrls[0].ax1.y, -100, 100, -0.15, 0.15);
//			kaleido->setSegments(kaleido->getSegments() + (derp*mult));
		}
	}
	if (ctrls[0].btS[1]) { // if X pressed
		float lerpAdjustY = ofMap(ctrls[0].ax1.y, -100, 100, -1, 1);
//		lerpSpd -= (lerpAdjustY*mult);
	}
	/*
	if (ctrls[0].btS[1]) { // if X is pressed
	if (ctrls[0].ax1.y > 10 || ctrls[0].ax1.y < 10) {
	float derpY = ofMap(ctrls[0].ax1.y, -100, 100, -1, 1);
	float derpX = ofMap(ctrls[0].ax1.x, -100, 100, -0.2, 0.2);
	rgbshift->setAngle(derpY);
	rgbshift->setAmount(derpX);
	}
	}
	*/
	if (ctrls[0].btS[2]) { // if () is pressed
		/*
						   //			if (ctrls[0].ax1.y > 10 || ctrls[0].ax1.y < 10) {
		float derpY = ofMap(ctrls[0].ax1.y, -100, 100, -10, 10);
		float derpY2 = ofMap(ctrls[0].ax2.y, -100, 100, 0, 2);
		//		float derpX = ofMap(ctrls[0].ax1.x, -100, 100, -8, 8);
		zoomblur->setDensity((derpY*mult));
		zoomblur->setDecay((derpY2*mult));
		//	zoomblur->setDecay(derpX);
		//			}
		*/
	}
	/*
	if (ctrls[0].btS[3]) { // if /\ is pressed
	if (ctrls[0].ax1.y > 10 || ctrls[0].ax1.y < 10) {
	float derpY = ofMap(ctrls[0].ax1.x, -100, 100, -1, 1);
	float derpY2 = ofMap(ctrls[0].ax2.y, -100, 100, -1, 1);
	float derpX = ofMap(ctrls[0].ax1.y, -100, 100, -0.7, 0.7);
	/*
	cam.setFov(cam.getFov()+(derpY*mult));
	cam.setDistance(cam.getDistance()+(derpY2*mult));
	dofalt->setFocalDepth(dofalt->getFocalDepth()+(derpX*mult));c
	*
	posZ += derpY;
	}
	}
	*/

}

// Controller bindings - button presses
void ofApp::btnPress(int btn, int jID) {
	cout << "\n Btn " << btn << " released on controller " << jID << "\n";
	// Controller #1 - gamepad bindings when joystick plugged in... lame...

	if (jID == 0) {

		if (btn == 3) { // triangle
			if (ctrls[jID].btS[9]) { // start held
				bCamLocked = !bCamLocked;
			}
			else if (ctrls[jID].btS[8]) { // select held
				camLockedView = ofVec3f(0, -h/3, 0);
			}
			else {
				camLockedView = ofVec3f(cam.getPosition() + (cam.getLookAtDir() * 2500.0f));
			}
		}

		
		if (btn == 0) { // []
			if (ctrls[jID].btS[9]) { // Holding start, releasing [] = toggle kaleido
			//	kaleido->setEnabled(!kaleido->getEnabled());
			}
			if (ctrls[jID].ax1.y > -20 && ctrls[jID].ax1.y < 20 && ctrls[jID].ax1.x < 20 && ctrls[jID].ax1.x > -20) {
		//		float focusVal = ofMap(ctrls[jID].ax1.y, -100, 100, -0.02, 0.02);
		//		cout << ofToString(focusVal) << endl;
		//		dofFocus.set(dofFocus+focusVal);
			}
		}
		if (btn == 1) { // X
			if (ctrls[jID].btS[9]) { // Holding start, releasing X = toggle autoLerp
			}
			if (ctrls[jID].btS[8]) { // Holding select, releasing X = reset Lerp
			}
			else if (ctrls[jID].ax1.y > -20 && ctrls[jID].ax1.y < 20 && ctrls[jID].ax1.x < 20 && ctrls[jID].ax1.x > -20) {
			}
		} // end X
		if (btn == 2) { // ()
			if (ctrls[jID].btS[9]) { // Holding start, releasing () = toggle soundRespo
//				bSoundRespo = !bSoundRespo;
			}
			if (ctrls[jID].ax1.y > -20 && ctrls[jID].ax1.y < 20 && ctrls[jID].ax1.x < 20 && ctrls[jID].ax1.x > -20) {

			}
		}
	}



}


void ofApp::controllerAsCamera(int jID) {

	// false all
	moveUp = moveDown = moveIn = moveOut = moveLeft = moveRight = panLeft = panRight = tiltUp = tiltDown = rollLeft = rollRight = false;

	// ax1 and ax2 values range from -100 to 100

	if (!ctrls[jID].btS[0]) {
		// axis 1 
		if (ctrls[jID].ax1.y < -5) { moveIn = true; }
		if (ctrls[jID].ax1.y > 5) { moveOut = true; }
		if (ctrls[jID].ax1.x < -5) { moveLeft = true; }
		if (ctrls[jID].ax1.x > 5) { moveRight = true; }
	}
	else {
			if (ctrls[jID].ax1.y < -10 || ctrls[jID].ax1.y > 10) {
				float focusVal = ofMap(ctrls[jID].ax1.y, -100, 100, -0.0002, 0.0002);
//				cout << ofToString(focusVal) << endl;
				focusVal = dofFocus + focusVal;
				if (focusVal > dofFocus.getMin() && focusVal < dofFocus.getMax()) {
					dofFocus.set(focusVal);
				}
			}
	}
	// axis 2 
	if (ctrls[jID].ax2.y < -5) { tiltUp = true; }
	if (ctrls[jID].ax2.y > 5) { tiltDown = true; }
	if (ctrls[jID].ax2.x < -5) { panLeft = true; }
	if (ctrls[jID].ax2.x > 5) { panRight = true; }
	if (ctrls[jID].btS[4]) { rollLeft = true; }
	if (ctrls[jID].btS[5]) { rollRight = true; }
	if (ctrls[jID].btS[6]) { moveDown = true; }
	if (ctrls[jID].btS[7]) { moveUp = true; }

//	if (ctrls[jID].btS[10]) { mult = 5; } else { mult = 1; }

	if (bCamLocked) {
		cam.lookAt(camLockedView);
	}

//	infoStr += "worldToCamera: " + ofToString(center) + "\n";


//	if (ctrls[jID].ax2.x < -20) { panLeft = true; }
//	if (ctrls[jID].ax2.x > 20) { panRight = true; }

	if (ctrls[jID].ax1.y > -20 && ctrls[jID].ax1.y < 20 && ctrls[jID].ax1.x < 20 && ctrls[jID].ax1.x > -20) {

	}

	/*
	str = "ax1.x: " + ofToString(ctrls[jID].ax1.x);
	str += " ax1.y: " + ofToString(ctrls[jID].ax1.y);
	str += " | ax2.x: " + ofToString(ctrls[jID].ax2.x);
	str += " ax2.y: " + ofToString(ctrls[jID].ax2.y);
	ofDrawBitmapString(ofToString(str), 100, 160, 0);
	*/

	/*
	< 30
	> 65
	*/

	if (moveUp) { cam.boom(moveSpeed * mult); }
	if (moveDown) { cam.boom(-moveSpeed * mult); }
	if (moveIn) { cam.dolly( (ctrls[jID].ax1.y+5)*0.125 * mult); }
	if (moveOut) { cam.dolly((ctrls[jID].ax1.y-5)*0.125 * mult); }
	if (moveLeft) { cam.truck((ctrls[jID].ax1.x+5)*0.125 * mult); }
	if (moveRight) { cam.truck((ctrls[jID].ax1.x-5)*0.125 * mult); }
	if (tiltUp) { cam.tilt(-(ctrls[jID].ax2.y+5) * 0.0125f * mult/2); }
	if (tiltDown) { cam.tilt(-(ctrls[jID].ax2.y-5) * 0.0125 * mult / 2); }
	if (panLeft) { cam.pan(-(ctrls[jID].ax2.x+5)*0.0125 * mult); }
	if (panRight) { cam.pan(-(ctrls[jID].ax2.x-5)*0.0125 * mult); }
	if (rollLeft) { cam.roll(moveSpeed*0.05f* mult); }
	if (rollRight) { cam.roll(-moveSpeed*0.05f* mult); }
	

}



//--------------------------------------------------------------
void ofApp::setupGui() {

	execParams.setName("Displays");
	execParams.add(bScreenGrab.set("updateScreenGrab", false, false, true));
	execParams.add(bLiveFootage.set("liveFootage", false, false, true));
//	execParams.add(bOfScreenGrab.set("bOfScreenGrab", false, false, true));
	execParams.add(screenshotSpeed.set("screenshotSpeed", 0, 0, 48));
	execGuiPanel.setup(execParams, "", 0, 300);


	drawParams.setName("Objects");
	drawParams.add(bDrawHelperGrid.set("helperGrid", false, false, true));
	drawParams.add(bDrawGridFloor.set("gridFloor", true, false, true));
	drawParams.add(bDrawGridFloorPlane.set("gridFloorPlane", true, false, true));
	drawParams.add(bDrawScreenCubes.set("screenCubes", false, false, true));
	drawParams.add(bDrawVille.set("bDrawVille", false, false, true));
	drawParams.add(bDrawMacs.set("bDrawMacs", false, false, true));
	drawParams.add(macCount.set("macCount", 10, 1, 100));
	drawParams.add(macRotateSpeed.set("macRotateSpeed", 0, -100, 100));
	drawParams.add(macSelfRotateSpeed.set("macSelfRotateSpeed", 0, -100, 100));
	drawParams.add(bDrawFbOnMacs.set("bDrawFbOnMacs", false, false, true));


	drawParams.add(bDrawBands.set("beatBands", true, false, true));
	drawParams.add(beatBandsMult.set("beatSensitivity", 4, 0, 10));
	drawParams.add(but_initRandoms.set("beatRandoms", false, false, true));
	drawParams.add(randomBoxPlus.set("beatRandomsPlus", 0, 0, 200));

	drawGuiPanel.setup(drawParams, "", 200, 300);

	worldParams.setName("World");
	worldParams.add(bInaCube.set("bInaCube", false, false, true));
	worldParams.add(bInaCubeAlpha.set("bInaCubeAlpha", 0, 0, 255));
	worldParams.add(bOfScreenGrabDraw.set("box: ofScreenGrab", false, false, true));
	worldParams.add(bScreenGrabDraw.set("box: screenGrab", false, false, true));
	worldParams.add(bDrawFacebookWall.set("box: bDrawFacebookWall", false, false, true));

	worldGuiPanel.setup(worldParams, "", 400, 300);

	camParams.setName("Camera");
	camParams.add(bCamBreathe.set("bCamBreathe", true, false, true));
	camParams.add(bCamLocked.set("bCamLocked", false, false, true));
	camParams.add(bCamAutomatic.set("bCamAutomatic", false, false, true));
	camParams.add(camAutoMove.set("camAutoMove", ofVec3f(0, 0, 0), ofVec3f(-30, -30, -30), ofVec3f(30, 30, 30)));
	camParams.add(camAutoPan.set("camAutoPan", 0, -30, 30 ));
	camGuiPanel.setup(camParams, "", 600, 300);


	lightParams.setName("Lights");
	lightParams.add(bPoint1.set("bPoint1", true, false, true));
	lightParams.add(bPoint2.set("bPoint2", true, false, true));
	lightParams.add(bPoint3.set("bPoint3", true, false, true));
	lightParams.add(spotsAlpha.set("spotsAlpha", 40.0f, 0.0f, 255.0f));
	lightParams.add(bWall1.set("bWall1", true, false, true));
	lightParams.add(bWall2.set("bWall2", true, false, true));
	lightParams.add(bWall3.set("bWall3", true, false, true));
	lightParams.add(bAmbient.set("bAmbient", false, false, true));
	lightParams.add(spotsCutoff.set("spotsCutoff", 40.0f, 0.0f, 200.0f));
	lightParams.add(spotsConcent.set("spotsConcent", 4.0f, 0.0f, 10.0f));

	lightParams.add(ambientLightAlpha.set("ambientLightAlpha", 0, 0, 255));
	lightsGuiPanel.setup(lightParams, "", 800, 300);

	fxParams.setName("Post fx");
	fxParams.add(zoomBlurDensity.set("zoomBlurDensity", 0, -1, 1));
	fxParams.add(zoomBlurDecay.set("zoomBlurDecay", 1, 0.0, 2.0));
	fxParams.add(dofOn.set("dofOn", true, false, true));
	fxParams.add(dofFocus.set("dofFocus", 0.985, 0.95, 1.05));
	fxParams.add(dofAperture.set("dofAperture", 0.8, 0, 1.5));
	fxParams.add(dofMaxBlur.set("dofMaxBlur", 0.6, 0.3, 1));
	//	fxParams.add(focalDepth.set("focalDepth", 1, 0, 5));
//	fxParams.add(focalLength.set("focalLength", 500, 0, 1300));
	fxGuiPanel.setup(fxParams, "", 1000, 300);


	ofSetBackgroundColor(0);
}
void ofApp::drawGui(ofEventArgs & args) {
//	ofBackground(0);
	execGuiPanel.draw();
//	soundGuiPanel.draw();
	drawGuiPanel.draw();
	worldGuiPanel.draw();
	camGuiPanel.draw();
	lightsGuiPanel.draw();
	fxGuiPanel.draw();

	/*
	glPushMatrix();
		ofPushStyle();
//		material.begin();
//		ofScreenGrab.bind();
		screenshotImgs[screenshotIndex].getTexture().bind();
		ofTranslate(1000, 130);
		ofDrawBox(320, 240, 2);
//		ofScreenGrab.unbind();
		screenshotImgs[screenshotIndex].getTexture().unbind();
//		material.end();
		ofPopStyle();
	glPopMatrix();
	*/

	ofRectangle viewport = ofGetCurrentViewport();
	ofVec3f curLook = cam.getLookAtDir();
//	curLook = cam.cameraToWorld(ofVec3f(curLook.x, curLook.y, curLook.z), viewport);
	curLook.x = roundf(curLook.x * 100) / 100;
	curLook.y = roundf(curLook.y * 100) / 100;
	curLook.z = roundf(curLook.z * 100) / 100;


	ofVec3f worldCenter = cam.worldToCamera(ofVec3f(0, 0, 0), viewport);
	worldCenter.x = roundf(worldCenter.x * 100) / 100;
	worldCenter.y = roundf(worldCenter.y * 100) / 100;
	worldCenter.z = roundf(worldCenter.z * 100) / 100;

//	if (bInfo) {
		string infoStr;
		infoStr = "campos: " + ofToString(cam.getPosition()) + "\n";
//		infoStr += "camLookAt: " + ofToString(cam.getLookAtDir()) + "\n";
		infoStr += "curLook: " + ofToString(curLook) + "\n";
//		infoStr += "curLookWorld: " + ofToString(curLookWorld) + "\n";
		infoStr += "worldCenter: " + ofToString(worldCenter) + "\n";
		//		infoStr += "curLookWorld: " + ofToString(curLookWorld) + "\n";
//		infoStr += "camPosToWorld: " + ofToString(cam.cameraToWorld(cam.getPosition(),viewport)) + "\n";
		infoStr += "movespeed: " + ofToString(moveSpeed) + "\n";
		infoStr += "fps: " + ofToString(ofGetFrameRate()) + "\n";
		ofSetColor(255);
		ofDrawBitmapString(ofToString(infoStr), 200, 10, 0);
//	}


//	if (bJoystickInfo) {
		ofxGLFWJoystick::one().drawDebug(300, 100);
		for (int i = 0; i < ctrls.size(); i++) {
			string jSstr;
			jSstr = "ax1.x: " + ofToString(ctrls[i].ax1.x) + "\n";
			jSstr += "ax1.y: " + ofToString(ctrls[i].ax1.y) + "\n";
			jSstr += "ax2.x: " + ofToString(ctrls[i].ax2.x) + "\n";
			jSstr += "ax2.y: " + ofToString(ctrls[i].ax2.y) + "\n";
			jSstr += "mult: " + ofToString(mult);
			ofDrawBitmapString(ofToString(jSstr), 420, 160, 0);
		}
//	}


//		if (bDrawBeatInfo) {
		string beatStr;
		beatStr = "kick: " + ofToString(beat.kick()) + "\n";
		beatStr += "snare: " + ofToString(beat.snare()) + "\n";
		beatStr += "hihat: " + ofToString(beat.hihat()) + "\n";
		beatStr += "isBeat: " + ofToString(beat.isBeat(1)) + "\n";
		ofDrawBitmapString(ofToString(beatStr), 200, 160, 0);
//		}

		for (int bC = 0; bC < beatCounter; bC++) {
			ofPushStyle();
			ofFill();
			ofSetColor(0,255,0);
			ofDrawRectangle(200 + bC * 25, 260, 20, 20);
			ofPopStyle();
		}

		for (unsigned i = 0; i < post.size(); ++i)
		{
			ofPushStyle();
			if (post[i]->getEnabled()) ofSetColor(0, 255, 255);
			else ofSetColor(255, 0, 0);
			ostringstream oss;
			oss << i << ": " << post[i]->getName() << (post[i]->getEnabled() ? " (on)" : " (off)");
			ofDrawBitmapString(oss.str(), 1000, 20 * (i + 2) + 400);
			ofPopStyle();
		}

}


void ofApp::initRandomBoxes() {

	randomBeatBoxes.clear();

	randomBoxCount = int(ofRandom(60) + 120 + randomBoxPlus);

	for (int i = 0; i < randomBoxCount; i++) {
		randomBox newBox;

		int randomXtimes = (ofRandom(tileCount) - tileCount / 2);
		int randomZtimes = (ofRandom(tileCount) - tileCount / 2);

		float pX = randomXtimes*bandCubeSize - 1;
		float pY = 1;
		//		float pZ = -bI*bandCubeSize - (bI*tileGap);
		float pZ = randomZtimes*bandCubeSize - 1;
		newBox.setPosition(pX, pY, pZ);

		float sX = bandCubeSize - 2;
		float sY = bandCubeSize;
		//		float sY = bandCubeSize*band * beatBandsMult;
		float sZ = bandCubeSize - 2;
		newBox.set(sX, sY, sZ);

		newBox.listeningTo = int(ofRandom(bands.size()));
		randomBeatBoxes.push_back(newBox);

	}
}