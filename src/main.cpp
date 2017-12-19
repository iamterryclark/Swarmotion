#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
    windowSettings.setGLVersion(4, 1);
#endif
    windowSettings.width = 680;
    windowSettings.height = 360 * 2;
    windowSettings.windowMode = OF_WINDOW;
    
    ofCreateWindow(windowSettings);
    
    ofRunApp(new ofApp());
}
