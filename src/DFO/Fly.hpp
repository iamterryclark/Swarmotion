//
//  Fly.hpp
//  Swarmotion
//
//  Created by Terry Clark on 19/12/2017.
//

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"

class Fly{
private:
    vector<double> position;
    double fitness = 0.0;
    int type;
    maxiOsc osc, osc2, phaser;
    maxiFilter filter, filter2;
    
public:
    
    Fly(){
        type = (int)ofRandom(0);
    }
    
    void setPos(vector<double> _pos){
        position = _pos;
    }
    
    vector<double> getPos(){
        return position;
    }
    
    void setFitness(double _fit){
        fitness = _fit;
    }
    
    double getFitness(){
        return fitness;
    }
    
    ofVec2f getDistance2vec(Fly &fly){
        double xDist = position[0] - fly.getPos()[0];
        double yDist = position[1] - fly.getPos()[1];
        return ofVec2f(xDist, yDist);
    }
    
    double getDistance(Fly &fly){
        return ofDistSquared(position[0], position[1], fly.getPos()[0], fly.getPos()[1]);
    }
    
    double output(double param1, double param2){
        switch(type){
            case 0:
                return osc.sinewave(param1*param2);
                break;
        }
    }
    
};

