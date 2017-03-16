#pragma once

#include "ofMain.h"
#include "ofxBeat.h"
#include "controller.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int wRsz, int hRsz);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofEasyCam cam;
			float moveSpeed;
			bool moveUp, moveDown, moveLeft, moveRight, moveIn, moveOut;
			bool tiltUp, tiltDown, panLeft, panRight, rollLeft, rollRight;

			ofParameter<bool> bCamLocked;

			ofParameter<bool> bCamBreathe;
			ofParameter<bool> bCamAutomatic;
			ofParameter<ofVec3f> camAutoMove;
			ofParameter<float> camAutoPan;
			
			ofVec3f camLockedView;
			ofParameterGroup camParams;
			ofxPanel camGuiPanel;


		bool bInfo;
		int w, h;
		ofShader shader;
		ofFbo fbo;



		// Post FX
		ofxPostProcessing post;
		KaleidoscopePass::Ptr kaleido;
		RGBShiftPass::Ptr rgbshift;
		ZoomBlurPass::Ptr zoomblur;
		NoiseWarpPass::Ptr noisewarp;
	//	DofAltPass::Ptr dofalt;
		DofPass::Ptr dof;
		
		ofxPanel fxGuiPanel;
		ofParameterGroup fxParams;

		ofParameter <float> zoomBlurDensity;
		ofParameter <float> zoomBlurDecay;

//		ofParameter <int> focalDepth;
//		ofParameter <int> focalLength;
		ofParameter<bool> dofOn;
		ofParameter <float> dofFocus;
		ofParameter <float> dofAperture;
		ofParameter <float> dofMaxBlur;


		// Controller
			vector<Controller> ctrls;
			// ctrls Bools
			void checkCtrl(int jID);
			void btnPress(int n, int jID);
			bool bJoystickInfo;

			void controllerAsCamera(int jID);

			float mult;

		// Full Desktop Capture
			void initScreenGrab();
			void updateScreenGrab();
			ofParameter<bool> bScreenGrab;
			ofParameter<bool> bScreenGrabDraw;

			int capture_src_width;
			int capture_src_height;
			int capture_dst_width;
			int capture_dst_height;
			HDC hSourceDC;
			HDC hCaptureDC;
			HBITMAP hCaptureBitmap;

			ofImage captureImg;
			BITMAPINFO bmi = { 0 };

			ofPixels capturedPixels;
			unsigned char* capturedPixelData;

			ofParameter<bool> bOfScreenGrabDraw;
			ofParameter<bool> bOfScreenGrab;
			ofTexture ofScreenGrab;


		// Desktop cubes
			ofParameter<bool> bDrawScreenCubes;
			int boxCount;
			int boxRows;
			int boxCols;
			float splitW, splitH;
			vector<ofFbo> captureImgSplits;
			float cubeMovementSpeed = 0.01;






		// Floor

		float floorSize;
//			bool bDrawGridFloor;
		ofParameter<bool> bDrawHelperGrid;
		ofParameter<bool> bDrawGridFloor;
		ofParameter<bool> bDrawGridFloorPlane;
			int tileCount = 200; // per dimension
			int tileSize = 60;
			int tileHeight = 1;

			int tileGap = 0;

			float roof;
			float floor;

		// Sound
			ofxBeat beat;
			int beatCounter;
			uint64_t beatTimer;
			void audioReceived(float*, int, int);
			
			ofParameter<bool> bDrawBands;
			ofParameter<float> beatBandsMult;

			// Band Cubes

				int bandCubeSize = 60;

				class beatBand {
				public:
					vector<float> band;
					float bandAv;
					float bandTot;
				};
				vector<beatBand> bands;
				int bandAvLength;

			// Random Beat Boxes
				void initRandomBoxes();
				int randomBoxCount;
				ofParameter<float> randomBoxPlus;
				ofParameter<bool> but_initRandoms;
				class randomBox : public ofBoxPrimitive {
				public:
					int listeningTo;
					float pY, sY;
				};
				vector<randomBox> randomBeatBoxes;

				
		// 3D model

			ofParameter<bool> bDrawVille;
			ofxAssimpModelLoader modelVille;
			ofxAssimpModelLoader modelMac;
			ofMaterial macScreenMaterial;

			ofParameter<bool> bDrawMacs;

			ofParameter<int> macCount = 10;
			ofParameter<bool> bRotateMacs;
			float macRotateAmount;
			ofParameter<float> macRotateSpeed;
			ofParameter<float> macSelfRotateSpeed;
			vector<float> macSelfRotateAmount;

		// Lights

			ofLight pointLight;
			ofLight pointLight2;
			ofLight pointLight3;
			ofMaterial material;

			ofLight ambientLight; // creates a light and enables lighting
			ofLight wallLight1;
			ofLight wallLight2;
			ofLight wallLight3;

			ofxPanel lightsGuiPanel;
			ofParameterGroup lightParams;
			ofParameter<bool> bPoint1;
			ofParameter<bool> bPoint2;
			ofParameter<bool> bPoint3;
			ofParameter<bool> bWall1;
			ofParameter<bool> bWall2;
			ofParameter<bool> bWall3;
			ofParameter<bool> bAmbient;
			ofParameter<int> ambientLightAlpha;
			ofParameter<float> spotsCutoff;
			ofParameter<float> spotsConcent;

			ofParameter<float> spotsAlpha;


			float lightCircleCount;



		// Footage

			ofParameter<bool> bLiveFootage;

			ofParameter<int> screenshotSpeed;
//			int   appFPS;
//			float sequenceFPS;
//			bool  bFrameIndependent;
			vector <ofImage> screenshotImgs;
			int screenshotIndex;

			ofImage fbScreenshot;
			ofVideoGrabber webcamGrabber;
			ofParameter<bool> bDrawFacebookWall;

			ofParameter<bool> bDrawFbOnMacs;

		// GUI and settings
			void drawGui(ofEventArgs & args);
			void setupGui();
			ofParameterGroup drawParams;
			ofParameterGroup execParams;
			ofParameter<int> dotSpeed;
			ofParameter<float> radius;
			ofParameter<ofColor> color;

			ofParameter<bool> bDrawBeatInfo;

			ofxPanel drawGuiPanel;
			ofxPanel execGuiPanel;

			ofParameter<bool> bInaCube;
			ofParameter<float> bInaCubeAlpha;
			ofxPanel worldGuiPanel;
			ofParameterGroup worldParams;

};