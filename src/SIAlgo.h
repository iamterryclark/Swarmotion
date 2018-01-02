//
//  SIAlgo.h
//  Swarmotion
//
//  Created by Terry Clark on 26/12/2017.
//

#pragma once

#include "ofMain.h"
#include "SIPoint.h"

class SIAlgo {
public:
    
    virtual void find(ofPixels &searchSpace, int topology, float dt, int isElitist) = 0;
    virtual void render(ofPixels &searchSpace) = 0;
    virtual double output() = 0;
    
    double highestVel(ofVec2f _velocity){
        //2 Part Fitness function
        float a = 255 - _velocity.x;
        float b = 255 - _velocity.y;
        return (a + b)/2;
    }
    
    ofVec2f getVelocity(ofColor color){
        ofVec2f velocity = ofVec2f(color.r, color.g);
        return velocity;
    }
};
