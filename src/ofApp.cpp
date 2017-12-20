#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    drawWidth = 480;
    drawHeight = 360;
    
    // process all but the density on 16th resolution
    flowWidth = drawWidth / 4;
    flowHeight = drawHeight / 4;
    
    // FLOW & MASK
    opticalFlow.setup(flowWidth, flowHeight);
    
    // VISUALIZATION
    velocityField.setup(flowWidth / 4, flowHeight / 4);
    
    // CAMERA
    video.setup(drawWidth, drawHeight, true);
    cameraFbo.allocate(drawWidth, drawHeight);
    cameraFbo.black();
    
    // GUI
    gui.setup("Optical Flow");
    gui.setPosition(0,0);
    gui.minimizeAll();
    
    gui.add(siAlgo.set("Algorithm", 0, 0, 1));
    gui.add(opticalFlow.parameters);
    gui.add(dfo.parameters);
    gui.add(pso.parameters);
    
    // if the settings file is not present the parameters will not be set during this setup
    if (!ofFile("settings.xml"))
        gui.saveToFile("settings.xml");
    gui.loadFromFile("settings.xml");
    
    //Sound Setup
    soundStream.setup(this, 2, 0, sampleRate, bufferSize, 1);
    soundStream.setDeviceID(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    
    if (video.isFrameNew()) {
        ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
                video.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
            cameraFbo.end();
        ofPopStyle();
        
        opticalFlow.setSource(cameraFbo.getTexture());
        opticalFlow.update();
    
        //Get the flow decay velocity reading from the texture
        opticalFlow.getOpticalFlowDecay().readToPixels(velocityPix);
        
        switch(siAlgo){
            case 0:
                //Only need .getColor().r and .getColour.g from "velocityPix", as this is equal to x and y of velocity
                dfo.find(velocityPix);
                siAlgo.setName("DFO");
                break;
            case 1:
                pso.update(velocityPix);
                siAlgo.setName("PSO");
                break;
        }
    }
}

//--------------------------------------------------------------

void ofApp::draw(){
    ofBackground(0);
    cameraFbo.draw(200,0, drawWidth, drawHeight);
    ofPushStyle();
    {
        for (int x = 0; x < velocityPix.getWidth(); x++){
            for (int y = 0; y < velocityPix.getHeight(); y++){
                ofColor color = velocityPix.getColor(x,y);
                ofSetColor(color);
                ofFill();
                ofDrawRectangle((ofGetWidth() + x*4) - drawWidth, drawHeight + y*4, 4,  4);
            }
        }
    }
    ofPopStyle();
    
    switch(siAlgo){
        case 0:
            dfo.render(velocityPix);
        break;
        case 1:
            pso.draw(velocityPix);
        break;
    }
    
    gui.draw();
    
 
}

//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    for (unsigned i = 0; i < bufferSize; i++) {
        
        switch(siAlgo){
            case 0:
                sound = dfo.output();
            break;
            case 1:
                sound = pso.output();
            break;
        }
        
        double delayedSound = delay.dl(sound, 3000, 0.6);
        double loresSound = filter.lores(delayedSound, 4000, 0.06);
        //float distortedSound = foldback(loresSound, 0.3);

        //distortedSound = ofClamp(distortedSound, -1 ,1);
        loresSound = ofClamp(loresSound , -1, 1);
        output[i*nChannels] = loresSound * amp;
        output[i*nChannels + 1] = output[i*nChannels];
    }
}

//--------------------------------------------------------------

double ofApp::foldback(double in, double threshold){
    if (in > thres || in < -threshold){
        in = fabs(fabs(fmod(in - thres, threshold*6)) - threshold * 4) - threshold;
    }
    
    return in;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    dfo.keypressed(key);
}
