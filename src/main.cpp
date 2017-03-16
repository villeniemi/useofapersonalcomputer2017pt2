#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

/* Technology is never pure and innocent, but it is always made by human (usually male) beings,
who have their own social, cultural, economical and political reasons to do so.
*/

//========================================================================
int main( ){
//	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// ofAppGLFWWindow window;
	/*
	ofAppGLFWWindow window;
	window.setWindowPosition(-1680, 27);
	ofSetupOpenGL(&window, 1680, 1050, OF_WINDOW);
	window.listMonitors();
	*/
	ofGLFWWindowSettings settings;
//	settings.width = 1680;
//	settings.height = 1050;
	settings.width = 1920;
	settings.height = 1080;
//	settings.setPosition(ofVec2f(-1680, 27));
	settings.setPosition(ofVec2f(-1920, 0));
	settings.resizable = true;
	settings.monitor = 1;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 1500;
	settings.height = 600;
	settings.setPosition(ofVec2f(20, 20));
	settings.resizable = true;
	shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

//	guiWindow->setVerticalSync(false);

	shared_ptr<ofApp> mainApp(new ofApp);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
//	ofRunApp(new ofApp());

	mainApp->setupGui();
	ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);

	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
