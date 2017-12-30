#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxFlowTools.h"

#include "ofxDatGui.h"
#include "ofxGui.h"

#include "DFO.hpp"
#include "PSO.hpp"

using namespace flowTools;

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
        void draw();
    
        void onSliderEvent(ofxDatGuiSliderEvent e);
        void onDropdownEvent(ofxDatGuiDropdownEvent e);
    
        void audioOut(float * output, int bufferSize, int nChannels);
        double foldback(double in, double threshold);
    
		void keyPressed(int key);
    
        void drawOpticalFlow() {
            drawOpticalFlow(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
        }
        void drawOpticalFlow(int _x, int _y, int _width, int _height);

    //DFO
    DFO dfo = DFO(15);
    PSO pso = PSO(15);
    
    //Video Source
    //ofVideoPlayer video;
    ofVideoGrabber video;
    ftFbo cameraFbo;

    // FlowTools
    int flowWidth;
    int flowHeight;
    int drawWidth;
    int drawHeight;
    
    ftOpticalFlow opticalFlow;
    ftVelocityField velocityField;
    
    // GUI
    ofxDatGui* gui;
    ofxPanel guiMotionTrack;

    int siAlgo;
    int topology;
    float dt;
    double y = 0.0;
    
    // Save velocity inti
    ofPixels velocityPix;

    //Sound
    ofSoundStream soundStream;
    maxiOsc mySine, myOtherSine;
    maxiDelayline delay;
    maxiFilter filter;
    
    int sampleRate = 44100;
    int bufferSize = 512;
    float sound;
    float amp = 0.9;
    float thres = 0.6;
};
