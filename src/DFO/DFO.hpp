//
//  DFO.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Fly.hpp"

class DFO{
private:
public:
    DFO(int population);
    void find(ofPixels &searchSpace);
    float highestVel(ofVec2f velocity);
    void render(ofPixels &searchSpace);
    void keypressed(int key);
    double output();
    
    vector<Fly> swarm;

    int dimensions = 2;
    int bestIndex = 0;
    
    double bestFitness = std::numeric_limits<float>::max();;
    double sound = 0.0;
    
    ofParameterGroup parameters;
    ofParameter<float> dt;
    ofParameter<int> topology;
};
