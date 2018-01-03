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
    
    // GUISetup
    guiMotionTrack.setup();
    guiMotionTrack.add(opticalFlow.parameters);
    guiMotionTrack.setDefaultWidth(200);
    guiMotionTrack.setPosition(0,0);
    
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_RIGHT );
    gui->setAssetPath("");
    
    gui->addHeader(":: Swarmotion Parameters ::");
    gui->setPosition(0,230);
    gui->setWidth(200);
    
    //Gui Switch Algo
    vector<string> algoOptions = { "DFO", "PSO"};
    gui->addDropdown("Pick Algorithm", algoOptions);
    gui->addBreak();
    vector<string> topOptions = { "Ring Topology", "Random Topology", "Closest Topology", "Elitist Approach" };
    gui->addDropdown("Select Topology", topOptions);
    gui->addBreak();
    gui->addSlider("Disturb", 0.001, 0.09, 0.001);
    gui->addToggle("Mute Sound");
    gui->addFooter();
    
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    
    // if the settings file is not present the parameters will not be set during this setup
    if (!ofFile("settings.xml"))
        guiMotionTrack.saveToFile("settings.xml");
    
    guiMotionTrack.loadFromFile("settings.xml");
    
    //Sound Setup
    soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);
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
                dfo.find(velocityPix, topology, dt, isElitist);
                //cout << dfo.swarm[dfo.bestIndex]->getFitness() << endl;
                break;
            case 1:
                pso.find(velocityPix, topology, dt, isElitist);
                //cout << pso.particles[dfo.bestIndex]->getFitness() << endl;
                break;
        }
    }
}

//--------------------------------------------------------------

void ofApp::draw(){
    ofBackground(0);
    
    cameraFbo.draw(200,drawHeight, drawWidth, drawHeight);
    
    ofPushStyle();
    {
        for (int x = 0; x < velocityPix.getWidth(); x++){
            for (int y = 0; y < velocityPix.getHeight(); y++){
                ofColor color = velocityPix.getColor(x,y);
                ofSetColor(color);
                ofFill();
                ofDrawRectangle((ofGetWidth() + x*4) - drawWidth, y*4, 4,  4);
            }
        }
    }
    ofPopStyle();
    
    switch(siAlgo){
        case 0: dfo.render(velocityPix); break;
        case 1: pso.render(velocityPix); break;
    }
    
    guiMotionTrack.draw();
    
    
    ofDrawBitmapString("Motion Feed", 220, 20);
    ofDrawBitmapString("Webcam Feed", 220, drawHeight + 20);
}

//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
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
        
        double delayedSound = delay.dl(sound, 2000, 0.4);
        double loresSound = filter.lores(delayedSound, 3000, 0.06);
        float distortedSound = foldback(loresSound, 0.0004);
        
        if (mute){
            output[i*nChannels] = 0.0;
            output[i*nChannels + 1] = 0.0;
        } else if (!mute) {
            output[i*nChannels] = ofClamp(distortedSound * amp, -1,1);
            output[i*nChannels + 1] = output[i*nChannels];
        }
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
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    string guiLabel = e.target->getLabel();
    
    if(guiLabel == "Disturb"){
        dt = e.value;
    }
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    string guiLabel = e.target->getLabel();
    
    if( guiLabel == "DFO"){
        siAlgo = 0;
    } else if (guiLabel == "PSO") {
        siAlgo = 1;
    }
    
    if(guiLabel == "Ring Topology"){
        topology = 0;
        isElitist = 0;
    } else if (guiLabel == "Random Topology"){
        topology = 1;
        isElitist = 0;
    } else if (guiLabel == "Closest Topology"){
        topology = 2;
        isElitist = 0;
    } else if (guiLabel == "Elitist Approach"){
        isElitist = 1;
        if (siAlgo == 0){
            for (auto & f : dfo.swarm){
                f->setPos(0, ofRandom(120));
                f->setPos(1, ofRandom(90));
            }
        } else if(siAlgo == 1){
            for (auto & p : pso.particles){
                p->setPos(0, ofRandom(120));
                p->setPos(1, ofRandom(90));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::onToggleEvent(ofxDatGuiToggleEvent e){
    string guiLabel = e.target->getLabel();
    
    if(guiLabel == "Mute Sound"){
        mute = e.target->getChecked();
    }
}
